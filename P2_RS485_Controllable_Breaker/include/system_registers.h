#ifndef SYSTEM_REGISTERS_H_
#define SYSTEM_REGISTERS_H_

#include <stdbool.h>

#include "modbus_register_handler.h"

const struct modbus_register_handler *system_registers_get_handler(void);
void system_registers_set_threshold_alarm(bool alarm_active);

#endif /* SYSTEM_REGISTERS_H_ */
