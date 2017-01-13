/******************************************************************************
MIT License

Copyright (c) 2017 Brian Feldman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include "../edison_node.hpp"
#include "edison_oled_wrap.hpp"
#include "Edison_OLED.h"
#include <node.h>

using namespace v8;

Nan::Persistent<v8::Function> EdisonOledWrap::constructor;

const char* ToCString(const String::Utf8Value& value) {
	return *value ? *value : "";
}

EdisonOledWrap::EdisonOledWrap() : m_oled() {
}

EdisonOledWrap::~EdisonOledWrap() {
}

void EdisonOledWrap::Init(v8::Local<v8::Object> exports) {
	Nan::HandleScope scope;

	// Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Oled").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "begin", begin);

	Nan::SetPrototypeMethod(tpl, "write", write);
	Nan::SetPrototypeMethod(tpl, "print", print);
	
	Nan::SetPrototypeMethod(tpl, "command", command);
	Nan::SetPrototypeMethod(tpl, "data", data);
	Nan::SetPrototypeMethod(tpl, "setColumnAddress", setColumnAddress);
	Nan::SetPrototypeMethod(tpl, "setPageAddress", setPageAddress);
	
	Nan::SetPrototypeMethod(tpl, "clear", clear);
	Nan::SetPrototypeMethod(tpl, "invert", invert);
	Nan::SetPrototypeMethod(tpl, "contrast", contrast);
	Nan::SetPrototypeMethod(tpl, "display", display);
	Nan::SetPrototypeMethod(tpl, "setCursor", setCursor);

	Nan::SetPrototypeMethod(tpl, "pixel", pixel);
	Nan::SetPrototypeMethod(tpl, "line", line);
	Nan::SetPrototypeMethod(tpl, "lineH", lineH);
	Nan::SetPrototypeMethod(tpl, "lineV", lineV);
	Nan::SetPrototypeMethod(tpl, "rect", rect);
	Nan::SetPrototypeMethod(tpl, "rectFill", rectFill);
	Nan::SetPrototypeMethod(tpl, "circle", circle);
	Nan::SetPrototypeMethod(tpl, "circleFill", circleFill);
	Nan::SetPrototypeMethod(tpl, "drawChar", drawChar);

	Nan::SetPrototypeMethod(tpl, "drawBitmap", drawBitmap);

	Nan::SetPrototypeMethod(tpl, "getLCDWidth", getLCDWidth);
	Nan::SetPrototypeMethod(tpl, "getLCDHeight", getLCDHeight);
	Nan::SetPrototypeMethod(tpl, "setColor", setColor);
	Nan::SetPrototypeMethod(tpl, "setDrawMode", setDrawMode);

	Nan::SetPrototypeMethod(tpl, "getFontWidth", getFontWidth);
	Nan::SetPrototypeMethod(tpl, "getFontHeight", getFontHeight);
	Nan::SetPrototypeMethod(tpl, "getTotalFonts", getTotalFonts);
	Nan::SetPrototypeMethod(tpl, "getFontType", getFontType);	
	Nan::SetPrototypeMethod(tpl, "setFontType", setFontType);
	Nan::SetPrototypeMethod(tpl, "getFontStartChar", getFontStartChar);
	Nan::SetPrototypeMethod(tpl, "getFontTotalChar", getFontTotalChar);	
		
	/*
	these exist in Edison_OLED.h but not in cpp file.		
	Nan::SetPrototypeMethod(tpl, "scrollRight", scrollRight);
	Nan::SetPrototypeMethod(tpl, "scrollLeft", scrollLeft);
	Nan::SetPrototypeMethod(tpl, "scrollVertRight", scrollVertRight);
	Nan::SetPrototypeMethod(tpl, "scrollVertLeft", scrollVertLeft);
	*/
	Nan::SetPrototypeMethod(tpl, "scrollStop", scrollStop);
	Nan::SetPrototypeMethod(tpl, "flipVertical", flipVertical);
	Nan::SetPrototypeMethod(tpl, "flipHorizontal", flipHorizontal);

	/* constants */
	NODE_DEFINE_CONSTANT(exports, BLACK); // 0
	NODE_DEFINE_CONSTANT(exports, WHITE); // 1

	NODE_DEFINE_CONSTANT(exports, LCDWIDTH); //			64
	NODE_DEFINE_CONSTANT(exports, LCDHEIGHT); //			48
	NODE_DEFINE_CONSTANT(exports, FONTHEADERSIZE); //		6

	NODE_DEFINE_CONSTANT(exports, NORM); //				0
	NODE_DEFINE_CONSTANT(exports, XOR); //					1

	NODE_DEFINE_CONSTANT(exports, PAGE); //				0
	NODE_DEFINE_CONSTANT(exports, ALL); //					1

	NODE_DEFINE_CONSTANT(exports, SETCONTRAST); // 		0x81
	NODE_DEFINE_CONSTANT(exports, DISPLAYALLONRESUME); // 	0xA4
	NODE_DEFINE_CONSTANT(exports, DISPLAYALLON); // 		0xA5
	NODE_DEFINE_CONSTANT(exports, NORMALDISPLAY); // 		0xA6
	NODE_DEFINE_CONSTANT(exports, INVERTDISPLAY); // 		0xA7
	NODE_DEFINE_CONSTANT(exports, DISPLAYOFF); // 			0xAE
	NODE_DEFINE_CONSTANT(exports, DISPLAYON); // 			0xAF
	NODE_DEFINE_CONSTANT(exports, SETDISPLAYOFFSET); // 	0xD3
	NODE_DEFINE_CONSTANT(exports, SETCOMPINS); // 			0xDA
	NODE_DEFINE_CONSTANT(exports, SETVCOMDESELECT); //		0xDB
	NODE_DEFINE_CONSTANT(exports, SETDISPLAYCLOCKDIV); // 	0xD5
	NODE_DEFINE_CONSTANT(exports, SETPRECHARGE); // 		0xD9
	NODE_DEFINE_CONSTANT(exports, SETMULTIPLEX); // 		0xA8
	NODE_DEFINE_CONSTANT(exports, SETLOWCOLUMN); // 		0x00
	NODE_DEFINE_CONSTANT(exports, SETHIGHCOLUMN); // 		0x10
	NODE_DEFINE_CONSTANT(exports, SETSTARTLINE); // 		0x40
	NODE_DEFINE_CONSTANT(exports, MEMORYMODE); // 			0x20
	NODE_DEFINE_CONSTANT(exports, COMSCANINC); // 			0xC0
	NODE_DEFINE_CONSTANT(exports, COMSCANDEC); // 			0xC8
	NODE_DEFINE_CONSTANT(exports, SEGREMAP); // 			0xA0
	NODE_DEFINE_CONSTANT(exports, CHARGEPUMP); // 			0x8D
	NODE_DEFINE_CONSTANT(exports, EXTERNALVCC); // 		0x01
	NODE_DEFINE_CONSTANT(exports, SWITCHCAPVCC); // 		0x02

// Scroll
	NODE_DEFINE_CONSTANT(exports, ACTIVATESCROLL); // 					0x2F
	NODE_DEFINE_CONSTANT(exports, DEACTIVATESCROLL); // 				0x2E
	NODE_DEFINE_CONSTANT(exports, SETVERTICALSCROLLAREA); // 			0xA3
	NODE_DEFINE_CONSTANT(exports, RIGHTHORIZONTALSCROLL); // 			0x26
	NODE_DEFINE_CONSTANT(exports, LEFT_HORIZONTALSCROLL); // 			0x27
	NODE_DEFINE_CONSTANT(exports, VERTICALRIGHTHORIZONTALSCROLL); //	0x29
	NODE_DEFINE_CONSTANT(exports, VERTICALLEFTHORIZONTALSCROLL); //	0x2A	
	
	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Oled").ToLocalChecked(), tpl->GetFunction());
}

void EdisonOledWrap::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall()) {
    // Invoked as constructor: `new EdisonOledWrap(...)`
    EdisonOledWrap* obj = new EdisonOledWrap();
	edOLED *oled = new edOLED();
	obj->m_oled = oled;
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `EdisonOledWrap(...)`, turn into construct call.
    //v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    //args.GetReturnValue().Set(cons->NewInstance());	
	Nan::ThrowTypeError(ERROR_CREATE_INSTANCE_WITH_NEW);
  }
}

void EdisonOledWrap::begin(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->begin();
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::write(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	
	unsigned char out = oled->write(args[0]->Uint32Value() & 0xFF);
	args.GetReturnValue().Set(Nan::New<Integer>(out));	
}

void EdisonOledWrap::print(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	String::Utf8Value str(args[0]);
	oled->print(ToCString(str));
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::command(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->command(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::data(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->data(args[0]->Uint32Value() & 0xFF);	
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::setColumnAddress(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->setColumnAddress(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::setPageAddress(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->setPageAddress(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::clear(const Nan::FunctionCallbackInfo<v8::Value>& args){
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 0 : 
			oled->clear(0x00);
			break;
		case 1 : 
			if (!args[0]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->clear(args[0]->Uint32Value() & 0xFF);
			break;
		case 2 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->clear(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF);
			break;
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;
			break;
	}
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::invert(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->invert(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}		

void EdisonOledWrap::contrast(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->contrast(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::display(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->display();
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::setCursor(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 2) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());		
	edOLED* oled = wrap->m_oled;
	oled->setCursor(args[0]->Uint32Value() & 0xFF,args[1]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::pixel(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 2 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->pixel(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF);
			break;
		case 4 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->pixel(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF);
			break;
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::line(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 4 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->line(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF);
			break;
		case 6 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->line(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF, args[5]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::lineH(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 3 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->lineH(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF);
			break;
		case 5 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->lineH(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::lineV(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 3 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->lineV(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF);
			break;
		case 5 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->lineV(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::rect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 4 : 		
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->rect(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF);
			break;
		case 6 : 		
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->rect(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF, args[5]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::rectFill(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 4 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->rectFill(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF);
			break;
		case 6 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->rectFill(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF, args[5]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::circle(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 3 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->circle(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF);
			break;
		case 5 : 	
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->circle(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::circleFill(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 3 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->circleFill(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF);
			break;
		case 5 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			oled->circleFill(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::drawChar(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	switch (args.Length()) {
		case 3 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->drawChar(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF);
			break;
		case 5 : 	
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}
			oled->drawChar(args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value() & 0xFF, args[2]->Uint32Value() & 0xFF, args[3]->Uint32Value() & 0xFF, args[4]->Uint32Value() & 0xFF);
			break;			
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;		
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::drawBitmap(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	//this is not implemented in the wrapped object, so throw an error
	throw "Not implemented";
}

void EdisonOledWrap::getLCDWidth(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getLCDWidth();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::getLCDHeight(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getLCDHeight();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));	
}

void EdisonOledWrap::setColor(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->setColor(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::setDrawMode(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->setDrawMode(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::getFontWidth(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getFontWidth();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::getFontHeight(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getFontHeight();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::getTotalFonts(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getTotalFonts();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::getFontType(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getFontType();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::setFontType(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->setFontType(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::getFontStartChar(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getFontStartChar();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

void EdisonOledWrap::getFontTotalChar(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	unsigned char out = oled->getFontTotalChar();
	//args.GetReturnValue().Set(Nan::New<Integer>(out).ToLocalChecked());
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}

/*
These exist in Edison_OLED.h but not in CPP file.
void EdisonOledWrap::scrollRight(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() == 2) {
		EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());		
		edOLED* oled = wrap->m_oled;
		oled->scrollRight(args[0]->Uint32Value() & 0xFF,args[1]->Uint32Value() & 0xFF);
	}	
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::scrollLeft(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() == 2) {
		EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());		
		edOLED* oled = wrap->m_oled;
		oled->scrollLeft(args[0]->Uint32Value() & 0xFF,args[1]->Uint32Value() & 0xFF);
	}	
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::scrollVertRight(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() == 2) {
		EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());		
		edOLED* oled = wrap->m_oled;
		oled->scrollVertRight(args[0]->Uint32Value() & 0xFF,args[1]->Uint32Value() & 0xFF);
	}	
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::scrollVertLeft(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() == 2) {
		EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());		
		edOLED* oled = wrap->m_oled;
		oled->scrollVertLeft(args[0]->Uint32Value() & 0xFF,args[1]->Uint32Value() & 0xFF);
	}	
    args.GetReturnValue().SetUndefined();
}
*/

void EdisonOledWrap::scrollStop(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->scrollStop();
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::flipVertical(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->flipVertical(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}

void EdisonOledWrap::flipHorizontal(const Nan::FunctionCallbackInfo<v8::Value>& args){
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonOledWrap* wrap = ObjectWrap::Unwrap<EdisonOledWrap>(args.Holder());
	edOLED* oled = wrap->m_oled;
	oled->flipHorizontal(args[0]->Uint32Value() & 0xFF);
    args.GetReturnValue().SetUndefined();
}