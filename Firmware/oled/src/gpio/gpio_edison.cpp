#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "gpio_edison.h"
#include "../sparklibs.h"

// Class constructor. initVal can be omitted; defaults to LOW.
gpio::gpio(int pinID, PIN_MODE initMode, PIN_VALUE initVal)
{
  /* We'll want to remember the pin number associated with this object, so we
      can provide the user with useful information when we throw exceptions. */
  this->_pinNumber = pinID;

  char tBuffer[75]; 

  /* GPIO functions are handled through accessing a resource in the filesystem.
      It's possible that these resources may not exist; in that case, we'll
      throw an exception, because creating a GPIO object with no resource to
      back it up makes no sense. */

  /* A few words here on the role of gpioHelper: since this library is
      meant to be platform agnostic, we want to divorce the filesystem-related
      elements of the GPIO from the main GPIO object. After all, not all of the
      target architectures this will work for will have a filesystem! */
  helper = new gpioHelper(); 	

  /* Create the path for the direction setting file and open it. This file
      handles GPIO direction only. */
	sprintf(tBuffer, GPIO_DIRECTION, pinID);
	helper->_dirFileID = open(tBuffer, O_RDWR);
  
  /* The mode file (aka pinmux) handles some weird internal logic on the Edison
      which routes the various IO peripherals (SPI, PWM) to the physical pin on
      the Edison connector. */
	sprintf(tBuffer, GPIO_MODE, pinID);
	helper->_modeFileID = open(tBuffer, O_RDWR);

  // Same process for the "value" file, same caveat..
	sprintf(tBuffer, GPIO_VALUE, pinID);
  helper->_valueFileID = open(tBuffer, O_RDWR);

  // And for pullmode.
  sprintf(tBuffer, GPIO_PULLMODE, pinID);
  helper->_pullmodeFileID = open(tBuffer, O_RDWR);

  /* It's not hard to imagine a case where the user would pass a bogus pin ID,
      so let's just make sure we handle that gracefully by throwing an
      exception. */
  if ((helper->_modeFileID == -1) ||
      (helper->_dirFileID == -1)  ||
      (helper->_pullmodeFileID == -1)  ||
      (helper->_valueFileID == -1)) 
  {
    throw (GPIO_NO_PIN << 16) + pinID;
    return;
  }

  // Write the initial value to the pin.
  this->pinWrite(initVal);

  // Write the initial mode to the pin.
  this->pinMode(initMode);
    
  // If we make it here without throwing any exceptions, we have a pin object!
}

// Destructor. Nothing special here.
gpio::~gpio()
{
	close(helper->_modeFileID);
	close(helper->_valueFileID);
  close(helper->_dirFileID);
  close(helper->_pullmodeFileID);
  delete helper;
}

// Mode setting function. Throws exception GPIO_BAD_MODE if you pass it an
//  invalid mode value.
void gpio::pinMode(PIN_MODE newMode)
{
  char tBuffer[10];
  switch (newMode)
  {
    // For an input or output, we write to the direction file ("in" or "out"),
    //  and to the pinmux file ("mode0"). We can assume that this is valid for
    //  all possible pins.
    case INPUT:
      sprintf(tBuffer, "in");
      helper->writeFile(helper->_dirFileID, tBuffer);
      sprintf(tBuffer, "mode0");
      helper->writeFile(helper->_modeFileID, tBuffer);
      sprintf(tBuffer, "nopull");
      helper->writeFile(helper->_pullmodeFileID, tBuffer);
      break;
    case OUTPUT:
      sprintf(tBuffer, "out");
      helper->writeFile(helper->_dirFileID, tBuffer);
      sprintf(tBuffer, "mode0");
      helper->writeFile(helper->_modeFileID, tBuffer);
      sprintf(tBuffer, "nopull");
      helper->writeFile(helper->_pullmodeFileID, tBuffer);
      break;
    case SPI:
      // We need to make sure we're on a valid SPI pin; that's pins 109, 111,
      //  114 and 115. Any other pin throws an exception. To activate SPI mode, 
      //  we need to set pinmux to mode1.
      switch (this->_pinNumber)
      {
        case 109: // SPI clock.
        case 111: // SPI CS
        case 115: // SPI MOSI
          sprintf(tBuffer, "out");
          helper->writeFile(helper->_dirFileID, tBuffer);
          sprintf(tBuffer, "mode1");
          helper->writeFile(helper->_modeFileID, tBuffer);
          sprintf(tBuffer, "nopull");
          helper->writeFile(helper->_pullmodeFileID, tBuffer);
          break;
        case 114: // SPI MISO
          sprintf(tBuffer, "in");
          helper->writeFile(helper->_dirFileID, tBuffer);
          sprintf(tBuffer, "mode1");
          helper->writeFile(helper->_modeFileID, tBuffer);
          sprintf(tBuffer, "nopull");
          helper->writeFile(helper->_pullmodeFileID, tBuffer);
          break;
        default:
          throw (GPIO_BAD_MODE << 16) + this->_pinNumber;
          return;
      }
      break;
    case PWM:
      // We need to make sure we're on a valid PWM pin; that's pins 12, 13, 182
      //  and 183, as PWM0-3 respectively. We're going to assume that the user
      //  took care of exporting the PWM pin elsewhere in their code (really,
      //  that just means that they created a PWMPin object); otherwise, they
      //  can set the pin to PWM mode but not really use it.
      switch(this->_pinNumber)
      {
        case 12:
        case 13:
        case 182:
        case 183:
          sprintf(tBuffer, "out");
          helper->writeFile(helper->_dirFileID, tBuffer);
          sprintf(tBuffer, "mode1");
          helper->writeFile(helper->_modeFileID, tBuffer);
          sprintf(tBuffer, "nopull");
          helper->writeFile(helper->_pullmodeFileID, tBuffer);
          break;
        default:
          throw (GPIO_BAD_MODE << 16) + this->_pinNumber;
          return;
      }
      break;
    case INPUT_PU:
      sprintf(tBuffer, "in");
      helper->writeFile(helper->_dirFileID, tBuffer);
      sprintf(tBuffer, "mode0");
      helper->writeFile(helper->_modeFileID, tBuffer);
      sprintf(tBuffer, "pullup");
      helper->writeFile(helper->_pullmodeFileID, tBuffer);
      break;
    case INPUT_PD:
      sprintf(tBuffer, "in");
      helper->writeFile(helper->_dirFileID, tBuffer);
      sprintf(tBuffer, "mode0");
      helper->writeFile(helper->_modeFileID, tBuffer);
      sprintf(tBuffer, "pulldown");
      helper->writeFile(helper->_pullmodeFileID, tBuffer);
      break;
    case I2C:
    case UART:
    case ADC:
    default:
      throw (GPIO_BAD_MODE<<16) + this->_pinNumber;
      return;
  }
}

// Pin value write function. Throws exception GPIO_BAD_VALUE if you pass it a
//  bad level value.
void gpio::pinWrite(PIN_VALUE newLevel)
{
  char tBuffer[5];
  // Writing to /sys/class/gpio#/value, we'd write '1' or '0'. To the debug
  //  file, we write 'low' or 'high'.
  if (newLevel == LOW)
  {
    sprintf(tBuffer, "low");
  }
  else if (newLevel == HIGH)
  {
    sprintf(tBuffer, "high");
  }
  else
  {
    throw (GPIO_BAD_VALUE<<16) + _pinNumber;
    return;
  }

	helper->writeFile(helper->_valueFileID, tBuffer);
}

PIN_VALUE gpio::pinRead()
{
	lseek(helper->_valueFileID, 0, SEEK_SET);
	char buffer[4];
	read(helper->_valueFileID, &buffer, 4);
  if (buffer[0] == 'h')
  {
    return HIGH;
  }
  else
  {
    //printf("IT'S LOW\n\r");
    return LOW;
  }
}

// writeFile() is made a function so we don't need to explicitly do the lseek()
//  to get to the top of the file before every write. It feels derpy to me that
//  a file like this needs you to seek to the top, but there it is.
void gpioHelper::writeFile(int fileID, const char *buffer)
{
  lseek(fileID, 0, SEEK_SET);   // Make sure we're at the top of the file!
  write(fileID, buffer, strlen(buffer));
}

