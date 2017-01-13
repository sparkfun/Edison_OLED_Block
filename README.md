NodeJS Wrapper for SparkFun Edison OLED Block
=============================================

Developing a node app for your Edison? Need to use the OLED display from SparkFun, GPIO pins or SPI? You're in luck! 

Features

	* OLED 
	* GPIO
	* SPI

NOTES

	* This has only been tested with the SparkFun Edison OLED Block, but GPIO and SPI should (may possibly) work with other hardware.
	* SPI - need to use a CS pin on Edison other than the default pin 23 (GP110), the MRAA library won't work. This library works using a pin of your choosing (tested with pin 9 (GP111)). 
	* GPIO pin mraa /edison reference mapping may be useful - https://github.com/intel-iot-devkit/mraa/blob/master/docs/edison.md#intelr-breakout-board
	* The files in the Tests folder are meant to be run on an Intel Edison as they interact with the hardware.

Installation
============
This module is only designed to work on the Intel Edison.

```
$ npm install edison-oled
```

You may need to use the --unsafe-perm option if you get the warning "cannot run in wd".

```
$ npm install edison-oled --unsafe-perm
```

API
============
Example using the OLED screen

```javascript
var edison = require('edison-oled');

//create Oled
var oled = new edison.Oled();

//setup input buttons
var btnUp = new edison.Gpio(47, edison.INPUT);
var btnDown = new edison.Gpio(44, edison.INPUT);
var btnLeft = new edison.Gpio(165, edison.INPUT);
var btnRight = new edison.Gpio(45, edison.INPUT);
var btnSelect = new edison.Gpio(48, edison.INPUT);
var btnA = new edison.Gpio(49, edison.INPUT);
var btnB = new edison.Gpio(46, edison.INPUT);

//setup screen
oled.begin();
oled.clear(0);
oled.display();
oled.setFontType(0);	

//draw on screen
oled.clear();
oled.setCursor(14, 5);
oled.print("Press A");
oled.setCursor(2, 13);
oled.print("for single");
oled.setCursor(14, 30);
oled.print("Press B");
oled.setCursor(6, 38);
oled.print("for multi");
oled.display();	

//wait for user to make a choice using Button A or Button B
while (btnA.pinRead() == edison.HIGH && btnB.pinRead() == edison.HIGH) {}

```

Example uses SPI and GPIO to communicate with OLED hardware (not a complete example, see test-spi.js for full code)

```javascript
var edison = require('edison-oled');

//setup pins needed for SPI
var cspin = new edison.Gpio(111, edison.OUTPUT, edison.HIGH); //chip select  --> edison_oled.c code uses 111 mraa uses 9
var dcpin = new edison.Gpio(14, edison.OUTPUT, edison.HIGH); //data/command --> edison_oled.c code uses 14 mraa uses 36
var rstpin = new edison.Gpio(15, edison.OUTPUT, edison.HIGH); //reset --> edison_oled.c code uses 15 mraa uses 48
var sclkpin = new edison.Gpio(109, edison.SPI, edison.HIGH); //sclk --> edison_oled.c code uses 109 mraa uses 10
var mosipin = new edison.Gpio(115, edison.SPI, edison.HIGH); //mosi --> edison_oled.c code uses 115 mraa uses 11
var misopin = new edison.Gpio(114, edison.SPI, edison.HIGH); //miso --> edison_oled.c code uses 114 mraa uses 24

//create spi
var spi = new edison.Spi(edison.SPI_MODE_0, 10000000, false, cspin);

//reset screen
rstpin.pinWrite(edison.HIGH);
//should add a wait time here for a few milliseconds
rstpin.pinWrite(edison.LOW);
//should add a wait time here for a few milliseconds
rstpin.pinWrite(edison.HIGH);
	
//send commands to OLED
dcpin.pinWrite(edison.LOW); // DC pin LOW
spi.transferData(edison.DISPLAYOFF);
spi.transferData(edison.SETDISPLAYCLOCKDIV);
spi.transferData(0x80);
	
//send data to OLED
dcpin.pinWrite(edison.HIGH); //DC pin HIGH
spi.transferData(0xFF);

```

SparkFun Edison OLED Block
===========================

![SparkFun Edison OLED Block](https://cdn.sparkfun.com//assets/parts/1/0/0/3/6/13035-01.jpg)

[*SparkFun Edison OLED Block(DEV-13035)*](https://www.sparkfun.com/products/13035)

Equip your Edison with a graphic display using the Edison OLED board! This board features a 0.66", 64x48 pixel monochrome OLED.

To add some control over your Edison and the OLED, this board also includes a small joystick and a pair of push-buttons. Use them to create a game, file navigator, or more!

Repository Contents
-------------------
* **/Firmware** - Example Pong sketch to demonstrate OLED Functionality. 
* **/Hardware** - All Eagle design files (.brd, .sch)
* **/Production** - Test bed files and production panel files

Documentation
--------------
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-blocks-for-intel-edison---oled-block)** - Basic hookup guide for the OLED Block.
* **[SparkFun Fritzing repo](https://github.com/sparkfun/Fritzing_Parts)** - Fritzing diagrams for SparkFun products.
* **[SparkFun 3D Model repo](https://github.com/sparkfun/3D_Models)** - 3D models of SparkFun products. 
* **[SparkFun Graphical Datasheets](https://github.com/sparkfun/Graphical_Datasheets)** -Graphical Datasheets for various SparkFun products.


License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
