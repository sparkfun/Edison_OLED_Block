#ifndef EDISON_GPIO_WRAP_H
#define EDISON_GPIO_WRAP_H

#include <nan.h>

#include "../edison_node.hpp"
#include "gpio.h"
#include "gpio_edison.h"

class EdisonGpioWrap : public Nan::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);
		static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
	
		gpio* getGpio();
		void setGpio(gpio *pin);
		
		EdisonGpioWrap();
	private:
		~EdisonGpioWrap();
		
		static void pinMode(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void pinWrite(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void pinRead(const Nan::FunctionCallbackInfo<v8::Value>& args);

		gpio* m_gpio;
		static Nan::Persistent<v8::Function> constructor;
};

#endif