################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/7_seg.c \
../src/button.c \
../src/ds18b20.c \
../src/gpio.c \
../src/led.c \
../src/main.c \
../src/onewire.c 

OBJS += \
./src/7_seg.o \
./src/button.o \
./src/ds18b20.o \
./src/gpio.o \
./src/led.o \
./src/main.o \
./src/onewire.o 

C_DEPS += \
./src/7_seg.d \
./src/button.d \
./src/ds18b20.d \
./src/gpio.d \
./src/led.d \
./src/main.d \
./src/onewire.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L476RGTx -DNUCLEO_L476RG -DDEBUG -I"D:/lab8_3/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


