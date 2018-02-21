#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "oled/Edison_OLED.h"
#include "gpio/gpio.h"

// Define an edOLED object:
edOLED oled;

// Pin definitions:
// All buttons have pull-up resistors on-board, so just declare
// them as regular INPUT's
gpio BUTTON_UP(47, INPUT);
gpio BUTTON_DOWN(44, INPUT);
gpio BUTTON_LEFT(165, INPUT);
gpio BUTTON_RIGHT(45, INPUT);
gpio BUTTON_SELECT(48, INPUT);
gpio BUTTON_A(49, INPUT);
gpio BUTTON_B(46, INPUT);

#define FIELD_SIZE 95

char field[FIELD_SIZE];

void init_field() {
	int i = 0;
	for (char c = 'a'; c <= 'z'; c++)
		field[i++] = c;
	for (char c = '0'; c <= '9'; c++)
		field[i++] = c;
	for (char c = 'A'; c <= 'Z'; c++)
		field[i++] = c;
	for (char c = 32; c <=47; c++)
		field[i++] = c;
	for (char c = 58; c <= 64; c++)
		field[i++] = c;
	for (char c = 91; c <= 96; c++)
		field[i++] = c;
	for (char c = 123; c <= 126; c++)
		field[i++] = c;
}


void draw_field(int n, int row) {
	int ul = 9 * row, lr = 9 * (row + 4) + 9 - 1;

	oled.setColor(BLACK);
	oled.rectFill(0, 8, 64, 40);

	oled.setColor(WHITE);

	char buf[] = {0,0};
	for (int i = ul; i <= lr && i < FIELD_SIZE; i++) {
		int x = 7 * (i % 9);
		int y = 8 + 8 * ((i / 9) - row);
		if (i == n) {
			oled.rectFill(x, y, 7, 8);
			oled.setColor(BLACK);
		}
		oled.setCursor(x+1, y);
		buf[0] = field[i];
		oled.print(buf);
		if (i == n)
			oled.setColor(WHITE);
	}
}

int main(int argc, char **argv) {
	init_field();

	oled.begin();

	int x = 0, y = 0;
	int offset = 0;

	draw_field(15, 0);

	oled.display();

}
