#ifndef EDISON_OLED_WRAP_H
#define EDISON_OLED_WRAP_H

#include <nan.h>

#include "../edison_node.hpp"
#include "Edison_OLED.h"

class EdisonOledWrap : public Nan::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);
		static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
	
		EdisonOledWrap();
	private:
		~EdisonOledWrap();
		
		static void begin(const Nan::FunctionCallbackInfo<v8::Value>& args);

		static void write(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void print(const Nan::FunctionCallbackInfo<v8::Value>& args);
		
		// RAW LCD functions
		static void command(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void data(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void setColumnAddress(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void setPageAddress(const Nan::FunctionCallbackInfo<v8::Value>& args);		

		// LCD Draw functions
		static void clear(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void invert(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void contrast(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void display(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void setCursor(const Nan::FunctionCallbackInfo<v8::Value>& args);

		static void pixel(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void line(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void lineH(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void lineV(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void rect(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void rectFill(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void circle(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void circleFill(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void drawChar(const Nan::FunctionCallbackInfo<v8::Value>& args);

		static void drawBitmap(const Nan::FunctionCallbackInfo<v8::Value>& args);

		static void getLCDWidth(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getLCDHeight(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void setColor(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void setDrawMode(const Nan::FunctionCallbackInfo<v8::Value>& args);

		// Font functions
		static void getFontWidth(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getFontHeight(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getTotalFonts(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getFontType(const Nan::FunctionCallbackInfo<v8::Value>& args);		
		static void setFontType(const Nan::FunctionCallbackInfo<v8::Value>& args);		
		static void getFontStartChar(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getFontTotalChar(const Nan::FunctionCallbackInfo<v8::Value>& args);		

		// LCD Rotate Scroll functions	
		/*
		these exist in Edison_OLED.h but not in cpp file.		
		static void scrollRight(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void scrollLeft(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void scrollVertRight(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void scrollVertLeft(const Nan::FunctionCallbackInfo<v8::Value>& args);
		*/
		static void scrollStop(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void flipVertical(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void flipHorizontal(const Nan::FunctionCallbackInfo<v8::Value>& args);

		edOLED* m_oled;
		static Nan::Persistent<v8::Function> constructor;
};

#endif