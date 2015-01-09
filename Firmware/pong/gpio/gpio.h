#ifndef __gpio_h__
#define __gpio_h__

/* Values that can be taken by a pin. LOW and HIGH are obvious, and NONE is
    provided as a useful alternative that may never be used. */
enum PIN_VALUE
{
  LOW,
  HIGH,
  NONE,
};

/* Pin modes. InPUT and OUTPUT should be nearly universal; the others may or
    may not be needed depending on platform. On platforms where a particular
    value isn't allowed, an exception will be thrown. */
enum PIN_MODE
{
  INPUT,
  OUTPUT,
  INPUT_PU,
  INPUT_PD,
  SPI,
  I2C,
  PWM,
  ADC,
  UART,
};

/* This is the top-level class for a gpio pin. It should be suitable for all
    platforms, since it provides the minimum needed for a GPIO pin. All
    functions will throw exceptions defined in the sparklibs.h file. */
class gpio
{
	public:
		gpio(int pinID, PIN_MODE initMode, PIN_VALUE initVal=LOW);
		~gpio();
		void pinMode(PIN_MODE newMode);
		void pinWrite(PIN_VALUE newLevel);
		PIN_VALUE pinRead();

	private:
    /* The gpioHelper is provided to provide a place to hide the
        down-and-dirty implementation details that vary from one architecture 
        to the next; that way, this class doesn't need to provide functions 
        for, say, file IO which wouldn't get used in an environment without a 
        filesystem. */
    class gpioHelper *helper; 
    int _pinNumber;
    gpio();
};

#endif

