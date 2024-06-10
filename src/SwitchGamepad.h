/*
  SwitchGamepad.h

  Copyright (c) 2024, BuXY

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SWITCH_GAMEPAD_h
#define SWITCH_GAMEPAD_h

#include "ArduinoJoystickLibrary/src/Joystick.h"

class SwitchGamepad : public Joystick_
{
public:
	static constexpr uint8_t Button_Index_X = 3; //0;
	static constexpr uint8_t Button_Index_B = 1;
	static constexpr uint8_t Button_Index_A = 2;
	static constexpr uint8_t Button_Index_Y = 0; //3;

	static constexpr uint8_t Button_Index_L = 4;
	static constexpr uint8_t Button_Index_R = 5;
	static constexpr uint8_t Button_Index_ZL = 6;
	static constexpr uint8_t Button_Index_ZR = 7;

	static constexpr uint8_t Button_Index_Minus = 8;
	static constexpr uint8_t Button_Index_Plus = 9;
	static constexpr uint8_t Button_Index_PrimaryStick = 10;
	static constexpr uint8_t Button_Index_SecondaryStick = 11;
	static constexpr uint8_t Button_Index_Home = 12;
	static constexpr uint8_t Button_Index_Capture = 13;

	SwitchGamepad();
	void setAllAxisRange(const int32_t minimum, const int32_t maximum);
	int32_t getDigitalAxis(const bool i_minimumButton, const bool i_maximumButton, const bool i_fullRange = true) const;
	int16_t getDPadValue(const bool i_leftButton, const bool i_rightButton, const bool i_upButton, const bool i_downButton) const;

protected:
	int32_t axisNegativeValue;
	int32_t axisSlightNegativeValue;
	int32_t axisMidValue;
	int32_t axisSlightPositiveValue;
	int32_t axisPositiveValue;
};

#endif // SWITCH_GAMEPAD_h
