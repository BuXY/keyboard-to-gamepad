#pragma once

#include <stddef.h>
#include <inttypes.h>

class GpioInput
{
public:
	void begin(const uint8_t i_allInputPins[], size_t i_inputPinsCount);

	// return: Pressed button count
	uint8_t updateInputs();
	void mapDigitalInputsToBools(bool* i_gpioMap[], const size_t i_gpioMapSize);

protected:
	static const size_t c_maxDigitalInputs = 22;
	uint8_t allInputPins[c_maxDigitalInputs];
	bool* pinStates[c_maxDigitalInputs];
};
