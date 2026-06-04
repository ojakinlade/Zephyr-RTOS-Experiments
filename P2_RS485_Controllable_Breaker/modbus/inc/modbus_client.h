#ifndef MODBUS_CLIENT_H_
#define MODBUS_CLIENT_H_

#include <stdint.h>

int modbus_client_init(void);
int modbus_client_read_holding_regs(uint8_t unit_id,
				    uint16_t start_addr,
				    uint16_t *reg_buf,
				    uint16_t num_regs);
int modbus_client_read_input_regs(uint8_t unit_id,
				  uint16_t start_addr,
				  uint16_t *reg_buf,
				  uint16_t num_regs);
int modbus_client_write_holding_reg(uint8_t unit_id,
				    uint16_t reg_addr,
				    uint16_t reg_val);
int modbus_client_write_holding_regs(uint8_t unit_id,
				     uint16_t start_addr,
				     uint16_t *reg_buf,
				     uint16_t num_regs);

#endif /* MODBUS_CLIENT_H_ */
