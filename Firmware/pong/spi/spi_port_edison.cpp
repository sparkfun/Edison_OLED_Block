#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "../sparklibs.h"
#include "../gpio/gpio.h"
#include "spi.h"


spiPort::~spiPort()
{
	close(_spiFile);
}

spiPort::spiPort(int spiDriver)
									 
{
  const char *path = "/dev/spidev5.1";
	_spiFile = open(path, O_RDWR);
  if (_spiFile < 0) throw SPI_NO_DEV;
  _portOwner = NULL;

}

// configurePort() only handles the parts which must be handled apart from
//  the message struct.
void spiPort::configurePort()
{
	if (_portOwner == NULL)
	{
		return;
	}

	// We'll make a little temp variable to transmit the various mode info to 
	//  the SPI driver.
	int temp = 0;
  
	// The boolean lsbfirst is a rare case where the bit order should be 
	//  reversed during transmission; it corresponds to the SPI_LSB_FIRST
	//  constant in the spidev.h file.
 	if (_portOwner->doLSBFirst()) 
	{
		temp = SPI_LSB_FIRST;
		ioctl(_spiFile, SPI_IOC_WR_LSB_FIRST, &temp);
	}
	else
  {	
    temp = 0;
		ioctl(_spiFile, SPI_IOC_WR_LSB_FIRST, &temp);
	}

  temp = _portOwner->getSPIMode();
	ioctl(_spiFile, SPI_IOC_WR_MODE, &temp);
}

void spiPort::transferData(spi_ioc_transfer *xfer)
{
	ioctl(_spiFile, SPI_IOC_MESSAGE(1), xfer);
}

bool spiPort::doIOwn(spiDevice *curiousDevice)
{
  return (curiousDevice == _portOwner);
} 

void spiPort::takeOwnership(spiDevice *bossyDevice)
{
  _portOwner = bossyDevice;
  configurePort();
}

