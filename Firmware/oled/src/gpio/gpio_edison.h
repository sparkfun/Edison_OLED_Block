#ifndef __gpio_edison_h__
#define __gpio_edison_h__

// gpioHelper is defined independently in a platform-specific file, and
//  abstracts the nitty-gritty details of how a GPIO pin is actually accessed
//  for a given platform. Sometimes it'll be via file descriptors, other 
//  times, it'll be via register access.

// In the Edison, GPIO pins can be multiplexed to different functions. While 
//  *can* access them via the /sys/class/gpio route, it's more useful (although
//  more verbose) to go through /sys/kernel/debug/gpio_debug.

// One of the main reasons for using the debug route is that the Edison has some
//  internal muxing going on that you need to adjust to enable alternate
//  functions on the GPIO pins (things like PWM and SPI, for instance).

// This directory contains *all* valid native GPIO pins. Pins provided via the 
//  I/O expanders are not in this folder, however; to access them, you'll need
//  to go through the /sys/class/gpio/ interface.
#define GPIO_ROOT "/sys/kernel/debug/gpio_debug/"

// Each GPIO has a directory associated with it. Each directory has a number of 
//  super useful files inside. These strings link to those files.

// This file is read/write for current value of the pin. Write a string of "low"
//  or "high" to it, or read it to get "low" or "high" back.
#define GPIO_VALUE GPIO_ROOT "gpio%u/current_value"

// Current pin direction. This is *only* for input/output mode: more advanced 
//  modes, like I2C et al, are set via the next file, current_pinmux.
#define GPIO_DIRECTION GPIO_ROOT "gpio%u/current_direction"

// Current pin mux mode. There are, generally, 8 modes, denoted by strings
//  "mode0", etc. "mode0" indicates GPIO mode; others are something else and
//  we'll cover that in appropriate places.
#define GPIO_MODE GPIO_ROOT "gpio%u/current_pinmux"

// If we want to have a pullup or pulldown on the pin, we need to write
//  "pullup" or "pulldown" to this file.
#define GPIO_PULLMODE GPIO_ROOT "gpio%u/current_pullmode"

class gpioHelper
{
  friend class gpio;
  private:
    int _valueFileID;
    int _modeFileID;
    int _dirFileID;
    int _pullmodeFileID;
    void writeFile(int fileID, const char *value);
};

#endif

