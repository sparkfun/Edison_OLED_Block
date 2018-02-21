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

#define UP_MASK 1
#define DOWN_MASK 2
#define LEFT_MASK 4
#define RIGHT_MASK 8
#define SELECT_MASK 16
#define A_MASK 32
#define B_MASK 64

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
	int x, y;

	oled.setColor(BLACK);
	oled.rectFill(0, 8, 64, 40);

	oled.setColor(WHITE);

	char buf[] = {0,0};
	for (int i = ul; i <= lr && i < FIELD_SIZE; i++) {
		if (i == n)
			continue;
		x = 7 * (i % 9);
		y = 8 + 8 * ((i / 9) - row);
		if (i == n) {
			oled.rectFill(x, y, 7, 9);
			oled.setColor(BLACK);
		}
		oled.setCursor(x+1, y);
		buf[0] = field[i];
		oled.print(buf);
		if (i == n)
			oled.setColor(WHITE);
	}
	x = 7 * (n % 9);
	y = 8 + 8 * ((n / 9) - row);
	oled.rectFill(x, y, 7, 9);
	oled.setColor(BLACK);
	oled.setCursor(x+1, y);
	buf[0] = field[n];
	oled.print(buf);
	oled.setColor(WHITE);
	oled.display();
}

int pressed() {
	int mask = 0;
	if (BUTTON_UP.pinRead() == LOW)
		mask |= UP_MASK;
	if (BUTTON_DOWN.pinRead() == LOW)
		mask |= DOWN_MASK;
	if (BUTTON_LEFT.pinRead() == LOW)
		mask |= LEFT_MASK;
	if (BUTTON_RIGHT.pinRead() == LOW)
		mask |= RIGHT_MASK;
	if (BUTTON_SELECT.pinRead() == LOW)
		mask |= SELECT_MASK;
	if (BUTTON_A.pinRead() == LOW)
		mask |= A_MASK;
	if (BUTTON_B.pinRead() == LOW)
		mask |= B_MASK;
	return mask;
}

int main(int argc, char **argv) {
	init_field();

	oled.begin();

	int n = 0;
	int row = 0;

	draw_field(n, row);

	for (int p = pressed(); p != SELECT_MASK; p = pressed()) {
		switch(p) {
		case UP_MASK:
			if (n >= 9)
				n -= 9;
			break;
		case DOWN_MASK:
			if (n < FIELD_SIZE - 9)
				n += 9;
			break;
		case LEFT_MASK:
			if (n >= 1)
				n -= 1;
			break;
		case RIGHT_MASK:
			if (n < FIELD_SIZE - 1)
				n += 1;
			break;
		default:
			usleep(100);
			continue;
		}
		if (row > n / 9)
			row -= 1;
		if (row + 4 < n/9)
			row += 1;
		draw_field(n, row);
		while (p == pressed())
			usleep(100);
	}
}
