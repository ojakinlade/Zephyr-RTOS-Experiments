#include <errno.h>
#include <stdio.h>

#include "level_sensor.h"
#include "modbus_client.h"

int level_sensor_read_data(uint8_t unit_id, struct level_sensor_param *data)
{
	uint16_t raw_level;
	int ret;

	if (data == NULL) {
		return -EINVAL;
	}

	ret = modbus_client_read_input_regs(unit_id,
					    LEVEL_SENSOR_LEVEL_ADDR,
					    &raw_level,
					    1);
	if (ret < 0) {
		return ret;
	}

	data->level = raw_level;
	return 0;
}

int level_sensor_get_level(uint8_t unit_id, uint16_t *level)
{
	struct level_sensor_param data;
	int ret;

	if (level == NULL) {
		return -EINVAL;
	}

	ret = level_sensor_read_data(unit_id, &data);
	if (ret < 0) {
		return ret;
	}

	*level = data.level;
	return 0;
}

int level_sensor_get_data(uint8_t unit_id, char *buffer, size_t buffer_size)
{
	struct level_sensor_param data;
	int ret;

	ret = level_sensor_read_data(unit_id, &data);
	if (ret < 0) {
		return ret;
	}

	if (!level_sensor_validate_data(&data)) {
		return -ERANGE;
	}

	return level_sensor_format_json(&data, unit_id, buffer, buffer_size);
}

bool level_sensor_validate_data(const struct level_sensor_param *data)
{
	if (data == NULL) {
		return false;
	}

	return true;
}

int level_sensor_format_json(const struct level_sensor_param *data,
			     uint8_t unit_id,
			     char *buffer,
			     size_t buffer_size)
{
	if ((data == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
		return -EINVAL;
	}

	return snprintf(buffer, buffer_size,
			"{\"meter_type\":\"level\",\"model\":\"level_sensor\","
			"\"ID\":\"meter%u\",\"channels\":[{\"ID\":\"ch1\","
			"\"level\":\"%u\"}]}",
			unit_id, data->level);
}
