'use strict';
var edison = require('../index.js');

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
};

var clearScreen = function() {
	console.log('--clearing screen');
	oled.clear(1);
	oled.display();
};

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
sleep(sleepTime);
buttonTest();
sleep(sleepTime);

cleanUp();
	
console.log('All done.');
