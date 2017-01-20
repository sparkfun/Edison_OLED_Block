CPP=g++
CPPFLAGS=-c -Wall -m32 -g
LDFLAGS=-m32
SOURCES=\
	src/spi/spi_port_edison.cpp \
	src/spi/spi_device_edison.cpp \
	src/oled/Edison_OLED.cpp \
	src/gpio/gpio_edison.cpp \
	src/edison-oled.cpp \


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oled

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CPP) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CPP) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) `find src -type f -name '*.o'`
