################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hd44780/hd44780.c 

OBJS += \
./src/hd44780/hd44780.o 

C_DEPS += \
./src/hd44780/hd44780.d 


# Each subdirectory must supply rules for building sources it contributes
src/hd44780/%.o: ../src/hd44780/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


