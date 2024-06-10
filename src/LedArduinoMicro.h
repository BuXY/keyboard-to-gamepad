#pragma once

#include "LedController.h"

class LedArduinoMicro : public LedController
{
public:
	void begin() override;
	void setVoltageLeds(uint8_t i_index) override;

	void setBuiltinLed(const bool i_enabled);
};
