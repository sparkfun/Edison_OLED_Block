'use strict';
var edison = require('../index.js');

var edisonOled = function (edi) {
	this._edison = edi

    this._fontType;
    this._foreColor;
    this._drawMode;
    this._cursorX;
    this._cursorY;

    this.screenmemory = new Array(384);

	console.log('setup pins');
    //pin mraa /edison reference mapping - https://github.com/intel-iot-devkit/mraa/blob/master/docs/edison.md#intelr-breakout-board
	/*
		MRAA CS PIN = 23 which is GP110 (110) on Edison
		WE NEED CS PIN = 9 which is GP111  (111) on Edison
	*/
	this.cspin = new this._edison.Gpio(111, this._edison.OUTPUT, this._edison.HIGH); //chip select  --> edison_oled.c code uses 111 mraa uses 9
	this.dcpin = new this._edison.Gpio(14, this._edison.OUTPUT, this._edison.HIGH); //data/command --> edison_oled.c code uses 14 mraa uses 36
	this.rstpin = new this._edison.Gpio(15, this._edison.OUTPUT, this._edison.HIGH); //reset --> edison_oled.c code uses 15 mraa uses 48
	this.sclkpin = new this._edison.Gpio(109, this._edison.SPI, this._edison.HIGH); //sclk --> edison_oled.c code uses 109 mraa uses 10
	this.mosipin = new this._edison.Gpio(115, this._edison.SPI, this._edison.HIGH); //mosi --> edison_oled.c code uses 115 mraa uses 11
	//var misopin = new this._edison.Gpio(114, this._edison.SPI, this._edison.HIGH); edison_oled.c code uses 114 mraa uses 24

	console.log('pin values');
	console.log(this.cspin.pinRead());
	console.log(this.dcpin.pinRead());
	console.log(this.rstpin.pinRead());
	console.log(this.sclkpin.pinRead());
	console.log(this.mosipin.pinRead());	
	
	console.log('create spi');
	this._spi = new this._edison.Spi(this._edison.SPI_MODE_0, 10000000, false, this.cspin);
	console.log('spi created');

};

edisonOled.prototype.usleep = function (delay) {
	var waitTill = new Date(new Date().getTime() + delay);
	while (waitTill > new Date()) { }
};

edisonOled.prototype.begin = function () {

	this.screenmemory.fill(0);
	
	this.setFontType(0);
	this.setColor(this._edison.WHITE);
	this.setDrawMode(this._edison.NORM);
	this.setCursor(0, 0);

	this.spiSetup();

	this.rstpin.pinWrite(this._edison.HIGH); //(digitalWrite(rstPin, HIGH);
	this.usleep(5); // VDD (3.3V) goes high at start, lets just chill for 5 ms
	this.rstpin.pinWrite(this._edison.LOW); // bring reset low
	this.usleep(10); // wait 10ms
	this.rstpin.pinWrite(this._edison.HIGH);	//digitalWrite(rstPin, HIGH);

	this.command(this._edison.DISPLAYOFF);

	this.command(this._edison.SETDISPLAYCLOCKDIV);
	this.command(0X80);

	this.command(this._edison.SETMULTIPLEX);			// 0xA8
	this.command(0x2F);

	this.command(this._edison.SETDISPLAYOFFSET);		// 0xD3
	this.command(0x0);					// no offset

	this.command(this._edison.SETSTARTLINE | 0x0);	// line #0

	this.command(this._edison.CHARGEPUMP);			// enable charge pump
	this.command(0x14);

	this.command(this._edison.NORMALDISPLAY);			// 0xA6
	this.command(this._edison.DISPLAYALLONRESUME);	// 0xA4

	this.command(this._edison.SEGREMAP | 0x1);
	this.command(this._edison.COMSCANDEC);

	this.command(this._edison.SETCOMPINS);			// 0xDA
	this.command(0x12);

	this.command(this._edison.SETCONTRAST);			// 0x81
	this.command(0x8F);

	this.command(this._edison.SETPRECHARGE);			// 0xd9
	this.command(0xF1);

	this.command(this._edison.SETVCOMDESELECT);			// 0xDB
	this.command(0x40);

	this.command(this._edison.DISPLAYON);

	this.clear(this._edison.ALL);
};

edisonOled.prototype.command = function (c) {
	this.dcpin.pinWrite(this._edison.LOW); // DC pin LOW
	this.spiTransfer(c);
};

edisonOled.prototype.data = function (c) {
	this.dcpin.pinWrite(this._edison.HIGH); //DC pin HIGH
	this.spiTransfer(c);
};

edisonOled.prototype.setPageAddress = function (add) {
	add = 0xb0 | add;
	this.command(add);
	return;
};

edisonOled.prototype.setColumnAddress = function (add) {
	this.command((0x10 | (add >> 4)) + 0x02);
	this.command((0x0f & add));
	return;
}

edisonOled.prototype.clear = function (mode, c) {
	//	unsigned char page=6, col=0x40;

	if (!c) {
		c = 0;
	}

	if (mode == this._edison.ALL) {
		for (var i= 0; i < 8; i++)
		{
			this.setPageAddress(i);
			this.setColumnAddress(0);
			for (var j= 0; j < 0x80; j++)
			{
				this.data(c);
			}
		}
	}
	else {
		this.screenmemory.fill(c);			// (64 x 48) / 8 = 384
		this.display();
	}
};

edisonOled.prototype.display = function () {
	var i, j;
	for (i = 0; i < 6; i++) {
		this.setPageAddress(i);
		this.setColumnAddress(0);
		for (j = 0; j < 0x40; j++) {
			this.data(this.screenmemory[i * 0x40 + j]);
		}
	}
};

edisonOled.prototype.pixel = function(x, y, color, mode)
{
	if ((x < 0) || (x >= this._edison.LCDWIDTH) || (y < 0) || (y >= this._edison.LCDHEIGHT))
		return;

	if (!color) {
		color = this._foreColor;
	}
	if (!mode) {
		mode = this._drawMode;
	}
	console.log('pixel: ' + x + ', ' + y + ' ' + color + ' ' + mode);
	if (mode == this._edison.XOR) {
		if (color == this._edison.WHITE)
			this.screenmemory[x + (y / 8) * this._edison.LCDWIDTH] ^= (1 << (y % 8));
	}
	else {
		if (color == this._edison.WHITE)
			this.screenmemory[x + (y / 8) * this._edison.LCDWIDTH] |= (1 << (y % 8));
		else
			this.screenmemory[x + (y / 8) * this._edison.LCDWIDTH] &= ~(1 << (y % 8));
	}
}

edisonOled.prototype.setCursor = function (x, y) {
	this._cursorX = x;
	this._cursorY = y;
};

edisonOled.prototype.setFontType = function (type) {
	this._fontType = type;
	return true;
};

edisonOled.prototype.setColor = function (color) {
	this._foreColor = color;
};

edisonOled.prototype.setDrawMode = function (mode)
{
	this._drawMode = mode;
}

edisonOled.prototype.spiSetup = function () {
};

edisonOled.prototype.flipVertical = function(flip) {
	if (flip) {
		this.command(edison.COMSCANINC);
	}
	else {
		this.command(edison.COMSCANDEC);
	}
};

edisonOled.prototype.flipHorizontal = function(flip) {
	if (flip) {
		this.command(edison.SEGREMAP | 0x0);
	}
	else {
		this.command(edison.SEGREMAP | 0x1);
	}
};

edisonOled.prototype.spiTransfer = function (data) {
	this._spi.transferData(data);
};

var runTest = function () {
	//sparkfun logo should appear on the screen
	oled.screenmemory = [
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0x07, 0x06, 0x06, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x07, 0x0F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
	0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xFD, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	
	];
	console.log('show sparkfun logo');
	oled.display();
	oled.usleep(2500);	
	console.log('flip vertical');
	oled.flipVertical(true);	
	oled.usleep(2500);	
	console.log('flip horizontal');
	oled.flipHorizontal(true);
	oled.usleep(2500);		
	console.log('flip vertical');
	oled.flipVertical(false);
	oled.usleep(2500);			
	console.log('flip horizontal');
	oled.flipHorizontal(false);
	oled.usleep(2500);			
};
	
var oled = new edisonOled(edison);

oled.begin();
runTest();
oled.clear(edison.ALL);
