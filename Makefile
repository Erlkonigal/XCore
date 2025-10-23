PWD:=$(shell pwd)
MILL=$(PWD)/mill
MILL_PROJECT=chisel
BUILD_DIR=build

EMU_CPP_DIR=$(PWD)/emu
EMU_RTL_DIR=$(BUILD_DIR)/rtl

EMU_CPP_SRCS=$(shell find $(EMU_CPP_DIR) -name '*.cpp')
EMU_RTL_SRCS=$(shell find $(EMU_RTL_DIR) -name '*.sv' -or -name '*.v')

emu-rtl: $(MILL)
	@mkdir -p $(EMU_RTL_DIR)
	$(MILL) -i $(MILL_PROJECT).runMain xcore.XCoreSimTop --target-dir $(EMU_RTL_DIR)

check-format:
	$(MILL) $(MILL_PROJECT).checkFormat

reformat:
	$(MILL) $(MILL_PROJECT).reformat

clean:
	rm -rf $(BUILD_DIR)

test:
	@echo $(EMU_SRCS)

.PHONY: emu emu-rtl clean test