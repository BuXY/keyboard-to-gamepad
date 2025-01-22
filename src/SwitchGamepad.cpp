/*
  SwitchGamepad.cpp

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

#include "SwitchGamepad.h"

SwitchGamepad::SwitchGamepad()
	: Joystick_(
	JOYSTICK_DONT_SEND_REPORT_ID, // hidReportId
	JOYSTICK_TYPE_GAMEPAD, // joystickType
	14,		// buttonCount
	1,		// hatSwitchCount
	true,	// includeXAxis
	true,	// includeYAxis
	true,	// includeZAxis
	false,	// includeRxAxis
	false,	// includeRyAxis
	true,	// includeRzAxis
	false,	// includeRudder
	false,	// includeThrottle
	false,	// includeAccelerator
	false,	// includeBrake
	false,	// includeSteering
	true,	// sendAxesAs8bit
	true)	// addOneBytePadding
{}

void SwitchGamepad::setAllAxisRange(const int32_t minimum, const int32_t maximum)
{
	setXAxisRange(minimum, maximum);
	setYAxisRange(minimum, maximum);
	setZAxisRange(minimum, maximum);
	setRzAxisRange(minimum, maximum);

	axisNegativeValue = minimum;
	axisPositiveValue = maximum;
	axisMidValue = (axisNegativeValue + axisPositiveValue) / 2 + 1; // Because of Arduino's map() miscalculation.
	axisSlightNegativeValue = (axisNegativeValue + axisMidValue) / 2;
	axisSlightPositiveValue = (axisMidValue + axisPositiveValue) / 2;
}

int32_t SwitchGamepad::getDigitalAxis(const bool i_minimumButton, const bool i_maximumButton, const bool i_fullRange) const
{
	if (i_minimumButton == i_maximumButton)
		return axisMidValue;

	if (i_fullRange)
		return i_maximumButton ? axisPositiveValue : axisNegativeValue;
	else
		return i_maximumButton ? axisSlightPositiveValue : axisSlightNegativeValue;
}

int16_t SwitchGamepad::getDPadValue(const bool i_leftButton, const bool i_rightButton, const bool i_upButton, const bool i_downButton) const
{
	int16_t value = JOYSTICK_HATSWITCH_RELEASE;

	constexpr uint8_t LeftBitMask = 0b1000;
	constexpr uint8_t RightBitMask = 0b0100;
	constexpr uint8_t UpBitMask = 0b0010;
	constexpr uint8_t DownBitMask = 0b0001;

	uint8_t dPadBitField = 0;
	dPadBitField |= (i_leftButton && !i_rightButton) ? LeftBitMask : 0;
	dPadBitField |= (!i_leftButton && i_rightButton) ? RightBitMask : 0;
	dPadBitField |= (i_upButton && !i_downButton) ? UpBitMask : 0;
	dPadBitField |= (!i_upButton && i_downButton) ? DownBitMask : 0;

	switch (dPadBitField) {
		case                UpBitMask:
			value = 0;
			break;
		case RightBitMask | UpBitMask:
			value = 1;
			break;
		case RightBitMask:
			value = 2;
			break;
		case RightBitMask | DownBitMask:
			value = 3;
			break;
		case                DownBitMask:
			value = 4;
			break;
		case LeftBitMask  | DownBitMask:
			value = 5;
			break;
		case LeftBitMask:
			value = 6;
			break;
		case LeftBitMask  | UpBitMask:
			value = 7;
			break;
		default:
			break;
	}

	return value * 45;
}
