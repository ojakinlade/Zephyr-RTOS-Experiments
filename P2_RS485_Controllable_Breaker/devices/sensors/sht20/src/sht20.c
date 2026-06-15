#include <errno.h>
#include <math.h>
#include <stdio.h>

#include <zephyr/kernel.h>

#include "modbus_client.h"
#include "sht20.h"

#define SHT20_DELAY_PERIOD_MS 100

static bool is_calibration_valid(uint16_t calibration)
{
	return calibration == 0U;
}

static int sht20_read_calibration(uint8_t unit_id, struct sht20_calib *data)
{
	int ret;

	ret = modbus_client_read_holding_regs(unit_id,
					      SHT20_TEMP_CALIBRATION_ADDR,
					      &data->temp_calib,
					      1);
	if (ret < 0) {
		return ret;
	}

	return modbus_client_read_holding_regs(unit_id,
					       SHT20_HUMIDITY_CALIBRATION_ADDR,
					       &data->hum_calib,
					       1);
}

static int sht20_reset_calibration(uint8_t unit_id)
{
	int ret;

	ret = modbus_client_write_holding_reg(unit_id,
					      SHT20_TEMP_CALIBRATION_ADDR,
					      SHT20_CALIBRATION_VALUE);
	if (ret < 0) {
		return ret;
	}

	k_msleep(SHT20_DELAY_PERIOD_MS);

	return modbus_client_write_holding_reg(unit_id,
					       SHT20_HUMIDITY_CALIBRATION_ADDR,
					       SHT20_CALIBRATION_VALUE);
}

int sht20_read_data(uint8_t unit_id, struct sht20_param *data, bool *error_flag)
{
	struct sht20_calib calib = {0};
	uint16_t raw_temperature;
	uint16_t raw_humidity;
	int ret;

	if (data == NULL) {
		return -EINVAL;
	}

	if (error_flag != NULL) {
		*error_flag = false;
	}

	// ret = sht20_read_calibration(unit_id, &calib);
	// if (ret < 0) {
	// 	return ret;
	// }

	// if (!is_calibration_valid(calib.temp_calib) ||
	//     !is_calibration_valid(calib.hum_calib)) {
	// 	if (error_flag != NULL) {
	// 		*error_flag = true;
	// 	}

	// 	ret = sht20_reset_calibration(unit_id);
	// 	if (ret < 0) {
	// 		return ret;
	// 	}

	// 	k_msleep(SHT20_DELAY_PERIOD_MS);
	// }

	ret = modbus_client_read_input_regs(unit_id,
					    SHT20_TEMPERATURE_ADDR,
					    &raw_temperature,
					    1);
	if (ret < 0) {
		return ret;
	}

	// k_msleep(SHT20_DELAY_PERIOD_MS);

	// ret = modbus_client_read_input_regs(unit_id,
	// 				    SHT20_HUMIDITY_ADDR,
	// 				    &raw_humidity,
	// 				    1);
	// if (ret < 0) {
	// 	return ret;
	// }

	// data->temperature = raw_temperature * 0.1f;
	// data->humidity = raw_humidity * 0.1f;

	// return 0;
}

int sht20_get_data(uint8_t unit_id, char *buffer, size_t buffer_size)
{
	struct sht20_param data;
	bool error_flag;
	int ret;

	ret = sht20_read_data(unit_id, &data, &error_flag);
	if (ret < 0) {
		return ret;
	}

	if (!sht20_validate_data(&data)) {
		return -ERANGE;
	}

  // printf("SHT20 Data - Temperature: %.2f C, Humidity: %.2f %%",
	// 	(double)data.temperature, (double)data.humidity);
	// sprintf(buffer, "{\"temperature\":%.2f,\"humidity\":%.2f,\"error\":%s}",
	// 	(double)data.temperature, (double)data.humidity,
	// 	error_flag ? "true" : "false");
	return sht20_format_json(&data, unit_id, buffer, buffer_size);
}

bool sht20_validate_data(const struct sht20_param *data)
{
	if (data == NULL) {
		return false;
	}

	if (isnan(data->temperature) || isnan(data->humidity)) {
		return false;
	}

	if ((data->temperature < 0.0f) || (data->temperature > 60.0f)) {
		return false;
	}

	if ((data->humidity < 0.0f) || (data->humidity > 100.0f)) {
		return false;
	}

	return true;
}

int sht20_format_json(const struct sht20_param *data,
		      uint8_t unit_id,
		      char *buffer,
		      size_t buffer_size)
{
	if ((data == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
		return -EINVAL;
	}

	return snprintf(buffer, buffer_size,
			"{\"meter_type\":\"ambient\",\"model\":\"SHT20\","
			"\"ID\":\"meter%u\",\"channels\":[{\"ID\":\"ch1\","
			"\"Temperature\":\"%.2f\",\"Humidity\":\"%.2f\"}]}",
			unit_id, (double)data->temperature, (double)data->humidity);
}
