################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../geiger/detectors.cpp \
../geiger/inverter.cpp 

OBJS += \
./geiger/detectors.o \
./geiger/inverter.o 

CPP_DEPS += \
./geiger/detectors.d \
./geiger/inverter.d 


# Each subdirectory must supply rules for building sources it contributes
geiger/%.o: ../geiger/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


