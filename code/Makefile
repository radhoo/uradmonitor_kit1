TARGET = Release/uradmonitor-KIT1-EXP.hex
TARGET_ELF = $(patsubst %.hex,%.elf,$(TARGET))
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
HEADERS = $(wildcard *.h)
CC = avr-g++ -g -mmcu=atmega328p -D__AVR_ATmega328P__ -DF_CPU=8000000UL -Os -funsigned-char
LINK = avr-g++ -g -mmcu=atmega328p -lprintf_flt -lm -Wl,-u,vfprintf

$(TARGET): $(TARGET_ELF)
	avr-objcopy -j .text -j .data -O ihex $(TARGET_ELF) $(TARGET)

$(TARGET_ELF): .depend $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET_ELF) $(OBJECTS) $(LIBPATH) $(LIBS)

.cpp.o: $*.h common.h
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
