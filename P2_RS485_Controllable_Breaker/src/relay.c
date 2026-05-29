#include <errno.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "relay.h"

LOG_MODULE_REGISTER(relay, LOG_LEVEL_INF);

#define RELAY_NODE DT_ALIAS(relay)

static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(RELAY_NODE, gpios);
static bool relay_state;

int relay_init(void)
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
	LOG_INF("Relay initialized");

	return 0;
}

int relay_set(bool on)
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

bool relay_get(void)
{
	return relay_state;
}
