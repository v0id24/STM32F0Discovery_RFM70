ARM_GCC_PATH=e:/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/bin/
ARM_GCC=arm-none-eabi-gcc
ARM_OBJ=arm-none-eabi-objcopy
OPENOCD_PATH=e:/STMicroelectronics/openocd-0.7.0/
OPENOCD_COMMAND=bin/openocd-0.7.0
LINKER_FILE=stm32_flash.ld
FILES=$(shell find src -name "*.c")
GCC_FLAGS=-DUSE_STDPERIPH_DRIVER -DUSE_FULL_ASSERT -std=c99 -I"inc" -Wall -mcpu=cortex-m0 -march=armv6-m -mthumb 
BUILD_PATH=DEBUG

all: 
	make dependency 
	make $(BUILD_PATH)/main.bin

dependency: $(FILES:src/%.c=$(BUILD_PATH)/%.dep)

$(BUILD_PATH)/%.dep: src/%.c
	@echo "Create $(<:src/%.c=%.dep)"
	@$(ARM_GCC_PATH)$(ARM_GCC) -MM -MT $(<:src/%.c=$(BUILD_PATH)/%.o) $< -o $@ $(GCC_FLAGS)
	@echo "	@echo \"Compile $(<:src/%=%)\"" >> $@
	@echo "	@$(ARM_GCC_PATH)$(ARM_GCC) $(GCC_FLAGS) -c -O0 -g -o $(<:src/%.c=$(BUILD_PATH)/%.o) $<" >> $@

-include $(FILES:src/%.c=$(BUILD_PATH)/%.dep)

$(BUILD_PATH)/startup_stm32f0xx.o: src/startup_stm32f0xx.s
	$(ARM_GCC_PATH)$(ARM_GCC) $(GCC_FLAGS) -c -O0 -g -o $@  $< 

$(BUILD_PATH)/main.bin: $(BUILD_PATH)/main.elf
	@echo "ObjCopy"
	@$(ARM_GCC_PATH)$(ARM_OBJ) -Obinary $< $@

$(BUILD_PATH)/main.elf: $(FILES:src/%.c=$(BUILD_PATH)/%.o) $(BUILD_PATH)/startup_stm32f0xx.o
	@echo "Linking"
	@$(ARM_GCC_PATH)$(ARM_GCC) $^ -o $@ $(GCC_FLAGS) -T$(LINKER_FILE) 

clear:
	rm -f $(BUILD_PATH)/*
	
new: clear all

flash: 
	$(OPENOCD_PATH)$(OPENOCD_COMMAND) -f $(OPENOCD_PATH)/scripts/board/stm32f0discovery.cfg -c "program $(BUILD_PATH)/main.bin 0x08000000"

