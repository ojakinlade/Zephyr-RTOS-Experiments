#include <errno.h>

#include <zephyr/logging/log.h>

#include "system_registers.h"

LOG_MODULE_REGISTER(system_registers, LOG_LEVEL_INF);

static uint16_t requested_unit_id = 1U;

static int system_read_holding_reg(uint16_t offset, uint16_t *value)
{
	switch (offset) {
	case 0:
		*value = requested_unit_id;
		return 0;
	default:
		return -ENOTSUP;
	}
}

static int system_write_holding_reg(uint16_t offset, uint16_t value)
{
	switch (offset) {
	case 0:
		if ((value < 1U) || (value > 247U)) {
			return -EINVAL;
		}

		requested_unit_id = value;
		LOG_INF("Requested Modbus unit ID %u", value);
		return 0;
	default:
		return -ENOTSUP;
	}
}

static const struct modbus_register_handler system_register_handler = {
	.start_addr = 1U,
	.reg_count = 1U,
	.read_holding_reg = system_read_holding_reg,
	.write_holding_reg = system_write_holding_reg,
};

const struct modbus_register_handler *system_registers_get_handler(void)
{
	return &system_register_handler;
}
