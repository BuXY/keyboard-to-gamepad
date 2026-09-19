#pragma once

#include <avr/common.h>
// This is a terrible workaround for an ODR problem.
// There is absoutely no need for this include for this class.
// The macro ZL is defined in the "Pointer registers definitions" section
// and would make this class definition different when included from InternalButtonStates.cpp or main.cpp.
// Now the ZL property is actually called r30, which does not cause any problems,
// but the rest of the code can use the ZL name.

class InternalButtonStates
{
public:
	// Button states

	bool PrimaryLeft = false;
	bool PrimaryRight = false;
	bool PrimaryUp = false;
	bool PrimaryDown = false;

	bool SecondaryLeft = false;
	bool SecondaryRight = false;
	bool SecondaryUp = false;
	bool SecondaryDown = false;

	bool DPadLeft = false;
	bool DPadRight = false;
	bool DPadUp = false;
	bool DPadDown = false;

	bool A = false;
	bool B = false;
	bool X = false;
	bool Y = false;

	bool L = false;
	bool R = false;
	bool ZL = false;
	bool ZR = false;

	bool Minus = false;
	bool Plus = false;
	bool PrimaryStick = false;
	bool SecondaryStick = false;
	bool Home = false;
	bool Capture = false;

	bool Run = false;
	bool SetRunOn = false;
	bool SetRunOff = false;

	// Latch states

	bool alwaysRunLatch = true;

	void updateLatches();
	void updateLatch(bool* io_latch, bool* i_setOn, bool* i_setOff);
	bool getRunState() const;
};
