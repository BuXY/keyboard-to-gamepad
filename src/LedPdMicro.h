#pragma once

#include "LedController.h"

class LedPdMicro : public LedController
{
public:
	void begin() override;
	void setVoltageLeds(uint8_t i_index) override;
	void setCurrentLeds(uint8_t i_index) override;

protected:
	// const uint8_t Voltage_LED_1_Pin = PE2; // Cannot set by library, but should be high for the other voltage LEDs to operate.
	const uint8_t Voltage_LED_2_Pin  = 22;
	const uint8_t Voltage_LED_3_Pin = 23;
	const uint8_t Voltage_LED_4_Pin = 11;

	const uint8_t Current_LED_1_Pin = 13;
	const uint8_t Current_LED_2_Pin = 12;
};
