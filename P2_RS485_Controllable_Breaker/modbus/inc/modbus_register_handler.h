#ifndef MODBUS_REGISTER_HANDLER_H_
#define MODBUS_REGISTER_HANDLER_H_

#include <stdint.h>

struct modbus_register_handler {
	uint16_t start_addr;
	uint16_t reg_count;
	int (*read_holding_reg)(uint16_t offset, uint16_t *value);
	int (*write_holding_reg)(uint16_t offset, uint16_t value);
};

#endif /* MODBUS_REGISTER_HANDLER_H_ */
