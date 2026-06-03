#include <errno.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/modbus/modbus.h>

#include "modbus_client.h"

LOG_MODULE_REGISTER(modbus_client, LOG_LEVEL_INF);

#define MODBUS_NODE DT_ALIAS(modbus_client)

static const struct modbus_iface_param client_param = {
	.mode = MODBUS_MODE_RTU,
	.serial = {
		.baud = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
	},
};