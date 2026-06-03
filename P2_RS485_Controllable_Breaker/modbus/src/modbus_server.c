#include <errno.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/modbus/modbus.h>

#include "modbus_server.h"

LOG_MODULE_REGISTER(modbus_server, LOG_LEVEL_INF);

#define MODBUS_NODE DT_ALIAS(modbus_server)
#define MODBUS_MAX_REGISTER_HANDLERS 8

static const struct modbus_register_handler
	*register_handlers[MODBUS_MAX_REGISTER_HANDLERS];
static size_t register_handler_count;

static const struct modbus_register_handler *find_register_handler(uint16_t addr)
{
	for (size_t i = 0; i < register_handler_count; i++) {
		const struct modbus_register_handler *handler = register_handlers[i];
		uint16_t end_addr = handler->start_addr + handler->reg_count;

		if ((addr >= handler->start_addr) && (addr < end_addr)) {
			return handler;
		}
	}

	return NULL;
}

static int holding_reg_rd(uint16_t addr, uint16_t *reg)
{
	const struct modbus_register_handler *handler = find_register_handler(addr);

	if ((handler == NULL) || (handler->read_holding_reg == NULL)) {
		return -ENOTSUP;
	}

	return handler->read_holding_reg(addr - handler->start_addr, reg);
}

static int holding_reg_wr(uint16_t addr, uint16_t reg)
{
	const struct modbus_register_handler *handler = find_register_handler(addr);

	if ((handler == NULL) || (handler->write_holding_reg == NULL)) {
		return -ENOTSUP;
	}

	return handler->write_holding_reg(addr - handler->start_addr, reg);
}

static struct modbus_user_callbacks mbs_cbs = {
	.holding_reg_rd = holding_reg_rd,
	.holding_reg_wr = holding_reg_wr,
};

static const struct modbus_iface_param server_param = {
	.mode = MODBUS_MODE_RTU,
	.server = {
		.user_cb = &mbs_cbs,
		.unit_id = 1,
	},
	.serial = {
		.baud = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
	},
};

int modbus_server_register_handler(const struct modbus_register_handler *handler)
{
	const struct modbus_register_handler *existing;

	if ((handler == NULL) || (handler->reg_count == 0U)) {
		return -EINVAL;
	}

	if ((handler->read_holding_reg == NULL) &&
	    (handler->write_holding_reg == NULL)) {
		return -EINVAL;
	}

	if (register_handler_count >= MODBUS_MAX_REGISTER_HANDLERS) {
		return -ENOMEM;
	}

	for (size_t i = 0; i < register_handler_count; i++) {
		uint16_t new_end = handler->start_addr + handler->reg_count;
		uint16_t existing_end;

		existing = register_handlers[i];
		existing_end = existing->start_addr + existing->reg_count;

		if ((handler->start_addr < existing_end) &&
		    (new_end > existing->start_addr)) {
			LOG_ERR("Register handler overlap: [%u, %u) with [%u, %u)",
				handler->start_addr, new_end,
				existing->start_addr, existing_end);
			return -EADDRINUSE;
		}
	}

	register_handlers[register_handler_count++] = handler;
	return 0;
}

int modbus_server_init(void)
{
	const char iface_name[] = DEVICE_DT_NAME(MODBUS_NODE);
	int iface;

	iface = modbus_iface_get_by_name(iface_name);
	if (iface < 0) {
		LOG_ERR("Failed to get Modbus iface for %s: %d", iface_name, iface);
		return iface;
	}

	iface = modbus_init_server(iface, server_param);
	if (iface < 0) {
		LOG_ERR("Failed to initialize Modbus server: %d", iface);
		return iface;
	}

	LOG_INF("Modbus server started on %s, unit ID %u",
		iface_name, server_param.server.unit_id);

	return 0;
}
