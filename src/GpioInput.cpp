#include "GpioInput.h"

#include <Arduino.h>

void GpioInput::begin(const uint8_t i_allInputPins[], size_t i_inputPinsCount)
{
	memset(allInputPins, 255, c_maxDigitalInputs);
	memcpy(allInputPins, i_allInputPins, i_inputPinsCount);
	for (size_t i = 0; i < i_inputPinsCount; ++i)
	{
		pinMode(allInputPins[i], INPUT_PULLUP);
	}
}

void GpioInput::mapDigitalInputsToBools(bool* i_gpioMap[], const size_t i_gpioMapSize)
{
	for (size_t i = 0; i < i_gpioMapSize; ++i)
	{
		pinStates[i] = i_gpioMap[i];
	}
}

uint8_t GpioInput::updateInputs()
{
	uint8_t pressedKnownKeyCount = 0;

	for (size_t i = 0; i < c_maxDigitalInputs; ++i)
	{
		if (allInputPins[i] == 255 || pinStates[i] == nullptr)
			continue;

		const bool isPressed = digitalRead(allInputPins[i]) == LOW;
		*pinStates[i] = isPressed;
		if (isPressed)
			++pressedKnownKeyCount;
	}

	return pressedKnownKeyCount;
}
