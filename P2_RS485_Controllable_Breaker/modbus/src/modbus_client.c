#include <errno.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/modbus/modbus.h>

#include "modbus_client.h"

LOG_MODULE_REGISTER(modbus_client, LOG_LEVEL_INF);

#define MODBUS_NODE DT_ALIAS(modbus_client)

static int client_iface = -1;

static const struct modbus_iface_param client_param = {
	.mode = MODBUS_MODE_RTU,
	.rx_timeout = 50000,
	.serial = {
		.baud = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
	},
};

int modbus_client_init(void)
{
	const char iface_name[] = DEVICE_DT_NAME(MODBUS_NODE);

	client_iface = modbus_iface_get_by_name(iface_name);
	if (client_iface < 0) {
		LOG_ERR("Failed to get Modbus client iface for %s: %d",
			iface_name, client_iface);
		return client_iface;
	}

	client_iface = modbus_init_client(client_iface, client_param);
	if (client_iface < 0) {
		LOG_ERR("Failed to initialize Modbus client: %d", client_iface);
		return client_iface;
	}

	LOG_INF("Modbus client started on %s", iface_name);

	return 0;
}

int modbus_client_read_holding_regs(uint8_t unit_id,
				    uint16_t start_addr,
				    uint16_t *reg_buf,
				    uint16_t num_regs)
{
	if (client_iface < 0) {
		return -ENODEV;
	}

	return modbus_read_holding_regs(client_iface, unit_id, start_addr,
					reg_buf, num_regs);
}

int modbus_client_read_input_regs(uint8_t unit_id,
				  uint16_t start_addr,
				  uint16_t *reg_buf,
				  uint16_t num_regs)
{
	if (client_iface < 0) {
		return -ENODEV;
	}

	return modbus_read_input_regs(client_iface, unit_id, start_addr,
				      reg_buf, num_regs);
}

int modbus_client_write_holding_reg(uint8_t unit_id,
				    uint16_t reg_addr,
				    uint16_t reg_val)
{
	if (client_iface < 0) {
		return -ENODEV;
	}

	return modbus_write_holding_reg(client_iface, unit_id, reg_addr, reg_val);
}

int modbus_client_write_holding_regs(uint8_t unit_id,
				     uint16_t start_addr,
				     uint16_t *reg_buf,
				     uint16_t num_regs)
{
	if (client_iface < 0) {
		return -ENODEV;
	}

	return modbus_write_holding_regs(client_iface, unit_id, start_addr,
					 reg_buf, num_regs);
}
