#ifndef LEVEL_SENSOR_H_
#define LEVEL_SENSOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LEVEL_SENSOR_LEVEL_ADDR 0x0001

struct level_sensor_param {
	float level;
};

int level_sensor_read_data(uint8_t unit_id, struct level_sensor_param *data);
int level_sensor_get_level(uint8_t unit_id, float *level);
int level_sensor_get_data(uint8_t unit_id, char *buffer, size_t buffer_size);
bool level_sensor_validate_data(const struct level_sensor_param *data);
int level_sensor_format_json(const struct level_sensor_param *data,
			     uint8_t unit_id,
			     char *buffer,
			     size_t buffer_size);

#endif /* LEVEL_SENSOR_H_ */
