################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sensors/bme280.cpp \
../sensors/bmp180.cpp 

OBJS += \
./sensors/bme280.o \
./sensors/bmp180.o 

CPP_DEPS += \
./sensors/bme280.d \
./sensors/bmp180.d 


# Each subdirectory must supply rules for building sources it contributes
sensors/%.o: ../sensors/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


