#include "../edison_node.hpp"
#include "edison_gpio_wrap.hpp"
#include "gpio.h"
#include "gpio_edison.h"
#include <node.h>

using namespace v8;

Nan::Persistent<v8::Function> EdisonGpioWrap::constructor;

EdisonGpioWrap::EdisonGpioWrap() : m_gpio() {
}

EdisonGpioWrap::~EdisonGpioWrap() {
}

void EdisonGpioWrap::Init(v8::Local<v8::Object> exports) {
	Nan::HandleScope scope;

	// Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Gpio").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "pinMode", pinMode);
	Nan::SetPrototypeMethod(tpl, "pinWrite", pinWrite);
	Nan::SetPrototypeMethod(tpl, "pinRead", pinRead);
	
	/* constants */
	/* PIN_MODE*/
	NODE_DEFINE_CONSTANT(exports, INPUT);
	NODE_DEFINE_CONSTANT(exports, OUTPUT);
	NODE_DEFINE_CONSTANT(exports, INPUT_PU);
	NODE_DEFINE_CONSTANT(exports, INPUT_PD);
	NODE_DEFINE_CONSTANT(exports, SPI);
	NODE_DEFINE_CONSTANT(exports, I2C);
	NODE_DEFINE_CONSTANT(exports, PWM);
	NODE_DEFINE_CONSTANT(exports, ADC);
	NODE_DEFINE_CONSTANT(exports, UART);
	
	/* PIN_VALUE */
	NODE_DEFINE_CONSTANT(exports, LOW);
	NODE_DEFINE_CONSTANT(exports, HIGH);
	NODE_DEFINE_CONSTANT(exports, NONE);
  
	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Gpio").ToLocalChecked(), tpl->GetFunction());
}

void EdisonGpioWrap::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 2 || args.Length() > 3) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;		
	}
	int pinID = args[0]->Uint32Value();
	PIN_MODE mode = static_cast<PIN_MODE>(args[1]->Uint32Value());
	PIN_VALUE value = LOW;
	if (args.Length() == 3) {
		value = static_cast<PIN_VALUE>(args[2]->Uint32Value());
	}
	if (args.IsConstructCall()) {
		// Invoked as constructor: `new EdisonGpioWrap(...)`
		EdisonGpioWrap* obj = new EdisonGpioWrap();
		//gpio *gpioObj = new gpio(args[0]->Uint32Value(), static_cast<PIN_MODE>(args[1]->Uint32Value()), static_cast<PIN_VALUE>(args[2]->Uint32Value()));
		gpio *gpioObj = new gpio(pinID, mode, value);
		obj->m_gpio = gpioObj;
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	} else {
		// Invoked as plain function `EdisonGpioWrap(...)`, turn into construct call.
		//v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		//args.GetReturnValue().Set(cons->NewInstance());	
		Nan::ThrowTypeError(ERROR_CREATE_INSTANCE_WITH_NEW);
	}
}

gpio* EdisonGpioWrap::getGpio() {
	// this is needed by SPI wrapper
	return m_gpio;
}

void EdisonGpioWrap::setGpio(gpio *pin) {
	// this is needed by SPI wrapper
	m_gpio = pin;
}

void EdisonGpioWrap::pinMode(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonGpioWrap* wrap = ObjectWrap::Unwrap<EdisonGpioWrap>(args.Holder());
	gpio* gpioObj = wrap->m_gpio;
	gpioObj->pinMode(static_cast<PIN_MODE>(args[0]->Uint32Value()));
    args.GetReturnValue().SetUndefined();
}

void EdisonGpioWrap::pinWrite(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1) {
		Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
		return;	
	}
	if (!args[0]->IsNumber()) {
		Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
		return;
	}
	EdisonGpioWrap* wrap = ObjectWrap::Unwrap<EdisonGpioWrap>(args.Holder());
	gpio* gpioObj = wrap->m_gpio;
	gpioObj->pinWrite(static_cast<PIN_VALUE >(args[0]->Uint32Value()));
    args.GetReturnValue().SetUndefined();
}

void EdisonGpioWrap::pinRead(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonGpioWrap* wrap = ObjectWrap::Unwrap<EdisonGpioWrap>(args.Holder());
	gpio* gpioObj = wrap->m_gpio;
	int out = gpioObj->pinRead();
	args.GetReturnValue().Set(Nan::New<Integer>(out));
}
