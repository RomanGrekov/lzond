################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/encoder/encoder.c 

OBJS += \
./src/encoder/encoder.o 

C_DEPS += \
./src/encoder/encoder.d 


# Each subdirectory must supply rules for building sources it contributes
src/encoder/%.o: ../src/encoder/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


