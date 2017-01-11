#include <nan.h>
#include "oled/edison_oled_wrap.hpp"
#include "gpio/edison_gpio_wrap.hpp"
#include "spi/edison_spi_wrap.hpp"

using namespace v8;

void InitAll (v8::Local<v8::Object> exports) {
    EdisonOledWrap::Init(exports);
	EdisonGpioWrap::Init(exports);
    EdisonSpiWrap::Init(exports);
}

NODE_MODULE(edisonoledaddon, InitAll)