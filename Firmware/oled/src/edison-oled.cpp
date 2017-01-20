//============================================================================
// Name        : edison-oled.cpp
// Author      : Robin Kirkman
// Version     :
// Copyright   : GPL
// Description : Hello World in C, Ansi-style
//============================================================================

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

void eval(char ***cmd);

int main(int argc, char **argv) {
	char **cmd = (char**) malloc(sizeof(char*) * argc);
	memcpy(cmd, argv+1, sizeof(char*) * (argc - 1));
	cmd[argc-1] = 0;

	while(*cmd)
		eval(&cmd);
}

#define NEXT(c) (*((*c)++))
#define INEXT(c) (atoi(NEXT(c)))

int buttons_value(void);

void eval(char ***cmd) {
	char *c = NEXT(cmd);
	if(!strcmp("begin", c))
		oled.begin();
	else if(!strcmp("print", c))
		oled.print(NEXT(cmd));
	else if(!strcmp("clear", c))
		oled.clear(PAGE);
	else if(!strcmp("invert", c))
		oled.invert(INEXT(cmd));
	else if(!strcmp("display", c))
		oled.display();
	else if(!strcmp("setCursor", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		oled.setCursor(x, y);
	} else if(!strcmp("pixel", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		oled.pixel(x, y);
	} else if(!strcmp("line", c)) {
		int x0 = INEXT(cmd);
		int y0 = INEXT(cmd);
		int x1 = INEXT(cmd);
		int y1 = INEXT(cmd);
		oled.line(x0, y0, x1, y1);
	} else if(!strcmp("rect", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		int width = INEXT(cmd);
		int height = INEXT(cmd);
		oled.rect(x, y, width, height);
	} else if(!strcmp("rectFill", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		int width = INEXT(cmd);
		int height = INEXT(cmd);
		oled.rectFill(x, y, width, height);
	} else if(!strcmp("circle", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		int radius = INEXT(cmd);
		oled.circle(x, y, radius);
	} else if(!strcmp("circleFill", c)) {
		int x = INEXT(cmd);
		int y = INEXT(cmd);
		int radius = INEXT(cmd);
		oled.circleFill(x, y, radius);
	} else if(!strcmp("setColor", c))
		oled.setColor(INEXT(cmd));
	else if(!strcmp("setDrawMode", c))
		oled.setDrawMode(INEXT(cmd));
	else if(!strcmp("buttons", c)) {
		if(!BUTTON_UP.pinRead())
			printf("U ");
		if(!BUTTON_DOWN.pinRead())
			printf("D ");
		if(!BUTTON_LEFT.pinRead())
			printf("L ");
		if(!BUTTON_RIGHT.pinRead())
			printf("R ");
		if(!BUTTON_SELECT.pinRead())
			printf("S ");
		if(!BUTTON_A.pinRead())
			printf("A ");
		if(!BUTTON_B.pinRead())
			printf("B ");
		printf("\n");
	} else if(!strcmp("wait", c)) {
		for(int bv = buttons_value(); bv == buttons_value();)
			usleep(1000);
	} else if(!strcmp("displayoff", c)) {
		oled.command(DISPLAYOFF);
	} else if(!strcmp("displayon", c)) {
		oled.command(DISPLAYON);
	} else if(!strcmp("save", c)) {
		int fd = creat(NEXT(cmd), S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
		if(fd != -1) {
			write(fd, oled.screenmemory, 384);
			close(fd);
		}
	} else if(!strcmp("load", c)) {
		int fd = open(NEXT(cmd), O_RDONLY);
		if(fd != -1) {
			read(fd, oled.screenmemory, 384);
			close(fd);
		}
	}
}

int buttons_value(void) {
	int v = 0;
	v |= BUTTON_UP.pinRead() << 0;
	v |= BUTTON_DOWN.pinRead() << 1;
	v |= BUTTON_LEFT.pinRead() << 2;
	v |= BUTTON_RIGHT.pinRead() << 3;
	v |= BUTTON_SELECT.pinRead() << 4;
	v |= BUTTON_A.pinRead() << 5;
	v |= BUTTON_B.pinRead() << 6;
	return v;

}
