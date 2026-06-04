#include <errno.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "relay_controller.h"

LOG_MODULE_REGISTER(relay_controller, LOG_LEVEL_INF);

#define RELAY_NODE DT_ALIAS(relay)

static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(RELAY_NODE, gpios);
static bool relay_state;

static int relay_read_holding_reg(uint16_t offset, uint16_t *value)
{
	switch (offset) {
	case 0:
		*value = relay_controller_get() ? 1U : 0U;
		return 0;
	default:
		return -ENOTSUP;
	}
}

static int relay_write_holding_reg(uint16_t offset, uint16_t value)
{
	switch (offset) {
	case 0:
		if (value > 1U) {
			return -EINVAL;
		}

		return relay_controller_set(value == 1U);
	default:
		return -ENOTSUP;
	}
}

static const struct modbus_register_handler relay_register_handler = {
	.start_addr = 0U,
	.reg_count = 1U,
	.read_holding_reg = relay_read_holding_reg,
	.write_holding_reg = relay_write_holding_reg,
};

int relay_controller_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&relay)) {
		LOG_ERR("Relay device is not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&relay, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure relay: %d", ret);
		return ret;
	}

	relay_state = false;
	LOG_INF("Relay controller initialized");

	return 0;
}

int relay_controller_set(bool on)
{
	int ret;

	ret = gpio_pin_set_dt(&relay, on);
	if (ret < 0) {
		LOG_ERR("Failed to set relay state: %d", ret);
		return ret;
	}

	relay_state = on;
	LOG_INF("Relay set to %d", (int)on);

	return 0;
}

bool relay_controller_get(void)
{
	return relay_state;
}

const struct modbus_register_handler *relay_controller_get_register_handler(void)
{
	return &relay_register_handler;
}
