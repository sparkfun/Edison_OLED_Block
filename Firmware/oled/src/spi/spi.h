#ifndef __spi_h__
#define __spi_h__

#include <linux/spi/spidev.h>
#include <stddef.h>

struct spi_ioc_transfer;
class spiDevice;
class gpio;

class spiPort
{
	public:
		spiPort(int spiDriver=5);
		~spiPort();
    bool doIOwn(spiDevice *curiousDevice);
    void takeOwnership(spiDevice *bossyDevice);
		void transferData(spi_ioc_transfer *xfer);

	private:
		void configurePort();
		void configurePins(int spiDriver); 

		int _spiFile;   
		spiDevice *_portOwner;
		gpio *_MOSI;
		gpio *_MISO;
		gpio *_SCK;
		gpio *_CS;
};

class spiDevice
{
	public:
    spiDevice(){};
		spiDevice(spiPort *port, \
		           unsigned char spiMode = SPI_MODE_0, \
							 long speed = 1000000, \
							 bool lsbFirst = false, \
							 gpio *csPin = NULL);
		void transferData(unsigned char *outData, \
		                  unsigned char *inData = NULL, \
											unsigned int len = 1, \
											bool deselect = true);
    bool customCS();
    void CSLow();
    void CSHigh();
    bool doLSBFirst();
    int getSPIMode();
    gpio* getCSPin();
	protected:
		gpio *_csPin;
    bool _customCS;
		spiPort *_port;
		unsigned char _spiMode;
		long _speed;
		bool _lsbFirst;

};

#endif

