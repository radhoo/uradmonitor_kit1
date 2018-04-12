################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../net/dhcp_client.cpp \
../net/dnslkup.cpp \
../net/enc28j60.cpp \
../net/ip_arp_udp_tcp.cpp 

OBJS += \
./net/dhcp_client.o \
./net/dnslkup.o \
./net/enc28j60.o \
./net/ip_arp_udp_tcp.o 

CPP_DEPS += \
./net/dhcp_client.d \
./net/dnslkup.d \
./net/enc28j60.d \
./net/ip_arp_udp_tcp.d 


# Each subdirectory must supply rules for building sources it contributes
net/%.o: ../net/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


