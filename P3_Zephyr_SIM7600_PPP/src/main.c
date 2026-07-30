#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/cellular.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>

LOG_MODULE_REGISTER(sim7600_modem_test, LOG_LEVEL_INF);

#define APP_EVENT_MODEM_INFO BIT(0)
#define APP_EVENT_REG_CHANGED BIT(1)

#define MODEM_BOOT_DELAY K_SECONDS(30)
#define MODEM_SETTLE_DELAY K_SECONDS(8)
#define MODEM_POLL_INTERVAL K_SECONDS(10)
#define CONSOLE_READY_TIMEOUT_MS 15000
#define CONSOLE_READY_POLL_MS 100

static const struct device *const modem = DEVICE_DT_GET(DT_ALIAS(modem));
static const struct device *const console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
static const struct gpio_dt_spec modem_power_gpio =
	GPIO_DT_SPEC_GET(DT_NODELABEL(power_ctrl), gpios);

K_EVENT_DEFINE(app_events);

static void wait_for_console_ready(void)
{
	uint32_t dtr = 0;
	int64_t deadline;
	int ret;

	if (!device_is_ready(console_dev)) {
		return;
	}

	deadline = k_uptime_get() + CONSOLE_READY_TIMEOUT_MS;

	while (k_uptime_get() < deadline) {
		ret = uart_line_ctrl_get(console_dev, UART_LINE_CTRL_DTR, &dtr);
		if (ret == 0 && dtr != 0U) {
			break;
		}

		k_sleep(K_MSEC(CONSOLE_READY_POLL_MS));
	}

	k_sleep(K_MSEC(250));
}

static const char *registration_status_to_string(enum cellular_registration_status status)
{
	switch (status) {
	case CELLULAR_REGISTRATION_NOT_REGISTERED:
		return "not registered";
	case CELLULAR_REGISTRATION_REGISTERED_HOME:
		return "registered home";
	case CELLULAR_REGISTRATION_SEARCHING:
		return "searching";
	case CELLULAR_REGISTRATION_DENIED:
		return "denied";
	case CELLULAR_REGISTRATION_UNKNOWN:
		return "unknown";
	case CELLULAR_REGISTRATION_REGISTERED_ROAMING:
		return "registered roaming";
	case CELLULAR_REGISTRATION_SMS_ONLY_HOME:
		return "sms only home";
	case CELLULAR_REGISTRATION_SMS_ONLY_ROAMING:
		return "sms only roaming";
	case CELLULAR_REGISTRATION_EMERGENCY_ONLY:
		return "emergency only";
	case CELLULAR_REGISTRATION_CSFB_NOT_PREFERRED_HOME:
		return "csfb not preferred home";
	case CELLULAR_REGISTRATION_CSFB_NOT_PREFERRED_ROAMING:
		return "csfb not preferred roaming";
	case CELLULAR_REGISTRATION_RLOS:
		return "rlos";
	default:
		return "unhandled";
	}
}

static void log_modem_info_field(enum cellular_modem_info_type field, const char *label)
{
	char value[64];
	int ret;

	ret = cellular_get_modem_info(modem, field, value, sizeof(value));
	if (ret == 0) {
		LOG_INF("%s: %s", label, value);
	} else {
		LOG_INF("%s unavailable yet (%d)", label, ret);
	}
}

static void log_signal_info(void)
{
	int16_t rssi;
	int ret;

	ret = cellular_get_signal(modem, CELLULAR_SIGNAL_RSSI, &rssi);
	if (ret == 0) {
		LOG_INF("RSSI: %d dBm", rssi);
	} else {
		LOG_INF("RSSI unavailable yet (%d)", ret);
	}
}

static void log_registration_status(void)
{
	enum cellular_registration_status status;
	int ret;

	ret = cellular_get_registration_status(modem, CELLULAR_ACCESS_TECHNOLOGY_E_UTRAN,
					       &status);
	if (ret == 0) {
		LOG_INF("Registration status: %s (%d)", registration_status_to_string(status), status);
	} else {
		LOG_INF("Registration status unavailable yet (%d)", ret);
	}
}

static void log_modem_snapshot(void)
{
	LOG_INF("----- modem snapshot -----");
	log_signal_info();
	log_registration_status();
	log_modem_info_field(CELLULAR_MODEM_INFO_IMEI, "IMEI");
	log_modem_info_field(CELLULAR_MODEM_INFO_MODEL_ID, "Model");
	log_modem_info_field(CELLULAR_MODEM_INFO_MANUFACTURER, "Manufacturer");
	log_modem_info_field(CELLULAR_MODEM_INFO_SIM_IMSI, "IMSI");
	log_modem_info_field(CELLULAR_MODEM_INFO_SIM_ICCID, "ICCID");
	log_modem_info_field(CELLULAR_MODEM_INFO_FW_VERSION, "FW");
}

static void modem_event_cb(const struct device *dev, enum cellular_event event, const void *payload,
			   void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	switch (event) {
	case CELLULAR_EVENT_MODEM_INFO_CHANGED: {
		const struct cellular_evt_modem_info *info = payload;

		if (info != NULL) {
			LOG_INF("Modem info changed, field=%d", info->field);
		} else {
			LOG_INF("Modem info changed");
		}
		k_event_post(&app_events, APP_EVENT_MODEM_INFO);
		break;
	}

	case CELLULAR_EVENT_REGISTRATION_STATUS_CHANGED: {
		const struct cellular_evt_registration_status *reg = payload;

		if (reg != NULL) {
			LOG_INF("Registration changed: %s (%d)",
				registration_status_to_string(reg->status), reg->status);
		} else {
			LOG_INF("Registration changed");
		}
		k_event_post(&app_events, APP_EVENT_REG_CHANGED);
		break;
	}

	case CELLULAR_EVENT_MODEM_COMMS_CHECK_RESULT: {
		const struct cellular_evt_modem_comms_check_result *result = payload;

		if (result != NULL) {
			LOG_INF("Modem comms check: %s", result->success ? "ok" : "failed");
		} else {
			LOG_INF("Modem comms check completed");
		}
		break;
	}

	default:
		break;
	}
}

int main(void)
{
	int64_t power_asserted_ms;
	int64_t resume_ms;
	int ret;

	if (!device_is_ready(modem)) {
		LOG_ERR("Modem device is not ready");
		return -ENODEV;
	}

	if (!gpio_is_ready_dt(&modem_power_gpio)) {
		LOG_ERR("Modem power GPIO is not ready");
		return -ENODEV;
	}

	ret = cellular_set_callback(modem,
				    CELLULAR_EVENT_MODEM_INFO_CHANGED |
					    CELLULAR_EVENT_REGISTRATION_STATUS_CHANGED |
					    CELLULAR_EVENT_MODEM_COMMS_CHECK_RESULT,
				    modem_event_cb, NULL);
	if (ret != 0) {
		LOG_ERR("Failed to register modem callback: %d", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&modem_power_gpio, GPIO_OUTPUT_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Failed to assert modem power GPIO: %d", ret);
		return ret;
	}

	power_asserted_ms = k_uptime_get();
	LOG_INF("SIM7600 power asserted at %lld ms, waiting %u ms before resume",
		power_asserted_ms, (uint32_t)k_ticks_to_ms_floor64(MODEM_BOOT_DELAY.ticks));
	k_sleep(MODEM_BOOT_DELAY);
	resume_ms = k_uptime_get();
	LOG_INF("SIM7600 boot wait complete at %lld ms, elapsed %lld ms",
		resume_ms, resume_ms - power_asserted_ms);

	ret = pm_device_action_run(modem, PM_DEVICE_ACTION_RESUME);
	if (ret != 0 && ret != -EALREADY) {
		LOG_ERR("Failed to resume modem: %d", ret);
		return ret;
	}

	wait_for_console_ready();

	LOG_INF("SIM7600 modem bring-up test boot");
	LOG_INF("Resuming SIM7600 modem");
	LOG_INF("Waiting for modem to settle");
	k_sleep(MODEM_SETTLE_DELAY);

	log_modem_snapshot();

	while (true) {
		uint32_t events;

		events = k_event_wait(&app_events, APP_EVENT_MODEM_INFO | APP_EVENT_REG_CHANGED,
				      true, MODEM_POLL_INTERVAL);

		if (events != 0U) {
			LOG_INF("Observed modem event mask: 0x%08x", events);
		}

		log_modem_snapshot();
	}

	return 0;
}
