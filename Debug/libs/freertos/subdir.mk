################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/freertos/croutine.c \
../libs/freertos/event_groups.c \
../libs/freertos/heap_2.c \
../libs/freertos/list.c \
../libs/freertos/port.c \
../libs/freertos/queue.c \
../libs/freertos/tasks.c \
../libs/freertos/timers.c 

OBJS += \
./libs/freertos/croutine.o \
./libs/freertos/event_groups.o \
./libs/freertos/heap_2.o \
./libs/freertos/list.o \
./libs/freertos/port.o \
./libs/freertos/queue.o \
./libs/freertos/tasks.o \
./libs/freertos/timers.o 

C_DEPS += \
./libs/freertos/croutine.d \
./libs/freertos/event_groups.d \
./libs/freertos/heap_2.d \
./libs/freertos/list.d \
./libs/freertos/port.d \
./libs/freertos/queue.d \
./libs/freertos/tasks.d \
./libs/freertos/timers.d 


# Each subdirectory must supply rules for building sources it contributes
libs/freertos/%.o: ../libs/freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../libs/CMSIS/inc" -I"../libs/freertos/inc" -I"../src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


