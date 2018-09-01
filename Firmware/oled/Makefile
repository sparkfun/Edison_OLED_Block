CPP=g++
CPPFLAGS=-c -Wall -m32 -g
LDFLAGS=-m32
SOURCES=\
	src/spi/spi_port_edison.cpp \
	src/spi/spi_device_edison.cpp \
	src/oled/Edison_OLED.cpp \
	src/gpio/gpio_edison.cpp \
#	src/edison-oled.cpp \


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oled oled-input

all: $(SOURCES) $(EXECUTABLE)
	
oled: $(OBJECTS) src/edison-oled.o 
	$(CPP) $(LDFLAGS) $(OBJECTS) src/edison-oled.o -o $@

oled-input: $(OBJECTS) src/input-oled.o 
	$(CPP) $(LDFLAGS) $(OBJECTS) src/input-oled.o -o $@

.cpp.o:
	$(CPP) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) `find src -type f -name '*.o'`
