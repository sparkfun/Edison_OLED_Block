/******************************************************************************
	Edison_OLED.h
	Header file for the Edison OLED Block C++ Library

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

#ifndef EDISON_OLED_H
#define EDISON_OLED_H

#define swapOLED(a, b) { unsigned char t = a; a = b; b = t; }

#define BLACK 0
#define WHITE 1

#define LCDWIDTH			64
#define LCDHEIGHT			48
#define FONTHEADERSIZE		6

#define NORM				0
#define XOR					1

#define PAGE				0
#define ALL					1

#define SETCONTRAST 		0x81
#define DISPLAYALLONRESUME 	0xA4
#define DISPLAYALLON 		0xA5
#define NORMALDISPLAY 		0xA6
#define INVERTDISPLAY 		0xA7
#define DISPLAYOFF 			0xAE
#define DISPLAYON 			0xAF
#define SETDISPLAYOFFSET 	0xD3
#define SETCOMPINS 			0xDA
#define SETVCOMDESELECT		0xDB
#define SETDISPLAYCLOCKDIV 	0xD5
#define SETPRECHARGE 		0xD9
#define SETMULTIPLEX 		0xA8
#define SETLOWCOLUMN 		0x00
#define SETHIGHCOLUMN 		0x10
#define SETSTARTLINE 		0x40
#define MEMORYMODE 			0x20
#define COMSCANINC 			0xC0
#define COMSCANDEC 			0xC8
#define SEGREMAP 			0xA0
#define CHARGEPUMP 			0x8D
#define EXTERNALVCC 		0x01
#define SWITCHCAPVCC 		0x02

// Scroll
#define ACTIVATESCROLL 					0x2F
#define DEACTIVATESCROLL 				0x2E
#define SETVERTICALSCROLLAREA 			0xA3
#define RIGHTHORIZONTALSCROLL 			0x26
#define LEFT_HORIZONTALSCROLL 			0x27
#define VERTICALRIGHTHORIZONTALSCROLL	0x29
#define VERTICALLEFTHORIZONTALSCROLL	0x2A

typedef enum CMD {
	CMD_CLEAR,			//0
	CMD_INVERT,			//1
	CMD_CONTRAST,		//2
	CMD_DISPLAY,		//3
	CMD_SETCURSOR,		//4
	CMD_PIXEL,			//5
	CMD_LINE,			//6
	CMD_LINEH,			//7
	CMD_LINEV,			//8
	CMD_RECT,			//9
	CMD_RECTFILL,		//10
	CMD_CIRCLE,			//11
	CMD_CIRCLEFILL,		//12
	CMD_DRAWCHAR,		//13
	CMD_DRAWBITMAP,		//14
	CMD_GETLCDWIDTH,	//15
	CMD_GETLCDHEIGHT,	//16
	CMD_SETCOLOR,		//17
	CMD_SETDRAWMODE		//18
} commCommand_t;

class edOLED {
public:
	edOLED();
	
	void begin(void);

	unsigned char write(unsigned char);
	void print(const char * c);
	void print(int d);
	void print(float f);

	// RAW LCD functions
	void command(unsigned char c);
	void data(unsigned char c);
	void setColumnAddress(unsigned char add);
	void setPageAddress(unsigned char add);
	
	// LCD Draw functions
	void clear(unsigned char mode);
	void clear(unsigned char mode, unsigned char c);
	void invert(unsigned char inv);
	void contrast(unsigned char contrast);
	void display(void);
	void setCursor(unsigned char x, unsigned char y);
	void pixel(unsigned char x, unsigned char y);
	void pixel(unsigned char x, unsigned char y, unsigned char color, unsigned char mode);
	void line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
	void line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char color, unsigned char mode);
	void lineH(unsigned char x, unsigned char y, unsigned char width);
	void lineH(unsigned char x, unsigned char y, unsigned char width, unsigned char color, unsigned char mode);
	void lineV(unsigned char x, unsigned char y, unsigned char height);
	void lineV(unsigned char x, unsigned char y, unsigned char height, unsigned char color, unsigned char mode);
	void rect(unsigned char x, unsigned char y, unsigned char width, unsigned char height);
	void rect(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char color , unsigned char mode);
	void rectFill(unsigned char x, unsigned char y, unsigned char width, unsigned char height);
	void rectFill(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char color , unsigned char mode);
	void circle(unsigned char x, unsigned char y, unsigned char radius);
	void circle(unsigned char x, unsigned char y, unsigned char radius, unsigned char color, unsigned char mode);
	void circleFill(unsigned char x0, unsigned char y0, unsigned char radius);
	void circleFill(unsigned char x0, unsigned char y0, unsigned char radius, unsigned char color, unsigned char mode);
	void drawChar(unsigned char x, unsigned char y, unsigned char c);
	void drawChar(unsigned char x, unsigned char y, unsigned char c, unsigned char color, unsigned char mode);
	void drawBitmap(void);
	unsigned char getLCDWidth(void);
	unsigned char getLCDHeight(void);
	void setColor(unsigned char color);
	void setDrawMode(unsigned char mode);

	// Font functions
	unsigned char getFontWidth(void);
	unsigned char getFontHeight(void);
	unsigned char getTotalFonts(void);
	unsigned char getFontType(void);
	unsigned char setFontType(unsigned char type);
	unsigned char getFontStartChar(void);
	unsigned char getFontTotalChar(void);

	// LCD Rotate Scroll functions	
	void scrollRight(unsigned char start, unsigned char stop);
	void scrollLeft(unsigned char start, unsigned char stop);
	void scrollVertRight(unsigned char start, unsigned char stop);
	void scrollVertLeft(unsigned char start, unsigned char stop);
	void scrollStop(void);
	void flipVertical(unsigned char flip);
	void flipHorizontal(unsigned char flip);
	
	//void doCmd(unsigned char index);
	
private:
	unsigned char foreColor,drawMode,fontWidth, fontHeight, fontType, fontStartChar, fontTotalChar, cursorX, cursorY;
	unsigned int fontMapWidth;
	static const unsigned char *fontsPointer[];
					  
	// Communication
	void spiTransfer(unsigned char data);
	void spiSetup();
};
#endif
