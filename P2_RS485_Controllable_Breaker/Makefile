APP := .
# BOARD ?= rpi_pico2/rp2350a/m33
BOARD ?= rpi_pico/rp2040
BUILD_DIR ?= build
ENV_SCRIPT ?= $(HOME)/.zinstaller/env.sh
SHELL := bash
BOARD_CONF_NAME := $(subst /,_,$(BOARD))
BOARD_CONF ?= boards/$(BOARD_CONF_NAME).conf
EXTRA_CONF_ARG := $(if $(wildcard $(BOARD_CONF)),-DEXTRA_CONF_FILE=$(BOARD_CONF),)
CMAKE_EXTRA_ARGS ?=

.PHONY: help env boards build pristine clean flash

help:
	@echo "Targets:"
	@echo "  make boards              List available boards"
	@echo "  make build               Build for BOARD=$(BOARD)"
	@echo "  make pristine            Clean configure/build for BOARD=$(BOARD)"
	@echo "  make flash               Flash current build"
	@echo "  make clean               Remove $(BUILD_DIR)"
	@echo "  BOARD_CONF=<file>        Append a board-specific Kconfig fragment"
	@echo "  CMAKE_EXTRA_ARGS=<args>  Pass extra CMake args after west build --"
	@echo ""
	@echo "Examples:"
	@echo "  make build"
	@echo "  make build BOARD=rpi_pico"
	@echo "  make pristine BOARD=rpi_pico2/rp2350a/m33"
	@echo "  make build BOARD_CONF=boards/rpi_pico.conf"

env:
	. $(ENV_SCRIPT) && python --version && west --version

boards:
	. $(ENV_SCRIPT) && west boards

build:
	. $(ENV_SCRIPT) && west build -b $(BOARD) -d $(BUILD_DIR) $(APP) -- $(EXTRA_CONF_ARG) $(CMAKE_EXTRA_ARGS)

pristine:
	. $(ENV_SCRIPT) && west build -p always -b $(BOARD) -d $(BUILD_DIR) $(APP) -- $(EXTRA_CONF_ARG) $(CMAKE_EXTRA_ARGS)

clean:
	rm -rf $(BUILD_DIR)

flash:
	. $(ENV_SCRIPT) && west flash -d $(BUILD_DIR)
