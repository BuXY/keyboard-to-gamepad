#pragma once

#include <stdint.h>

class LedController
{
public:
	virtual void begin() {};
	virtual void setRxLed(const bool i_enabled);
	virtual void setTxLed(const bool i_enabled);
	virtual void setVoltageLeds(uint8_t i_index);
	virtual void setCurrentLeds(uint8_t i_index);
};
