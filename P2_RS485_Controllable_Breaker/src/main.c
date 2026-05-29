/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

#define RELAY_NODE DT_ALIAS(relay)

static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(RELAY_NODE, gpios);

int main(void)
{
	int ret;

	LOG_INF("Initializing relay...");
	if (!gpio_is_ready_dt(&relay)) {
		LOG_ERR("Relay device is not ready");
		return 0;
	}

	ret = gpio_pin_configure_dt(&relay, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure relay");
		return 0;
	}

	LOG_INF("Relay initialized successfully");

	bool state = false;

	while (1)
	{
		state = !state;
		ret = gpio_pin_set_dt(&relay, state);
		if (ret < 0) {
			LOG_ERR("Failed to set relay state: %d", ret);
		} else {
			LOG_INF("Relay set to %d", state);
		}

		k_sleep(K_SECONDS(1));
	}

	return 0;
}
