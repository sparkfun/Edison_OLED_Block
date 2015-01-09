#ifndef __sparklibs_h_
#define __sparklibs_h_

enum exceptions { 
GPIO_NO_PIN = 0x10, // No GPIO file exists for that GPIO ID.
GPIO_BAD_MODE,    // Selected GPIO cannot take that mode.
GPIO_BAD_VALUE,   // Selected GPIO cannot take that value.
I2C_NO_DEV = 0x30,  // No I2C device with that ID can be accessed.
SPI_NO_DEV = 0x50,  // No SPI device at that location.
PWM_NO_DEV = 0x70,  // No PWM function at that location.
PWM_BAD_PERIOD,   // Period value is outside the acceptable limit.
PWM_BAD_DUTY,     // Duty cycle value is outside the acceptable limit.

};

#endif
