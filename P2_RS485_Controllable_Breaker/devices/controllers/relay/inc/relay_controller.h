#ifndef RELAY_CONTROLLER_H_
#define RELAY_CONTROLLER_H_

#include <stdbool.h>

#include "modbus_register_handler.h"

int relay_controller_init(void);
int relay_controller_set(bool on);
bool relay_controller_get(void);
const struct modbus_register_handler *relay_controller_get_register_handler(void);

#endif /* RELAY_CONTROLLER_H_ */
