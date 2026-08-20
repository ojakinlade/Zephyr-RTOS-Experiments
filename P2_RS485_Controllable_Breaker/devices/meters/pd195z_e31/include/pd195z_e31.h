#ifndef PD195Z_E31_H_
#define PD195Z_E31_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PD195Z_E31_DC_VOLTAGE        0x06
#define PD195Z_E31_DC_CURRENT        0x08
#define PD195Z_E31_DC_POWER          0x0A
#define PD195Z_E31_DC_ENERGY_EXPORT  0x0C
#define PD195Z_E31_DC_ENERGY_IMPORT  0x16

struct pd195z_e31_param {
	float dc_voltage;
	float dc_current;
	float dc_power;
	float dc_energy_out;
	float dc_energy_in;
};

int pd195z_e31_read_data(uint8_t unit_id, struct pd195z_e31_param *data);
int pd195z_e31_get_data(uint8_t unit_id, char *buffer, size_t buffer_size);
bool pd195z_e31_validate_data(const struct pd195z_e31_param *data);
int pd195z_e31_format_json(const struct pd195z_e31_param *data,
			   uint8_t unit_id,
			   char *buffer,
			   size_t buffer_size);

#endif /* PD195Z_E31_H_ */
