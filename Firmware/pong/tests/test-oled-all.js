'use strict';
var edison = require('../index.js');

/*
screen is 64 X 48
*/
var lcdWidth = edison.LCDWIDTH;
var lcdHeight = edison.LCDHEIGHT;

var oled = new edison.Oled();

var btnUp = new edison.Gpio(47, edison.INPUT);
var btnDown = new edison.Gpio(44, edison.INPUT);
var btnLeft = new edison.Gpio(165, edison.INPUT);
var btnRight = new edison.Gpio(45, edison.INPUT);
var btnSelect = new edison.Gpio(48, edison.INPUT);
var btnA = new edison.Gpio(49, edison.INPUT);
var btnB = new edison.Gpio(46, edison.INPUT);

var sleepTime = 1000;

var sleep = function(ms) {
    var now = new Date().getTime();
    while(new Date().getTime() < now + ms){ /* do nothing */ } 
};

var setupOled = function() {
	console.log('--setup screen');
	oled.begin();
	oled.clear(0);
	oled.display();
	oled.setFontType(0);	
	
	lcdWidth = oled.getLCDWidth();
	console.log('---width: ' + lcdWidth);
	lcdHeight = oled.getLCDHeight();
	console.log('---height: ' + lcdHeight);
	
};

var clearScreen = function() {
	console.log('--clearing screen');
	oled.clear(1);
	oled.display();
};

var startScreen = function() {
	console.log('--show start');
	
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
};

var drawRectangle = function() {
	console.log('--draw retangle');
	oled.clear();
	oled.rect(3, 3, 58, 45);
	oled.rectFill(15, 15, 34, 18);
	oled.display();		
}

var drawCircle = function() {
	console.log('--draw circle');
	oled.clear();
	oled.circle(32, 24, 20);
	oled.circleFill(32, 24, 10);
	oled.display();		
}

var drawLine = function() {
	console.log('--draw line');
	oled.clear();
	oled.line(0, 0, lcdWidth, lcdHeight);
	oled.line(lcdWidth, 0, 0, lcdHeight);
	
	oled.lineH(0, parseInt((lcdHeight/2), 10), lcdWidth);
	oled.lineV(parseInt((lcdWidth/2), 10), 0, lcdHeight);
	oled.display();		
}

var drawPixel = function() {
	console.log('--draw pixel');
	oled.clear();
	var x = 0;
	var y = 0;
	for (var i = 0; i <= 100; i++) {
		x = Math.floor(Math.random() * lcdWidth) + 1;  
		y = Math.floor(Math.random() * lcdHeight) + 1;  
		oled.pixel(x, y);
	}
	oled.display();		
}

var drawChar = function() {
	console.log('--draw char');
	oled.clear();
	var x = 0;
	var y = 0;
	var c = 0;
	for (var i = 0; i <= 5; i++) {
		x = Math.floor(Math.random() * 54) + 10;  
		//y = Math.floor(Math.random() * 40) + 5;  
		y = i * 8;
		c = Math.floor(Math.random() * 26) + 97;  
		oled.drawChar(x, y, c);
	}
	oled.display();		
}

var gameOverScreen = function() {
	console.log('--show game over');	
	
	oled.clear(0);
	oled.setCursor(14, 5);
	oled.print("Game");
	oled.setCursor(14, 13);
	oled.print("Over");
	oled.setCursor(14, 30);
	oled.print("Play");
	oled.setCursor(14, 38);
	oled.print("again?");
	oled.display();	
};

var invertScreen = function(inv) {
	console.log('--invert');
	oled.invert(inv);
}

var invertWithCommand = function(inv) {
	console.log('--test command - invert screen');
	if (inv == 1) {
		oled.command(0xA7);
	} else {
		oled.command(0xA6);
	}
}

var testData = function() {
	console.log('--test data , setPageAddress and setColumnAddress');
	oled.clear(0);
	for (var i=0;i<8; i++)
	{
		oled.setPageAddress(i);
		oled.setColumnAddress(0);
		for (var j=0; j<0x80; j++)
		{
			if (j % 2 == 0) {
				oled.data(0xFF);
			} else {
				oled.data(0x00);
			}
			
		}
	}	
}

var changeColor = function() {
	console.log('--change color');
	oled.clear(0, 0xFF);
	oled.setColor(edison.BLACK); //black
	oled.setCursor(2, 13);
	oled.print("Black Text");	
	oled.display();
	oled.setColor(edison.WHITE); //white
}

var changeMode = function() {
	console.log('--change mode');
	oled.clear(0, 0xFF);
	oled.setDrawMode(edison.XOR); //XOR
	oled.setCursor(4, 13);
	oled.print("XOR MODE");	
	oled.display();
	oled.setDrawMode(edison.NORM); //NORM
}

var changeFont = function() {
	console.log('--change font');
	console.log('---- total fonts: ' + oled.getTotalFonts());
	console.log('---- font type: ' + oled.getFontType());
	console.log('---- font width: ' + oled.getFontWidth());
	console.log('---- font height: ' + oled.getFontHeight());
	console.log('---- font start character: ' + oled.getFontStartChar());
	console.log('---- font total characters: ' + oled.getFontTotalChar());
	oled.clear(0);
	oled.setFontType(1); //font8x16
	oled.setCursor(4, 4);
	oled.print("Font 8X16");	
	oled.display();
	oled.setFontType(2); //sevensegment 
	oled.setCursor(4, 25);
	oled.print("7-S");	
	oled.display();	
	oled.setFontType(3); //fontlargenumber 
	oled.setCursor(44, 25);
	oled.print("LG");		
	oled.display();	
	oled.setFontType(0); //font 5X7
}

var flipScreen = function () {
	console.log('--flip screen & scroll stop');
	console.log('---calling scroll stop just to test it');
	oled.scrollStop();
	oled.clear(0);
	oled.setCursor(12, 4);
	oled.print("Flip Me");		
	oled.display();		
	oled.flipVertical(1);
	sleep(sleepTime);
	oled.flipHorizontal(1);
	sleep(sleepTime);		
	oled.flipVertical(0);
	sleep(sleepTime);
	oled.flipHorizontal(0);	
	
	oled.display();	
}

var buttonScreenInstructions = function(moveType, btnName) {
	oled.clear(0);
	oled.setCursor(4, 4);
	oled.print(moveType);		
	oled.setCursor(4, 16);
	oled.print(btnName);			
	oled.display();	
}

var buttonTest = function() {
	console.log('--buttons');

	console.log('press Button A:');
	buttonScreenInstructions("Press", "btn A");
	while (btnA.pinRead() == edison.HIGH) {}
	console.log('pressed A: ' + btnA.pinRead());
	console.log('Button A pressed.');
	
	console.log('press Button B:');
	buttonScreenInstructions("Press", "btn B");
	while (btnB.pinRead() == edison.HIGH) {}
	console.log('Button B pressed.');	
	
	console.log('press Select:');
	buttonScreenInstructions("Press", "stick");
	while (btnSelect.pinRead() == edison.HIGH) {}
	console.log('Button Select pressed.');		
	
	console.log('move joystick up:');
	buttonScreenInstructions("Move", "up");
	while (btnUp.pinRead() == edison.HIGH) {}
	console.log('joystick pressed up.');			
	
	console.log('move joystick down:');
	buttonScreenInstructions("Move", "down");
	while (btnDown.pinRead() == edison.HIGH) {}
	console.log('joystick pressed down.');	

	console.log('move joystick left:');
	buttonScreenInstructions("Move", "left");
	while (btnLeft.pinRead() == edison.HIGH) {}
	console.log('joystick pressed left.');	
	
	console.log('move joystick right:');
	buttonScreenInstructions("Move", "right");
	while (btnRight.pinRead() == edison.HIGH) {}
	console.log('joystick pressed right.');		
}

var cleanUp = function() {
	oled.clear(0);
	oled.display();
}

setupOled();

startScreen();
sleep(sleepTime);
flipScreen()
sleep(sleepTime);
drawRectangle();
sleep(sleepTime);
invertScreen(1);
sleep(sleepTime);
invertScreen(0);
sleep(sleepTime);
drawCircle();
sleep(sleepTime);
invertWithCommand(1);
sleep(sleepTime);
invertWithCommand(0);
sleep(sleepTime);
drawLine();
sleep(sleepTime);
drawPixel();
sleep(sleepTime);
drawChar();
sleep(sleepTime);
testData();
sleep(sleepTime);
changeColor();
sleep(sleepTime);
changeMode();
sleep(sleepTime);
changeFont();
sleep(sleepTime);
buttonTest();
sleep(sleepTime);
gameOverScreen();
sleep(sleepTime);

cleanUp();
	
console.log('All done.');
