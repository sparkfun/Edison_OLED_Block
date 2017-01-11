#ifndef EDISON_SPI_WRAP_H
#define EDISON_SPI_WRAP_H

#include <nan.h>

#include "../edison_node.hpp"
#include "../gpio/edison_gpio_wrap.hpp"
#include "../gpio/gpio.h"
#include "../gpio/gpio_edison.h"
#include "spi.h"

class EdisonSpiWrap : public Nan::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);
		static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
	
		EdisonSpiWrap();
	private:
		~EdisonSpiWrap();
		
		static void transferData(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void customCS(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void CSLow(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void CSHigh(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void doLSBFirst(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getSPIMode(const Nan::FunctionCallbackInfo<v8::Value>& args);
		static void getCSPin(const Nan::FunctionCallbackInfo<v8::Value>& args);

		//gpio* m_pin;
		gpio* m_pin;
		spiPort* m_port;
		spiDevice* m_device;
		static Nan::Persistent<v8::Function> constructor;
};

#endif