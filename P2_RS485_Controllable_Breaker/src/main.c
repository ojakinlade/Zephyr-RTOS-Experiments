#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "modbus_client.h"
#include "modbus_server.h"
#include "pd195z_e31.h"
#include "relay_controller.h"
#include "system_registers.h"
#include "sht20.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

#define SHT20_UNIT_ID 33U
#define PD195Z_E31_UNIT_ID 1U
#define TEMPERATURE_THRESHOLD_X10 300U

int main(void)
{
	int ret;
	struct sht20_param sht20_data;
	struct pd195z_e31_param pd195z_data;

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

	// ret = modbus_server_init();
	// if (ret < 0) {
	// 	LOG_ERR("Failed to initialize Modbus server: %d", ret);
	// 	return 0;
	// }

	LOG_INF("System ready");

	char json[1024];

	while (1) {
		// ret = sht20_read_data(SHT20_UNIT_ID, &sht20_data);
		// if (ret < 0) {
		// 	LOG_ERR("Failed to get SHT20 data: %d", ret);
		// } else {
		// 	system_registers_set_threshold_alarm(
		// 		sht20_data.temperature <= TEMPERATURE_THRESHOLD_X10);
		// 	ret = sht20_format_json(&sht20_data, SHT20_UNIT_ID, json, sizeof(json));
		// 	if (ret < 0) {
		// 		LOG_ERR("Failed to format SHT20 data: %d", ret);
		// 	} else {
		// 	LOG_INF("SHT20 Data: %s", json);
		// 	}
		// }

		ret = pd195z_e31_read_data(PD195Z_E31_UNIT_ID, &pd195z_data);
		if (ret < 0) {
			LOG_ERR("Failed to get PD195Z_E31 data: %d", ret);
		} else {
			ret = pd195z_e31_format_json(&pd195z_data,
						     PD195Z_E31_UNIT_ID,
						     json,
						     sizeof(json));
			if (ret < 0) {
				LOG_ERR("Failed to format PD195Z_E31 data: %d", ret);
			} else {
				LOG_INF("PD195Z_E31 Data: %s", json);
			}
		}

		k_sleep(K_SECONDS(1));
		// int ret = modbus_client_read_input_regs(33, 0, data, 3);
		// if (ret < 0) {
		// 	LOG_ERR("Failed to read Modbus input registers: %d", ret);
		// } else {
		// 	LOG_INF("Modbus Data: %u, %.1f, %.1f", data[0], (double)data[1] * 0.1, (double)data[2] * 0.1);
		// }
		// k_sleep(K_SECONDS(1));
	}

	return 0;
}
