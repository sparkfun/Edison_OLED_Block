#include "../edison_node.hpp"
#include "edison_spi_wrap.hpp"
#include "../gpio/edison_gpio_wrap.hpp"
#include "../gpio/gpio.h"
#include "../gpio/gpio_edison.h"
#include "spi.h"
#include <linux/spi/spidev.h>

using namespace v8;

Nan::Persistent<v8::Function> EdisonSpiWrap::constructor;

EdisonSpiWrap::EdisonSpiWrap() : m_device() {
}

EdisonSpiWrap::~EdisonSpiWrap() {
}

void EdisonSpiWrap::Init(v8::Local<v8::Object> exports) {
	Nan::HandleScope scope;

	// Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Spi").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "transferData", transferData);
	Nan::SetPrototypeMethod(tpl, "customCS", customCS);
	Nan::SetPrototypeMethod(tpl, "CSLow", CSLow);
	Nan::SetPrototypeMethod(tpl, "CSHigh", CSHigh);
	Nan::SetPrototypeMethod(tpl, "doLSBFirst", doLSBFirst);
	Nan::SetPrototypeMethod(tpl, "getSPIMode", getSPIMode);		
	//Nan::SetPrototypeMethod(tpl, "getCSPin", getCSPin);		
	
	/*
	#define SPI_CPHA                0x01
	#define SPI_CPOL                0x02	
	*/
	NODE_DEFINE_CONSTANT(exports, SPI_MODE_0); //(0|0)
	NODE_DEFINE_CONSTANT(exports, SPI_MODE_1); //(0|SPI_CPHA)
	NODE_DEFINE_CONSTANT(exports, SPI_MODE_2); //(SPI_CPOL|0)
	NODE_DEFINE_CONSTANT(exports, SPI_MODE_3); //(SPI_CPOL|SPI_CPHA)
		
	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Spi").ToLocalChecked(), tpl->GetFunction());
}

void EdisonSpiWrap::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new EdisonSpiWrap(...)`
		EdisonSpiWrap* obj = new EdisonSpiWrap();
		unsigned char spiMode = SPI_MODE_0;
		long speed = 1000000;
		bool lsbFirst = false;
		spiPort *port = new spiPort();
		
		switch (args.Length()) {
			case 0 : 
				//device = new spiDevice(port);
				break;
			case 1 : 
				if (!args[0]->IsNumber()) {
					Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
					return;
				}		
				//device = new spiDevice(port, args[0]->Uint32Value() & 0xFF);
				spiMode = args[0]->Uint32Value() & 0xFF;
				break;
			case 2 : 
				if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
					Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
					return;
				}	
				//device = new spiDevice(port, args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value());
				spiMode = args[0]->Uint32Value() & 0xFF;
				speed = args[1]->Uint32Value();
				break;
			case 3 : 
			case 4 :
				if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsBoolean()) {
					Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
					return;
				}		
				spiMode = args[0]->Uint32Value() & 0xFF;
				speed = args[1]->Uint32Value();				
				lsbFirst = Nan::To<bool>(args[2]).FromJust();				
				//device = new spiDevice(port, args[0]->Uint32Value() & 0xFF, args[1]->Uint32Value(), lsbFirst);
				break;	
			default:
				Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
				return;
				break;
		}	
		
		if (args.Length() == 4) {
			if (!args[3]->IsObject()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}			
			EdisonGpioWrap* pin = ObjectWrap::Unwrap<EdisonGpioWrap>(args[3]->ToObject());
			obj->m_pin = pin->getGpio();
			spiDevice *device = new spiDevice(port, spiMode, speed, lsbFirst, obj->m_pin);
			//spiDevice *device = new spiDevice(port, spiMode, speed, lsbFirst, gpioPin);
			//obj->m_pin = gpioPin;
			obj->m_port = port;
			obj->m_device = device;
			obj->Wrap(args.This());			
		}
		else {
			spiDevice *device2 = new spiDevice(port, spiMode, speed, lsbFirst);
			obj->m_port = port;
			obj->m_device = device2;
			obj->Wrap(args.This());	
		}
		args.GetReturnValue().Set(args.This());
	} else {
		// Invoked as plain function `EdisonSpiWrap(...)`, turn into construct call.
		//v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		//args.GetReturnValue().Set(cons->NewInstance());	
		Nan::ThrowTypeError(ERROR_CREATE_INSTANCE_WITH_NEW);
	}
}

void EdisonSpiWrap::transferData(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	unsigned char dataOut; //= args[0]->Uint32Value() & 0xFF;
	unsigned char dataIn; //= args[1]->Uint32Value() & 0xFF;
	bool deselect; // = Nan::To<bool>(args[3]).FromJust();	
	switch (args.Length()) {
		case 1 : 
			if (!args[0]->IsNumber()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}	
			dataOut = args[0]->Uint32Value() & 0xFF;		
			device->transferData(&dataOut);
			break;
		case 4 : 
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsBoolean()) {
				Nan::ThrowTypeError(ERROR_ARGUMENTS_WRONG_TYPE);
				return;
			}		
			dataOut = args[0]->Uint32Value() & 0xFF;
			dataIn = args[1]->Uint32Value() & 0xFF;
			deselect = Nan::To<bool>(args[3]).FromJust();			
			device->transferData(&dataOut, &dataIn, args[2]->Uint32Value(), deselect);
			break;
		default:
			Nan::ThrowTypeError(ERROR_WRONG_NUMBER_OF_ARGUMENTS);
			return;
			break;
	}
	args.GetReturnValue().SetUndefined();
}

void EdisonSpiWrap::customCS(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	bool out = device->customCS();
	args.GetReturnValue().Set(Nan::New<Boolean>(out));
}

void EdisonSpiWrap::CSLow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	device->CSLow();
	args.GetReturnValue().SetUndefined();
}

void EdisonSpiWrap::CSHigh(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	device->CSHigh();
	args.GetReturnValue().SetUndefined();
}

void EdisonSpiWrap::doLSBFirst(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	bool out = device->doLSBFirst();
    args.GetReturnValue().Set(Nan::New<Boolean>(out));
}

void EdisonSpiWrap::getSPIMode(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	spiDevice* device = wrap->m_device;
	int out = device->getSPIMode();
    args.GetReturnValue().Set(Nan::New<Integer>(out));
}
/*
void EdisonSpiWrap::getCSPin(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	EdisonSpiWrap* wrap = ObjectWrap::Unwrap<EdisonSpiWrap>(args.Holder());
	args.GetReturnValue().SetUndefined();
}
*/
