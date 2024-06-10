#include "LedPdMicro.h"

#include <Arduino.h>

void LedPdMicro::begin()
{
	setRxLed(false);
	setTxLed(false);
	setVoltageLeds(0);
	setCurrentLeds(0);
}

void LedPdMicro::setVoltageLeds(uint8_t i_index)
{
	if (i_index == 0) {
		DDRE &= 0xFB;  // pinMode PE2 INPUT
		PORTE &= 0xFB;  // Clear PE2
		pinMode(Voltage_LED_2_Pin, INPUT);
		pinMode(Voltage_LED_3_Pin, INPUT);
		pinMode(Voltage_LED_4_Pin, INPUT);
	} else {
		const uint8_t led1[5] = {0, 0, 0, 0, 1};
		const uint8_t led2[5] = {1, 0, 0, 0, 1};
		const uint8_t led3[5] = {1, 1, 0, 0, 1};
		const uint8_t led4[5] = {1, 1, 1, 0, 1};
		i_index -= 1;
		if (led1[i_index]) {
			PORTE |= 0x04;  // Set PE2
		} else {
			PORTE &= 0xFB;  // Clear PE2
		}
		digitalWrite(Voltage_LED_2_Pin, led2[i_index]);
		digitalWrite(Voltage_LED_3_Pin, led3[i_index]);
		digitalWrite(Voltage_LED_4_Pin, led4[i_index]);
		DDRE |= 0x04;  // pinMode PE2 OUTPUT
		pinMode(Voltage_LED_2_Pin, OUTPUT);
		pinMode(Voltage_LED_3_Pin, OUTPUT);
		pinMode(Voltage_LED_4_Pin, OUTPUT);
	}
}

void LedPdMicro::setCurrentLeds(uint8_t i_index)
{
	if (i_index > 4)
		i_index = 4;

	if (i_index == 0) {
			pinMode(Current_LED_1_Pin, INPUT);
			pinMode(Current_LED_2_Pin, INPUT);
	} else {
			const uint8_t led1[3] = {0, 1, 1};
			const uint8_t led2[3] = {0, 0, 1};
			i_index -= 1;
			digitalWrite(Current_LED_1_Pin, led1[i_index]);
			digitalWrite(Current_LED_2_Pin, led2[i_index]);
			pinMode(Current_LED_1_Pin, OUTPUT);
			pinMode(Current_LED_2_Pin, OUTPUT);
	}
}
