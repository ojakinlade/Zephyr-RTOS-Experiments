# P2 RS485 Controllable Breaker

## Overview

This project is a Zephyr-based Modbus RTU server for the Raspberry Pi Pico.
It exposes a relay over RS485 so a Modbus client can control the breaker state.

Current functionality:

- USB CDC ACM console for logs
- Modbus RTU server on `uart1`
- Relay control through holding register `0`
- Placeholder holding register `1` for a requested Modbus unit ID

## Project Layout

The application is split into small modules:

- `src/main.c`: system startup
- `src/relay.c`: relay GPIO initialization and control
- `src/modbus_server.c`: Modbus RTU server and register map
- `include/relay.h`: relay module API
- `include/modbus_server.h`: Modbus server API

## Hardware Mapping

The board overlay currently uses:

- `GP25`: relay output, active low
- `GP8`: `uart1` TX for RS485
- `GP9`: `uart1` RX for RS485
- `GP7`: RS485 driver enable (DE), active high
- USB CDC ACM: console and shell

## Modbus Map

### Unit ID

The active Modbus unit ID is currently fixed to `1` in the firmware.

### Holding Registers

- `HR0`: relay control
  - write `0`: relay off
  - write `1`: relay on
  - read: returns current relay state
- `HR1`: requested Modbus unit ID
  - valid range: `1` to `247`
  - currently stored in RAM only
  - does not yet reconfigure the active Modbus address

## Building

This project includes a simple `Makefile` wrapper around `west`.

Build with:

```console
make build
```

Clean rebuild with:

```console
make pristine
```

The default board is:

```text
rpi_pico/rp2040
```

## Flashing

Flash the current build with:

```console
make flash
```

## USB Console

Logs are routed to USB CDC ACM, not `uart0`.

This keeps both hardware UARTs available for Modbus-related work:

- `uart1`: current Modbus RTU server
- `uart0`: reserved for possible future Modbus client use

When you connect the Pico over USB, open the CDC serial device on your host to
view logs.

## RS485 Testing

Use a Modbus RTU client with:

- baud rate: `9600`
- data bits: `8`
- parity: `none`
- stop bits: `1`
- unit ID: `1`

Example test flow:

1. Write holding register `0` with value `1` to turn the relay on.
2. Write holding register `0` with value `0` to turn the relay off.
3. Read holding register `0` to confirm the state.

## Next Steps

Planned improvements include:

- persistent Modbus unit ID storage
- adding a Modbus client on `uart0`
- unit tests with `ztest`
