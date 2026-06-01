#ifndef SYSTEM_REGISTERS_H_
#define SYSTEM_REGISTERS_H_

#include "modbus_register_handler.h"

const struct modbus_register_handler *system_registers_get_handler(void);

#endif /* SYSTEM_REGISTERS_H_ */
