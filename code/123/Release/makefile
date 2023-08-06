################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include time/subdir.mk
-include sensors/subdir.mk
-include net/subdir.mk
-include misc/subdir.mk
-include lcd/subdir.mk
-include i2c/subdir.mk
-include gpio/subdir.mk
-include geiger/subdir.mk
-include app/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
uradmonitor-KIT1-EXP.lss \

FLASH_IMAGE += \
uradmonitor-KIT1-EXP.hex \

EEPROM_IMAGE += \
uradmonitor-KIT1-EXP.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: uradmonitor-KIT1-EXP.elf secondary-outputs

# Tool invocations
uradmonitor-KIT1-EXP.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,uradmonitor-KIT1-EXP.map,--cref -mrelax -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -lm -Llibprintf_flt.a -Llibm.a -mmcu=atmega328p -o "uradmonitor-KIT1-EXP.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

uradmonitor-KIT1-EXP.lss: uradmonitor-KIT1-EXP.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S uradmonitor-KIT1-EXP.elf  >"uradmonitor-KIT1-EXP.lss"
	@echo 'Finished building: $@'
	@echo ' '

uradmonitor-KIT1-EXP.hex: uradmonitor-KIT1-EXP.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex uradmonitor-KIT1-EXP.elf  "uradmonitor-KIT1-EXP.hex"
	@echo 'Finished building: $@'
	@echo ' '

uradmonitor-KIT1-EXP.eep: uradmonitor-KIT1-EXP.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex uradmonitor-KIT1-EXP.elf  "uradmonitor-KIT1-EXP.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: uradmonitor-KIT1-EXP.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p uradmonitor-KIT1-EXP.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) uradmonitor-KIT1-EXP.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
