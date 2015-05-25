################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/clock/clock.c 

OBJS += \
./src/clock/clock.o 

C_DEPS += \
./src/clock/clock.d 


# Each subdirectory must supply rules for building sources it contributes
src/clock/%.o: ../src/clock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


