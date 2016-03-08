/******************************************************************************
	Edison_OLED.cpp
	Main source code for the Edison OLED Block C++ Library

	Jim Lindblom @ SparkFun Electronics
	January 8, 2014
	https://github.com/sparkfun/Edison_OLED_Block/tree/master/Firmware/

	This file defines the hardware interface for the Edison OLED Block

	This code was heavily based around the MicroView library, written by GeekAmmo
	(https://github.com/geekammo/MicroView-Arduino-Library), and released under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "Edison_OLED.h"
#include "../spi/spi.h"
#include "../gpio/gpio.h"
#include "edison_fonts.h" // External file to store font bit-map arrays
#include <stdlib.h>
#include <string.h>	// for memset
#include <stdio.h>	// for sprintf
#include <unistd.h> // for usleep

// Change the total fonts included
#define TOTALFONTS		4
#define recvLEN			100
char serInStr[recvLEN];
unsigned char serCmd[recvLEN];

// Add the font name as declared in the header file
const unsigned char *edOLED::fontsPointer[]={
	font5x7
	,font8x16
	,sevensegment
	,fontlargenumber
};

/** \brief edOLED screen buffer.

Page buffer 64 x 48 divided by 8 = 384 unsigned chars
Page buffer is required because in SPI mode, the host cannot read the SSD1306's
GDRAM of the controller.  This page buffer serves as a scratch RAM for graphical
functions.  All drawing function will first be drawn on this page buffer, only
upon calling display() function will transfer the page buffer to the actual LCD
controller's memory.
*/
static unsigned char screenmemory [] = {
	/* LCD Memory organised in 64 horizontal pixel and 6 rows of unsigned char
	 B  B .............B  -----
	 y  y .............y        \
	 t  t .............t         \
	 e  e .............e          \
	 0  1 .............63          \
	                                \
	 D0 D0.............D0            \
	 D1 D1.............D1            / ROW 0
	 D2 D2.............D2           /
	 D3 D3.............D3          /
	 D4 D4.............D4         /
	 D5 D5.............D5        /
	 D6 D6.............D6       /
	 D7 D7.............D7  ----
	*/
	//SparkFun Electronics LOGO 
	
	// ROW0, unsigned char0 to unsigned char63
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0x07, 0x06, 0x06, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	// ROW1, unsigned char64 to unsigned char127
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x07, 0x0F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	// ROW2, unsigned char128 to unsigned char191
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
	0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xFD, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW3, unsigned char192 to unsigned char255
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW4, unsigned char256 to unsigned char319
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	// ROW5, unsigned char320 to unsigned char383
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Pin definitions:
gpio CS_PIN(111, OUTPUT, HIGH);
gpio RST_PIN(15, OUTPUT, HIGH);
gpio DC_PIN(14, OUTPUT, HIGH);
gpio SCLK_PIN(109, SPI, HIGH);
gpio MOSI_PIN(115, SPI, HIGH);

// SPI set up:
spiPort *spi5 = new spiPort();
//spiDevice oledSPI(spi5);
spiDevice oledSPI(spi5, SPI_MODE_0, 10000000, false, &CS_PIN);
#define pgm_read_byte(x) (*(x))

edOLED::edOLED()
{

}

/** \brief Initialization of edOLED Library.

    Setup IO pins for SPI port then send initialization commands to the
    SSD1306 controller inside the OLED.
*/
void edOLED::begin()
{
	// default 5x7 font
	setFontType(0);
	setColor(WHITE);
	setDrawMode(NORM);
	setCursor(0,0);
	
	spiSetup();
	
	RST_PIN.pinWrite(HIGH); //(digitalWrite(rstPin, HIGH);
	usleep(5000); // VDD (3.3V) goes high at start, lets just chill for 5 ms
	RST_PIN.pinWrite(LOW); // bring reset low
	usleep(10000); // wait 10ms
	RST_PIN.pinWrite(HIGH);	//digitalWrite(rstPin, HIGH);

	// Init sequence for 64x48 OLED module
	command(DISPLAYOFF);			// 0xAE

	command(SETDISPLAYCLOCKDIV);	// 0xD5
	command(0x80);					// the suggested ratio 0x80

	command(SETMULTIPLEX);			// 0xA8
	command(0x2F);

	command(SETDISPLAYOFFSET);		// 0xD3
	command(0x0);					// no offset

	command(SETSTARTLINE | 0x0);	// line #0

	command(CHARGEPUMP);			// enable charge pump
	command(0x14);

	command(NORMALDISPLAY);			// 0xA6
	command(DISPLAYALLONRESUME);	// 0xA4

	command(SEGREMAP | 0x1);
	command(COMSCANDEC);

	command(SETCOMPINS);			// 0xDA
	command(0x12);

	command(SETCONTRAST);			// 0x81
	command(0x8F);

	command(SETPRECHARGE);			// 0xd9
	command(0xF1);
	
	command(SETVCOMDESELECT);			// 0xDB
	command(0x40);

	command(DISPLAYON);				//--turn on oled panel
	clear(ALL);						// Erase hardware memory inside the OLED
}

/** \brief SPI command.

    Setup DC and SS pins, then send command via SPI to SSD1306 controller.
*/
void edOLED::command(unsigned char c)
{
	DC_PIN.pinWrite(LOW); // DC pin LOW
	spiTransfer(c);
}

/** \brief SPI data.

    Setup DC and SS pins, then send data via SPI to SSD1306 controller.
*/
void edOLED::data(unsigned char c)
{
	DC_PIN.pinWrite(HIGH);	// DC HIGH
	spiTransfer(c);
}

/** \brief Set SSD1306 page address.

    Send page address command and address to the SSD1306 OLED controller.
*/
void edOLED::setPageAddress(unsigned char add)
{
	add=0xb0|add;
	command(add);
	return;
}

/** \brief Set SSD1306 column address.

    Send column address command and address to the SSD1306 OLED controller.
*/
void edOLED::setColumnAddress(unsigned char add)
{
	command((0x10|(add>>4))+0x02);
	command((0x0f&add));
	return;
}

/** \brief Clear screen buffer or SSD1306's memory.
 
    To clear GDRAM inside the LCD controller, pass in the variable mode =
    ALL and to clear screen page buffer pass in the variable mode = PAGE.
*/
void edOLED::clear(unsigned char mode)
{
	//	unsigned char page=6, col=0x40;
	if (mode==ALL)
	{
		for (int i=0;i<8; i++)
		{
			setPageAddress(i);
			setColumnAddress(0);
			for (int j=0; j<0x80; j++)
			{
				data(0);
			}
		}
	}
	else
	{
		memset(screenmemory,0,384);			// (64 x 48) / 8 = 384
	}
}

/** \brief Clear or replace screen buffer or SSD1306's memory with a character.	

	To clear GDRAM inside the LCD controller, pass in the variable mode = ALL
	with c character and to clear screen page buffer, pass in the variable
	mode = PAGE with c character.
*/
void edOLED::clear(unsigned char mode, unsigned char c)
{
	//unsigned char page=6, col=0x40;
	if (mode==ALL)
	{
		for (int i=0;i<8; i++)
		{
			setPageAddress(i);
			setColumnAddress(0);
			for (int j=0; j<0x80; j++)
			{
				data(c);
			}
		}
	}
	else
	{
		memset(screenmemory,c,384);			// (64 x 48) / 8 = 384
		display();
	}	
}

/** \brief Invert display.

    The WHITE color of the display will turn to BLACK and the BLACK will turn
    to WHITE.
*/
void edOLED::invert(unsigned char inv)
{
	if (inv)
		command(INVERTDISPLAY);
	else
		command(NORMALDISPLAY);
}

/** \brief Set contrast.

    OLED contract value from 0 to 255. Note: Contrast level is not very obvious.
*/
void edOLED::contrast(unsigned char contrast)
{
	command(SETCONTRAST);			// 0x81
	command(contrast);
}

/** \brief Transfer display memory.

    Bulk move the screen buffer to the SSD1306 controller's memory so that images/graphics drawn on the screen buffer will be displayed on the OLED.
*/
void edOLED::display(void)
{
	unsigned char i, j;
	
	for (i=0; i<6; i++)
	{
		setPageAddress(i);
		setColumnAddress(0);
		for (j=0;j<0x40;j++)
		{
			data(screenmemory[i*0x40+j]);
		}
	}
}

/** \brief write a character to the display

*/
unsigned char edOLED::write(unsigned char c)
{
	if (c == '\n')
	{
		cursorY += fontHeight;
		cursorX  = 0;
	}
	else if (c == '\r')
	{
		// skip
	}
	else
	{
		drawChar(cursorX, cursorY, c, foreColor, drawMode);
		cursorX += fontWidth+1;
		if ((cursorX > (LCDWIDTH - fontWidth)))
		{
			cursorY += fontHeight;
			cursorX = 0;
		}
	}

	return 1;
}


void edOLED::print(const char * c)
{
	int len = strlen(c);

	for (int i=0; i<len; i++)
	{
		write(c[i]);
	}
}

void edOLED::print(int d)
{
	char temp[24];
	sprintf(temp, "%d", d);
	print(temp);
}

void edOLED::print(float f)
{
	char temp[24];
	sprintf(temp, "%.2f", f);
	print(temp);
}

/** \brief Set cursor position.

    edOLED's cursor position to x,y.
*/
void edOLED::setCursor(unsigned char x, unsigned char y)
{
	cursorX=x;
	cursorY=y;
}

/** \brief Draw pixel.

    Draw pixel using the current fore color and current draw mode in the screen buffer's x,y position.
*/
void edOLED::pixel(unsigned char x, unsigned char y)
{
	pixel(x,y,foreColor,drawMode);
}

/** \brief Draw pixel with color and mode.

    Draw color pixel in the screen buffer's x,y position with NORM or XOR draw mode.
*/
void edOLED::pixel(unsigned char x, unsigned char y, unsigned char color, unsigned char mode)
{
	if ((x<0) ||  (x>=LCDWIDTH) || (y<0) || (y>=LCDHEIGHT))
		return;

	if (mode==XOR)
	{
		if (color==WHITE)
			screenmemory[x+ (y/8)*LCDWIDTH] ^= (1<<(y%8));
	}
	else
	{
		if (color==WHITE)
			screenmemory[x+ (y/8)*LCDWIDTH] |= (1<<(y%8));
		else
			screenmemory[x+ (y/8)*LCDWIDTH] &= ~(1<<(y%8));
	}
}

/** \brief Draw line.

    Draw line using current fore color and current draw mode from x0,y0 to x1,y1 of the screen buffer.
*/
void edOLED::line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
	line(x0,y0,x1,y1,foreColor,drawMode);
}

/** \brief Draw line with color and mode.

    Draw line using color and mode from x0,y0 to x1,y1 of the screen buffer.
*/
void edOLED::line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char color, unsigned char mode)
{
	unsigned char steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swapOLED(x0, y0);
		swapOLED(x1, y1);
	}

	if (x0 > x1)
	{
		swapOLED(x0, x1);
		swapOLED(y0, y1);
	}

	unsigned char dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0<x1; x0++)
	{
		if (steep)
		{
			pixel(y0, x0, color, mode);
		} else
		{
			pixel(x0, y0, color, mode);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

/** \brief Draw horizontal line.

    Draw horizontal line using current fore color and current draw mode from x,y to x+width,y of the screen buffer.
*/
void edOLED::lineH(unsigned char x, unsigned char y, unsigned char width)
{
	line(x,y,x+width,y,foreColor,drawMode);
}

/** \brief Draw horizontal line with color and mode.

    Draw horizontal line using color and mode from x,y to x+width,y of the screen buffer.
*/
void edOLED::lineH(unsigned char x, unsigned char y, unsigned char width, unsigned char color, unsigned char mode)
{
	line(x,y,x+width,y,color,mode);
}

/** \brief Draw vertical line.

    Draw vertical line using current fore color and current draw mode from x,y to x,y+height of the screen buffer.
*/
void edOLED::lineV(unsigned char x, unsigned char y, unsigned char height)
{
	line(x,y,x,y+height,foreColor,drawMode);
}

/** \brief Draw vertical line with color and mode.

    Draw vertical line using color and mode from x,y to x,y+height of the screen buffer.
*/
void edOLED::lineV(unsigned char x, unsigned char y, unsigned char height, unsigned char color, unsigned char mode)
{
	line(x,y,x,y+height,color,mode);
}

/** \brief Draw rectangle.

    Draw rectangle using current fore color and current draw mode from x,y to x+width,y+height of the screen buffer.
*/
void edOLED::rect(unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{
	rect(x,y,width,height,foreColor,drawMode);
}

/** \brief Draw rectangle with color and mode.

    Draw rectangle using color and mode from x,y to x+width,y+height of the screen buffer.
*/
void edOLED::rect(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char color , unsigned char mode)
{
	unsigned char tempHeight;

	lineH(x,y, width, color, mode);
	lineH(x,y+height-1, width, color, mode);

	tempHeight=height-2;

	// skip drawing vertical lines to avoid overlapping of pixel that will
	// affect XOR plot if no pixel in between horizontal lines
	if (tempHeight<1)
		return;

	lineV(x,y+1, tempHeight, color, mode);
	lineV(x+width-1, y+1, tempHeight, color, mode);
}
	
/** \brief Draw filled rectangle.

    Draw filled rectangle using current fore color and current draw mode from x,y to x+width,y+height of the screen buffer.
*/
void edOLED::rectFill(unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{
	rectFill(x,y,width,height,foreColor,drawMode);
}

/** \brief Draw filled rectangle with color and mode.

    Draw filled rectangle using color and mode from x,y to x+width,y+height of the screen buffer.
*/	
void edOLED::rectFill(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char color , unsigned char mode)
{
	for (int i=x; i<x+width;i++)
	{
		lineV(i,y, height, color, mode);
	}
}

/** \brief Draw circle.

    Draw circle with radius using current fore color and current draw mode at x,y of the screen buffer.
*/
void edOLED::circle(unsigned char x0, unsigned char y0, unsigned char radius)
{
	circle(x0,y0,radius,foreColor,drawMode);
}

/** \brief Draw circle with color and mode.

    Draw circle with radius using color and mode at x,y of the screen buffer.
*/
void edOLED::circle(unsigned char x0, unsigned char y0, unsigned char radius, unsigned char color, unsigned char mode)
{
	int8_t f = 1 - radius;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * radius;
	int8_t x = 0;
	int8_t y = radius;

	pixel(x0, y0+radius, color, mode);
	pixel(x0, y0-radius, color, mode);
	pixel(x0+radius, y0, color, mode);
	pixel(x0-radius, y0, color, mode);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		pixel(x0 + x, y0 + y, color, mode);
		pixel(x0 - x, y0 + y, color, mode);
		pixel(x0 + x, y0 - y, color, mode);
		pixel(x0 - x, y0 - y, color, mode);

		pixel(x0 + y, y0 + x, color, mode);
		pixel(x0 - y, y0 + x, color, mode);
		pixel(x0 + y, y0 - x, color, mode);
		pixel(x0 - y, y0 - x, color, mode);

	}
}

/** \brief Draw filled circle.

    Draw filled circle with radius using current fore color and current draw mode at x,y of the screen buffer.
*/
void edOLED::circleFill(unsigned char x0, unsigned char y0, unsigned char radius)
{
	circleFill(x0,y0,radius,foreColor,drawMode);
}

/** \brief Draw filled circle with color and mode.

    Draw filled circle with radius using color and mode at x,y of the screen buffer.
*/
void edOLED::circleFill(unsigned char x0, unsigned char y0, unsigned char radius, unsigned char color, unsigned char mode)
{
	int8_t f = 1 - radius;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * radius;
	int8_t x = 0;
	int8_t y = radius;

	// Temporary disable fill circle for XOR mode.
	if (mode==XOR) return;

	for (unsigned char i=y0-radius; i<=y0+radius; i++)
	{
		pixel(x0, i, color, mode);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (unsigned char i=y0-y; i<=y0+y; i++)
		{
			pixel(x0+x, i, color, mode);
			pixel(x0-x, i, color, mode);
		}
		for (unsigned char i=y0-x; i<=y0+x; i++)
		{
			pixel(x0+y, i, color, mode);
			pixel(x0-y, i, color, mode);
		}
	}
}

/** \brief Get LCD height.

    The height of the LCD return as unsigned char.
*/
unsigned char edOLED::getLCDHeight(void)
{
	return LCDHEIGHT;
}

/** \brief Get LCD width.

    The width of the LCD return as unsigned char.
*/	
unsigned char edOLED::getLCDWidth(void)
{
	return LCDWIDTH;
}

/** \brief Get font width.

    The cucrrent font's width return as unsigned char.
*/	
unsigned char edOLED::getFontWidth(void)
{
	return fontWidth;
}

/** \brief Get font height.

    The current font's height return as unsigned char.
*/
unsigned char edOLED::getFontHeight(void)
{
	return fontHeight;
}

/** \brief Get font starting character.

    Return the starting ASCII character of the currnet font, not all fonts start with ASCII character 0. Custom fonts can start from any ASCII character.
*/
unsigned char edOLED::getFontStartChar(void)
{
	return fontStartChar;
}

/** \brief Get font total characters.

    Return the total characters of the current font.
*/
unsigned char edOLED::getFontTotalChar(void)
{
	return fontTotalChar;
}

/** \brief Get total fonts.

    Return the total number of fonts loaded into the edOLED's flash memory.
*/
unsigned char edOLED::getTotalFonts(void)
{
	return TOTALFONTS;
}

/** \brief Get font type.

    Return the font type number of the current font.
*/
unsigned char edOLED::getFontType(void)
{
	return fontType;
}

/** \brief Set font type.

    Set the current font type number, ie changing to different fonts base on the type provided.
*/
unsigned char edOLED::setFontType(unsigned char type)
{
	if ((type>=TOTALFONTS) || (type<0))
		return false;

	fontType=type;
	fontWidth=pgm_read_byte(fontsPointer[fontType]+0);
	fontHeight=pgm_read_byte(fontsPointer[fontType]+1);
	fontStartChar=pgm_read_byte(fontsPointer[fontType]+2);
	fontTotalChar=pgm_read_byte(fontsPointer[fontType]+3);
	fontMapWidth=(pgm_read_byte(fontsPointer[fontType]+4)*100)+pgm_read_byte(fontsPointer[fontType]+5); // two unsigned chars values into integer 16
	return true;
}

/** \brief Set color.

    Set the current draw's color. Only WHITE and BLACK available.
*/
void edOLED::setColor(unsigned char color)
{
	foreColor=color;
}

/** \brief Set draw mode.

    Set current draw mode with NORM or XOR.
*/
void edOLED::setDrawMode(unsigned char mode)
{
	drawMode=mode;
}

/** \brief Draw character.

    Draw character c using current color and current draw mode at x,y.
*/
void  edOLED::drawChar(unsigned char x, unsigned char y, unsigned char c)
{
	drawChar(x,y,c,foreColor,drawMode);
}

/** \brief Draw character with color and mode.

    Draw character c using color and draw mode at x,y.
*/
void  edOLED::drawChar(unsigned char x, unsigned char y, unsigned char c, unsigned char color, unsigned char mode)
{
	unsigned char rowsToDraw,row, tempC;
	unsigned char i,j,temp;
	unsigned int charPerBitmapRow,charColPositionOnBitmap,charRowPositionOnBitmap,charBitmapStartPosition;

	if ((c<fontStartChar) || (c>(fontStartChar+fontTotalChar-1)))		// no bitmap for the required c
	return;

	tempC=c-fontStartChar;

	// each row (in datasheet is call page) is 8 bits high, 16 bit high character will have 2 rows to be drawn
	rowsToDraw=fontHeight/8;	// 8 is LCD's page size, see SSD1306 datasheet
	if (rowsToDraw<=1) rowsToDraw=1;

	// the following draw function can draw anywhere on the screen, but SLOW pixel by pixel draw
	if (rowsToDraw==1)
	{
		for  (i=0;i<fontWidth+1;i++)
		{
			if (i==fontWidth) // this is done in a weird way because for 5x7 font, there is no margin, this code add a margin after col 5
				temp=0;
			else
				temp=pgm_read_byte(fontsPointer[fontType]+FONTHEADERSIZE+(tempC*fontWidth)+i);

			for (j=0;j<8;j++)
			{			// 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1)
				{
					pixel(x+i, y+j, color,mode);
				}
				else
				{
					pixel(x+i, y+j, !color,mode);
				}
				
				temp >>=1;
			}
		}
		return;
	}

	// font height over 8 bit
	// take character "0" ASCII 48 as example
	charPerBitmapRow=fontMapWidth/fontWidth;  // 256/8 =32 char per row
	charColPositionOnBitmap=tempC % charPerBitmapRow;  // =16
	charRowPositionOnBitmap=int(tempC/charPerBitmapRow); // =1
	charBitmapStartPosition=(charRowPositionOnBitmap * fontMapWidth * (fontHeight/8)) + (charColPositionOnBitmap * fontWidth) ;

	// each row on LCD is 8 bit height (see datasheet for explanation)
	for(row=0;row<rowsToDraw;row++)
	{
		for (i=0; i<fontWidth;i++)
		{
			temp=pgm_read_byte(fontsPointer[fontType]+FONTHEADERSIZE+(charBitmapStartPosition+i+(row*fontMapWidth)));
			for (j=0;j<8;j++)
			{			// 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1)
				{
					pixel(x+i,y+j+(row*8), color, mode);
				}
				else
				{
					pixel(x+i,y+j+(row*8), !color, mode);
				}
				temp >>=1;
			}
		}
	}

}

/** \brief Stop scrolling.

    Stop the scrolling of graphics on the OLED.
*/
void edOLED::scrollStop(void)
{
	command(DEACTIVATESCROLL);
}

/** \brief Right scrolling.

    Set row start to row stop on the OLED to scroll right. Refer to http://learn.edOLED.io/intro/general-overview-of-edOLED.html for explanation of the rows.
*/
void edOLED::scrollRight(unsigned char start, unsigned char stop)
{
	if (stop<start)		// stop must be larger or equal to start
		return;
	scrollStop();		// need to disable scrolling before starting to avoid memory corrupt
	command(RIGHTHORIZONTALSCROLL);
	command(0x00);
	command(start);
	command(0x7);		// scroll speed frames
	command(stop);
	command(0x00);
	command(0xFF);
	command(ACTIVATESCROLL);
}
	
/** \brief Vertical flip.

    Flip the graphics on the OLED vertically.
*/
void edOLED::flipVertical(unsigned char flip)
{
	if (flip)
	{
		command(COMSCANINC);
	}
	else
	{
		command(COMSCANDEC);
	}
}

/** \brief Horizontal flip.

    Flip the graphics on the OLED horizontally.
*/	
void edOLED::flipHorizontal(unsigned char flip)
{
	if (flip)
	{
		command(SEGREMAP | 0x0);
	}
	else
	{
		command(SEGREMAP | 0x1);
	}
}

void edOLED::spiSetup()
{
	/*
	// Bit-bang method:
	MOSI_PIN.pinMode(OUTPUT);	//pinMode(MOSI, OUTPUT_FAST);
	SCLK_PIN.pinMode(OUTPUT);	//pinMode(SCK, OUTPUT_FAST);
	CS_PIN.pinMode(OUTPUT);		//pinMode(csPin, OUTPUT_FAST);
	CS_PIN.pinWrite(HIGH);		//digitalWrite(csPin, HIGH);
	*/
}

void edOLED::spiTransfer(unsigned char data)
{
	// SPI library method:
	//CS_PIN.pinWrite(LOW);	//digitalWrite(csPin, LOW);
	oledSPI.transferData(&data);	//, NULL, 1, true);
	//CS_PIN.pinWrite(HIGH);	//digitalWrite(csPin, HIGH);

	/*
	// Bit-bang method (working, but slow)
	CS_PIN.pinWrite(LOW);	//digitalWrite(csPin, LOW);
	SCLK_PIN.pinWrite(HIGH);	//digitalWrite(SCK, HIGH);

	for (int i=7; i>=0; i--)
	{
		if (data & (1<<i))
			MOSI_PIN.pinWrite(HIGH);	//digitalWrite(MOSI, HIGH);
		else
			MOSI_PIN.pinWrite(LOW);		//digitalWrite(MOSI, LOW);
		
		delayMicroseconds(1);
		SCLK_PIN.pinWrite(LOW);			//digitalWrite(SCK, LOW);
		delayMicroseconds(1);
		SCLK_PIN.pinWrite(HIGH);		//digitalWrite(SCK, HIGH);
		delayMicroseconds(1);
	}

	SCLK_PIN.pinWrite(HIGH);			//digitalWrite(SCK, HIGH);
	CS_PIN.pinWrite(HIGH);	//digitalWrite(csPin, HIGH);
	*/
}
