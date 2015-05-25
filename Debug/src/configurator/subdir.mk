################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/configurator/configurator.c 

OBJS += \
./src/configurator/configurator.o 

C_DEPS += \
./src/configurator/configurator.d 


# Each subdirectory must supply rules for building sources it contributes
src/configurator/%.o: ../src/configurator/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


