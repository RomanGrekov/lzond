################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/usart/subdir.mk
-include src/string_lib/subdir.mk
-include src/led/subdir.mk
-include src/hd44780/subdir.mk
-include src/flash/subdir.mk
-include src/encoder/subdir.mk
-include src/dac/subdir.mk
-include src/configurator/subdir.mk
-include src/clock/subdir.mk
-include src/buttons/subdir.mk
-include src/adc/subdir.mk
-include src/subdir.mk
-include libs/freertos/subdir.mk
-include libs/CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
lzond.hex \

SECONDARY_SIZE += \
lzond.siz \


# All Target
all: lzond.elf secondary-outputs

# Tool invocations
lzond.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T /home/romang/Projects/eclipse/lzond/startup/stm32_flash.ld -Xlinker --gc-sections -Wl,-Map,"lzond.map" -o "lzond.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lzond.hex: lzond.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "lzond.elf"  "lzond.hex"
	@echo 'Finished building: $@'
	@echo ' '

lzond.siz: lzond.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "lzond.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(S_UPPER_DEPS) lzond.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
