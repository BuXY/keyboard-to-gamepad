#include "LedArduinoMicro.h"

#include <Arduino.h>

void LedArduinoMicro::begin()
{
	pinMode(LED_BUILTIN, OUTPUT);
	setRxLed(false);
	setTxLed(false);
}

void LedArduinoMicro::setBuiltinLed(const bool i_enabled)
{
	digitalWrite(LED_BUILTIN, i_enabled ? HIGH : LOW);
}

void LedArduinoMicro::setVoltageLeds(uint8_t i_index)
{
	setBuiltinLed((i_index % 2) != 1);
}
