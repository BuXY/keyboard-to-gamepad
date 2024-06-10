#include "LedController.h"

#include <arduino.h>

void LedController::setRxLed(const bool i_enabled)
{
	if (i_enabled)
	{
		RXLED0;
	}
	else
	{
		RXLED1;
	}
}

void LedController::setTxLed(const bool i_enabled)
{
	if (i_enabled)
	{
		TXLED0;
	}
	else
	{
		TXLED1;
	}
}

void LedController::setVoltageLeds(uint8_t i_index)
{
	(void)i_index;
}

void LedController::setCurrentLeds(uint8_t i_index)
{
	(void)i_index;
}
