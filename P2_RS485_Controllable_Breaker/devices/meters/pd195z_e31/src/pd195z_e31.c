#include <errno.h>
#include <math.h>
#include <stdio.h>

#include <zephyr/kernel.h>

#include "modbus_client.h"
#include "pd195z_e31.h"

#define PD195Z_E31_DELAY_PERIOD_MS 100
#define PD195Z_E31_FLOAT_REG_COUNT 2U

static int pd195z_e31_read_float(uint8_t unit_id, uint16_t reg_addr, float *value)
{
	union {
		uint16_t regs[PD195Z_E31_FLOAT_REG_COUNT];
		float value;
	} raw_data;
	int ret;

	if (value == NULL) {
		return -EINVAL;
	}

	ret = modbus_client_read_holding_regs(unit_id,
					    reg_addr,
					    raw_data.regs,
					    PD195Z_E31_FLOAT_REG_COUNT);
	if (ret < 0) {
		return ret;
	}

	*value = raw_data.value;
	return 0;
}

int pd195z_e31_read_data(uint8_t unit_id, struct pd195z_e31_param *data)
{
	int ret;

	if (data == NULL) {
		return -EINVAL;
	}

	ret = pd195z_e31_read_float(unit_id,
				    PD195Z_E31_DC_VOLTAGE,
				    &data->dc_voltage);
	if (ret < 0) {
		return ret;
	}

	k_msleep(PD195Z_E31_DELAY_PERIOD_MS);

	ret = pd195z_e31_read_float(unit_id,
				    PD195Z_E31_DC_CURRENT,
				    &data->dc_current);
	if (ret < 0) {
		return ret;
	}

	k_msleep(PD195Z_E31_DELAY_PERIOD_MS);

	ret = pd195z_e31_read_float(unit_id,
				    PD195Z_E31_DC_POWER,
				    &data->dc_power);
	if (ret < 0) {
		return ret;
	}

	k_msleep(PD195Z_E31_DELAY_PERIOD_MS);

	ret = pd195z_e31_read_float(unit_id,
				    PD195Z_E31_DC_ENERGY_EXPORT,
				    &data->dc_energy_out);
	if (ret < 0) {
		return ret;
	}

	k_msleep(PD195Z_E31_DELAY_PERIOD_MS);

	ret = pd195z_e31_read_float(unit_id,
				    PD195Z_E31_DC_ENERGY_IMPORT,
				    &data->dc_energy_in);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int pd195z_e31_get_data(uint8_t unit_id, char *buffer, size_t buffer_size)
{
	struct pd195z_e31_param data;
	int ret;

	ret = pd195z_e31_read_data(unit_id, &data);
	if (ret < 0) {
		return ret;
	}

	ret = pd195z_e31_format_json(&data, unit_id, buffer, buffer_size);
	if (ret < 0) {
		return ret;
	}

	return (int)pd195z_e31_validate_data(&data);
}

bool pd195z_e31_validate_data(const struct pd195z_e31_param *data)
{
	if (data == NULL) {
		return false;
	}

	if (!isfinite(data->dc_voltage) ||
	    !isfinite(data->dc_current) ||
	    !isfinite(data->dc_power) ||
	    !isfinite(data->dc_energy_out) ||
	    !isfinite(data->dc_energy_in)) {
		return false;
	}

	if ((data->dc_voltage < 0.0f) || (data->dc_voltage > 1000.0f)) {
		return false;
	}

	if ((data->dc_energy_out < 0.0f) || (data->dc_energy_in < 0.0f)) {
		return false;
	}

	return true;
}

int pd195z_e31_format_json(const struct pd195z_e31_param *data,
			   uint8_t unit_id,
			   char *buffer,
			   size_t buffer_size)
{
	if ((data == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
		return -EINVAL;
	}

	return snprintf(buffer, buffer_size,
			"{\"meter_type\":\"DC\",\"model\":\"PD195Z_E31\","
			"\"ID\":\"meter%u\",\"phA_V\":\"%.2f\","
			"\"channels\":[{\"ID\":\"ch1\",\"I\":\"%.3f\","
			"\"P\":\"%.3f\",\"KWh_In\":\"%.3f\","
			"\"KWh_Out\":\"%.3f\"}]}",
			unit_id,
			(double)data->dc_voltage,
			(double)data->dc_current,
			(double)data->dc_power,
			(double)data->dc_energy_in,
			(double)data->dc_energy_out);
}
