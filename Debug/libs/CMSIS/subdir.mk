################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/CMSIS/core_cm3.c \
../libs/CMSIS/misc.c \
../libs/CMSIS/stm32f10x_adc.c \
../libs/CMSIS/stm32f10x_gpio.c \
../libs/CMSIS/stm32f10x_rcc.c \
../libs/CMSIS/stm32f10x_usart.c \
../libs/CMSIS/system_stm32f10x.c 

OBJS += \
./libs/CMSIS/core_cm3.o \
./libs/CMSIS/misc.o \
./libs/CMSIS/stm32f10x_adc.o \
./libs/CMSIS/stm32f10x_gpio.o \
./libs/CMSIS/stm32f10x_rcc.o \
./libs/CMSIS/stm32f10x_usart.o \
./libs/CMSIS/system_stm32f10x.o 

C_DEPS += \
./libs/CMSIS/core_cm3.d \
./libs/CMSIS/misc.d \
./libs/CMSIS/stm32f10x_adc.d \
./libs/CMSIS/stm32f10x_gpio.d \
./libs/CMSIS/stm32f10x_rcc.d \
./libs/CMSIS/stm32f10x_usart.d \
./libs/CMSIS/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
libs/CMSIS/%.o: ../libs/CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


