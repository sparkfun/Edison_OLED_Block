#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "../gpio/gpio.h"
#include "spi.h"

spiDevice::spiDevice(spiPort *port, \
                       unsigned char spiMode, \
                       long speed, \
                       bool lsbFirst, \
                       gpio *csPin)
{
		_port = port;
    _port->takeOwnership(this);
		_spiMode = spiMode;
		_speed = speed;
		_lsbFirst = lsbFirst;
		_csPin = csPin;
    if (csPin == NULL)
    {
      _customCS = false;
    }
    else
    {
      _customCS = true;
    }
}

void spiDevice::transferData(unsigned char *dataOut, \
                              unsigned char *dataIn, \
                              unsigned int len,
                              bool deselect)
{
	spi_ioc_transfer xfer;	
	xfer.tx_buf =(unsigned long)dataOut;
	xfer.rx_buf =(unsigned long)dataIn;
	xfer.len = len;
	xfer.speed_hz = _speed;
	xfer.bits_per_word = 8;
	if (deselect)
	{
		xfer.cs_change = 1;
	}
	else
	{
		xfer.cs_change = 0;
	}
	if (_port->doIOwn(this))
	{
		_port->takeOwnership(this);
	}
	_port->transferData(&xfer);
}

gpio* spiDevice::getCSPin()
{
  return this->_csPin;
}

bool spiDevice::customCS()
{
  return this->_customCS;
}
void spiDevice::CSLow()
{
  if (_csPin != NULL)
    _csPin->pinWrite(LOW);
}

void spiDevice::CSHigh()
{
  if (_csPin != NULL)
    _csPin->pinWrite(HIGH);
}

bool spiDevice::doLSBFirst()
{
  return _lsbFirst;
}

int spiDevice::getSPIMode()
{
  return _spiMode;
}

