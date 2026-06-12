#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "modbus_client.h"
#include "modbus_server.h"
#include "relay_controller.h"
#include "system_registers.h"
#include "sht20.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
	int ret;

	ret = relay_controller_init();
	if (ret < 0) {
		LOG_ERR("Relay init failed: %d", ret);
		return 0;
	}

	ret = modbus_server_register_handler(relay_controller_get_register_handler());
	if (ret < 0) {
		LOG_ERR("Failed to register relay handler: %d", ret);
		return 0;
	}

	ret = modbus_server_register_handler(system_registers_get_handler());
	if (ret < 0) {
		LOG_ERR("Failed to register system handler: %d", ret);
		return 0;
	}

	ret = modbus_client_init();
	if (ret < 0) {
		LOG_ERR("Failed to initialize Modbus client: %d", ret);
		return 0;
	}

	ret = modbus_server_init();
	if (ret < 0) {
		LOG_ERR("Failed to initialize Modbus server: %d", ret);
		return 0;
	}

	LOG_INF("System ready");

	char json[160];

	while (1) {
		int ret = sht20_get_data(33, json, sizeof(json));
		if (ret < 0) {
			LOG_ERR("Failed to get SHT20 data: %d", ret);
		} else {
			LOG_INF("SHT20 Data: %s", json);
		}
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
