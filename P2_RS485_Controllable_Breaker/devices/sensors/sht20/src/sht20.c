#include <errno.h>
#include <math.h>
#include <stdio.h>

#include <zephyr/kernel.h>

#include "modbus_client.h"
#include "sht20.h"

#define SHT20_DELAY_PERIOD_MS 100

int sht20_read_data(uint8_t unit_id, struct sht20_param *data)
{
	uint16_t raw_temperature;
	uint16_t raw_humidity;
	int ret;

	if (data == NULL) {
		return -EINVAL;
	}

	ret = modbus_client_read_input_regs(unit_id,
					    SHT20_TEMPERATURE_ADDR,
					    &raw_temperature,
					    1);
	if (ret < 0) {
		return ret;
	}

	k_msleep(SHT20_DELAY_PERIOD_MS);

	ret = modbus_client_read_input_regs(unit_id,
					    SHT20_HUMIDITY_ADDR,
					    &raw_humidity,
					    1);
	if (ret < 0) {
		return ret;
	}

	data->temperature = raw_temperature;
	data->humidity = raw_humidity;

	return 0;
}

int sht20_get_data(uint8_t unit_id, char *buffer, size_t buffer_size)
{
	struct sht20_param data;
	int ret;

	ret = sht20_read_data(unit_id, &data);
	if (ret < 0) {
		return ret;
	}

	sht20_format_json(&data, unit_id, buffer, buffer_size);
	return (int)sht20_validate_data(&data);
}

bool sht20_validate_data(const struct sht20_param *data)
{
	if (data == NULL) {
		return false;
	}

	if ((data->temperature < 0) || (data->temperature > 600)) {
		return false;
	}

	if ((data->humidity < 0) || (data->humidity > 1000)) {
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

	uint16_t temp_x10 = data->temperature;
	uint16_t hum_x10 = data->humidity;	
	return snprintf(buffer, buffer_size,
			"{\"meter_type\":\"ambient\",\"model\":\"SHT20\","
			"\"ID\":\"meter%u\",\"channels\":[{\"ID\":\"ch1\","
			"\"Temperature\":\"%u.%u\",\"Humidity\":\"%u.%u\"}]}",
			unit_id, temp_x10 / 10, temp_x10 % 10, hum_x10 / 10, hum_x10 % 10);
}