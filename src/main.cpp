#define PS2_KEYBOARD
// #define PD_MICRO

#include "SwitchGamepad.h"
#if defined PS2_KEYBOARD
#include "PS2KeyCode.h"
#include "KeyboardInput.h"
#endif
#ifdef PD_MICRO
#include "LedPdMicro.h"
#else
#include "LedArduinoMicro.h"
#endif
#include "InternalButtonStates.h"

// General config

#if defined PS2_KEYBOARD
const uint8_t dataPin_usbDataMinus = 1; // D1 / PD3 / TXD1 / INT3 / white
const uint8_t irqpin_usbDataPlus = 0;   // D0 / PD2 / RXD1 / AIN1 / INT2 / green
#endif
const uint32_t refreshRateMillis = 10;
const uint32_t liveLedFlashIntervalMillis = 1000;

// Common variables

uint32_t nextMillis = 0;
#if defined PS2_KEYBOARD
KeyboardInput keyboardInput;
#endif
SwitchGamepad gamepadOutput;
LedController* ledController = nullptr;

// Internal button states

InternalButtonStates internalButtonStates;

#if defined PS2_KEYBOARD
KeyboardInput::KeyMapItem keyMap[] = 
{
	{keyboardInput.addModifier(PS2_KC_L_ARROW), &internalButtonStates.PrimaryLeft},
	{keyboardInput.addModifier(PS2_KC_R_ARROW), &internalButtonStates.PrimaryRight},
	{keyboardInput.addModifier(PS2_KC_UP_ARROW), &internalButtonStates.PrimaryUp},
	{keyboardInput.addModifier(PS2_KC_DN_ARROW), &internalButtonStates.PrimaryDown},

	{keyboardInput.addModifier(PS2_KC_DELETE), &internalButtonStates.SecondaryLeft},
	{keyboardInput.addModifier(PS2_KC_PGDN), &internalButtonStates.SecondaryRight},
	{keyboardInput.addModifier(PS2_KC_HOME), &internalButtonStates.SecondaryUp},
	{keyboardInput.addModifier(PS2_KC_END), &internalButtonStates.SecondaryDown},

	{PS2_KC_H, &internalButtonStates.DPadLeft},
	{PS2_KC_K, &internalButtonStates.DPadRight},
	{PS2_KC_U, &internalButtonStates.DPadUp},
	{PS2_KC_J, &internalButtonStates.DPadDown},

	{PS2_KC_S, &internalButtonStates.A},
	{PS2_KC_A, &internalButtonStates.B},
	{PS2_KC_W, &internalButtonStates.X},
	{PS2_KC_Q, &internalButtonStates.Y},

	{PS2_KC_CAPS, &internalButtonStates.L},
	{PS2_KC_D, &internalButtonStates.R},
	{PS2_KC_TAB, &internalButtonStates.ZL},
	{PS2_KC_E, &internalButtonStates.ZR},

	{PS2_KC_KP_MINUS, &internalButtonStates.Minus},
	{PS2_KC_KP_PLUS, &internalButtonStates.Plus},
	{PS2_KC_R, &internalButtonStates.PrimaryStick},
	{PS2_KC_F, &internalButtonStates.SecondaryStick},
	{PS2_KC_ESC, &internalButtonStates.Home},
	{PS2_KC_F12, &internalButtonStates.Capture},
	
	{keyboardInput.addModifier(PS2_KC_CTRL), &internalButtonStates.Run},
	{PS2_KC_F1, &internalButtonStates.SetRunOn},
	{PS2_KC_F2, &internalButtonStates.SetRunOff},
};
#endif

// Nintendo Switch gamepad output

void updateGamepadAnalogSticks()
{
	gamepadOutput.setXAxis(
		gamepadOutput.getDigitalAxis(
			internalButtonStates.PrimaryLeft,
			internalButtonStates.PrimaryRight,
			internalButtonStates.getRunState()));

	gamepadOutput.setYAxis(
		gamepadOutput.getDigitalAxis(
			internalButtonStates.PrimaryUp,
			internalButtonStates.PrimaryDown,
			internalButtonStates.getRunState()));

	gamepadOutput.setZAxis(
		gamepadOutput.getDigitalAxis(
			internalButtonStates.SecondaryLeft,
			internalButtonStates.SecondaryRight));

	gamepadOutput.setRzAxis(
		gamepadOutput.getDigitalAxis(
			internalButtonStates.SecondaryUp,
			internalButtonStates.SecondaryDown));
}

void updateGamepadDPad()
{
	const int16_t dPadValue = gamepadOutput.getDPadValue(
		internalButtonStates.DPadLeft,
		internalButtonStates.DPadRight,
		internalButtonStates.DPadUp,
		internalButtonStates.DPadDown);

	gamepadOutput.setHatSwitch(0, dPadValue);
}

void updateGamepadButtons()
{
	gamepadOutput.setButton(gamepadOutput.Button_Index_X, internalButtonStates.X);
	gamepadOutput.setButton(gamepadOutput.Button_Index_B, internalButtonStates.B);
	gamepadOutput.setButton(gamepadOutput.Button_Index_A, internalButtonStates.A);
	gamepadOutput.setButton(gamepadOutput.Button_Index_Y, internalButtonStates.Y);

	gamepadOutput.setButton(gamepadOutput.Button_Index_L, internalButtonStates.L);
	gamepadOutput.setButton(gamepadOutput.Button_Index_R, internalButtonStates.R);
	gamepadOutput.setButton(gamepadOutput.Button_Index_ZL, internalButtonStates.ZL);
	gamepadOutput.setButton(gamepadOutput.Button_Index_ZR, internalButtonStates.ZR);

	gamepadOutput.setButton(gamepadOutput.Button_Index_Minus, internalButtonStates.Minus);
	gamepadOutput.setButton(gamepadOutput.Button_Index_Plus, internalButtonStates.Plus);
	gamepadOutput.setButton(gamepadOutput.Button_Index_PrimaryStick, internalButtonStates.PrimaryStick);
	gamepadOutput.setButton(gamepadOutput.Button_Index_SecondaryStick, internalButtonStates.SecondaryStick);
	gamepadOutput.setButton(gamepadOutput.Button_Index_Home, internalButtonStates.Home);
	gamepadOutput.setButton(gamepadOutput.Button_Index_Capture, internalButtonStates.Capture);
}

// LED controller

void updateLiveLeds()
{
	// Handle live LED
	const bool liveLedState = (millis() % liveLedFlashIntervalMillis) < (liveLedFlashIntervalMillis >> 1);

	ledController->setRxLed(liveLedState);
	ledController->setTxLed(internalButtonStates.getRunState());

	uint8_t currentLedIndex = (millis() / 100) % 8;
	if (currentLedIndex == 4)
	{
		currentLedIndex = 0;
	}
	else if (currentLedIndex > 4)
	{
		currentLedIndex = 4 - (currentLedIndex - 4);
	}
	ledController->setCurrentLeds(currentLedIndex);
}

// Arduino internal

void setup() {
#if defined PS2_KEYBOARD
	// Initialize PS/2 keyboard input
	keyboardInput.begin(dataPin_usbDataMinus, irqpin_usbDataPlus);
	keyboardInput.mapKeyCodesToBools(keyMap, sizeof(keyMap) / sizeof(*keyMap));
#endif

	// Initialize Nintendo Switch gamepad output
	const bool joyAutoSendState = false;
	gamepadOutput.begin(joyAutoSendState);
	gamepadOutput.setAllAxisRange(0, 1 << 8);

	// Initialize LED controller
#ifdef PD_MICRO
	ledController = new LedPdMicro();
#else
	ledController = new LedArduinoMicro();
#endif
	ledController->begin();
}

void loop() {
	// Refresh rate and early exit
	if (millis() < nextMillis)
	{
		return;
	}
	nextMillis = millis() + refreshRateMillis;

#if defined PS2_KEYBOARD
	// Handle PS/2 keyboard input
	const uint8_t pressedKnownKeyCount = keyboardInput.updateInputs();
#endif
	internalButtonStates.updateLatches();

	// Handle Nintendo Switch gamepad output
	updateGamepadAnalogSticks();
	updateGamepadDPad();
	updateGamepadButtons();
	gamepadOutput.sendState();

	// Handle LED states
	updateLiveLeds();
	ledController-> setVoltageLeds((pressedKnownKeyCount + 1 % 6));
}
