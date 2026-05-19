APP := .
BOARD ?= rpi_pico2/rp2350a/m33
BUILD_DIR ?= build
ENV_SCRIPT ?= $(HOME)/.zinstaller/env.sh
SHELL := bash

.PHONY: help env boards build pristine clean flash

help:
	@echo "Targets:"
	@echo "  make boards              List available boards"
	@echo "  make build               Build for BOARD=$(BOARD)"
	@echo "  make pristine            Clean configure/build for BOARD=$(BOARD)"
	@echo "  make flash               Flash current build"
	@echo "  make clean               Remove $(BUILD_DIR)"
	@echo ""
	@echo "Examples:"
	@echo "  make build"
	@echo "  make build BOARD=rpi_pico"
	@echo "  make pristine BOARD=rpi_pico2/rp2350a/m33"

env:
	. $(ENV_SCRIPT) && python --version && west --version

boards:
	. $(ENV_SCRIPT) && west boards

build:
	. $(ENV_SCRIPT) && west build -b $(BOARD) -d $(BUILD_DIR) $(APP)

pristine:
	. $(ENV_SCRIPT) && west build -p always -b $(BOARD) -d $(BUILD_DIR) $(APP)

clean:
	rm -rf $(BUILD_DIR)

flash:
	. $(ENV_SCRIPT) && west flash -d $(BUILD_DIR)
