#ifndef SHT20_H_
#define SHT20_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SHT20_DEV_ADDR 0x0101
#define SHT20_TEMPERATURE_ADDR 0x0001
#define SHT20_HUMIDITY_ADDR 0x0002
#define SHT20_TEMP_CALIBRATION_ADDR 0x0103
#define SHT20_HUMIDITY_CALIBRATION_ADDR 0x0104
#define SHT20_CALIBRATION_VALUE 0U

struct sht20_param {
	float temperature;
	float humidity;
};

struct sht20_calib {
	uint16_t temp_calib;
	uint16_t hum_calib;
};

int sht20_read_data(uint8_t unit_id, struct sht20_param *data, bool *error_flag);
int sht20_get_data(uint8_t unit_id, char *buffer, size_t buffer_size);
bool sht20_validate_data(const struct sht20_param *data);
int sht20_format_json(const struct sht20_param *data,
		      uint8_t unit_id,
		      char *buffer,
		      size_t buffer_size);

#endif /* SHT20_H_ */
