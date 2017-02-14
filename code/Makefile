TARGET = Release/uradmonitor-KIT1-EXP.hex
TARGET_ELF = $(patsubst %.hex,%.elf,$(TARGET))
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*.c)
OBJECTS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
HEADERS = $(wildcard *.h)
CC = avr-g++ -g -mmcu=atmega328p -D__AVR_ATmega328P__ -DF_CPU=8000000UL -Os -funsigned-char -fdata-sections -ffunction-sections
LINK = $(CC) -lprintf_flt -lm -Wl,--gc-sections -Wl,-u,vfprintf -Wl,-relax -Xlinker -Map=uradmonitor.map
STRIP = avr-strip --strip-unneeded

$(TARGET): $(TARGET_ELF)
	avr-objcopy -j .text -j .data -O ihex $(TARGET_ELF) $(TARGET)

$(TARGET_ELF): .depend $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET_ELF) $(OBJECTS) $(LIBPATH) $(LIBS)
#	$(STRIP) $(TARGET_ELF)
	avr-size --mcu=atmega328p --format=avr $(TARGET_ELF)

.c.o: $*.h config.h
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -o $@ $<

.cpp.o: $*.h config.h
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -o $@ $<

.hex.elf:
	avr-objcopy -j .text -j .data -O ihex $< $@

-include .depend

clean:
	-rm -f .depend
	-rm -f $(OBJECTS)
	-rm -f *~ core *.core
	-rm -f $(TARGET)
	-rm -f $(TARGET_ELF)

depend:
.depend: Makefile $(SOURCES) $(HEADERS)
	@if [ ! -f .depend ]; then touch .depend; fi
	@makedepend -Y -f .depend  $(SOURCES) 2>/dev/null

test:
	echo $(OBJECTS)
