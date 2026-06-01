#ifndef MODBUS_SERVER_H_
#define MODBUS_SERVER_H_

#include "modbus_register_handler.h"

int modbus_server_register_handler(const struct modbus_register_handler *handler);
int modbus_server_init(void);

#endif /* MODBUS_SERVER_H_ */
