#include <errno.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/modbus/modbus.h>

#include "modbus_server.h"
#include "relay.h"

LOG_MODULE_REGISTER(modbus_server, LOG_LEVEL_INF);

#define MODBUS_NODE DT_ALIAS(modbus)
#define HOLDING_REG_COUNT 2

static uint16_t holding_reg[HOLDING_REG_COUNT] = {
	[0] = 0,
	[1] = 1,
};

static int holding_reg_rd(uint16_t addr, uint16_t *reg)
{
	if (addr >= HOLDING_REG_COUNT) {
		return -ENOTSUP;
	}

	if (addr == 0U) {
		holding_reg[0] = relay_get() ? 1U : 0U;
	}

	*reg = holding_reg[addr];
	return 0;
}

static int holding_reg_wr(uint16_t addr, uint16_t reg)
{
	int ret;

	switch (addr) {
	case 0:
		if (reg > 1U) {
			return -EINVAL;
		}

		ret = relay_set(reg == 1U);
		if (ret < 0) {
			return ret;
		}

		holding_reg[0] = reg;
		LOG_INF("Relay set via holding register 0: %u", reg);
		return 0;
	case 1:
		if ((reg < 1U) || (reg > 247U)) {
			return -EINVAL;
		}

		holding_reg[1] = reg;
		LOG_INF("Requested Modbus unit ID %u", reg);
		return 0;
	default:
		return -ENOTSUP;
	}
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
