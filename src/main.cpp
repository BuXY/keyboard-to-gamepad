#define PS2_KEYBOARD
// #define MIDI_KEYBOARD
#define BASE_OCTAVE 5
// #define PENCON64
// #define VRCON
// #define TELECON
// #define PD_MICRO

#include "SwitchGamepad.h"
#if defined PS2_KEYBOARD
#include "PS2KeyCode.h"
#include "KeyboardInput.h"
#elif defined MIDI_KEYBOARD
#include "MidiInput.h"
#elif defined(PENCON64) || defined(VRCON)
#include "PenCon64Input.h"
#elif defined TELECON 
#include "KeypadInput.h"
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
#elif defined MIDI_KEYBOARD
#elif defined PENCON64
const uint8_t rxPin = 9;
const uint8_t txPin = 8;
const uint32_t baudRate = 2400;
const bool useInvertedSoftwareSerial = false;
#elif defined VRCON
const uint8_t rxPin = 9;
const uint8_t txPin = 8;
const uint32_t baudRate = 9600;
const bool useInvertedSoftwareSerial = true;
#elif defined TELECON
byte rowPins[4] = { 3, 4, 5, 8 };
byte colPins[4] = { 7, 6, 2, 9 };
const uint8_t pulseToneSwitchPin = A3;
const int pulseToneValuePulseSmash = HIGH;
const int pulseToneValueToneDpad = LOW;
#endif
const uint32_t refreshRateMillis = 10;
const uint32_t liveLedFlashIntervalMillis = 1000;

// Common variables

uint32_t nextMillis = 0;
#if defined PS2_KEYBOARD
KeyboardInput keyboardInput;
#elif defined MIDI_KEYBOARD
MidiInput midiInput;
#elif defined(PENCON64) || defined(VRCON)
PenCon64Input penCon64Input;
#elif defined TELECON
KeypadInput keypadInput;
#endif
SwitchGamepad gamepadOutput;
LedController* ledController = nullptr;

// Internal button states

InternalButtonStates internalButtonStates;

#if defined PS2_KEYBOARD
bool f3_selectDefaultBuxyKeyMap = false;
bool f4_selectHurimKeyMap = false;
bool f5_selectPocketZKeyMap = false;

KeyboardInput::KeyMapItem defaultBuxyKeyMap[] = 
{
	{PS2_KC_F3, &f3_selectDefaultBuxyKeyMap},
	{PS2_KC_F4, &f4_selectHurimKeyMap},
	{PS2_KC_F5, &f5_selectPocketZKeyMap},

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

	{PS2_KC_TAB, &internalButtonStates.L},
	{PS2_KC_E, &internalButtonStates.R},
	{PS2_KC_CAPS, &internalButtonStates.ZL},
	{PS2_KC_D, &internalButtonStates.ZR},

	{PS2_KC_KP_MINUS, &internalButtonStates.Minus},
	{PS2_KC_KP_PLUS, &internalButtonStates.Plus},
	{PS2_KC_R, &internalButtonStates.PrimaryStick},
	{PS2_KC_F, &internalButtonStates.SecondaryStick},
	{PS2_KC_ESC, &internalButtonStates.Home},
	{keyboardInput.addModifier(PS2_KC_PRTSCR), &internalButtonStates.Capture},
	
	{PS2_KC_CTRL, &internalButtonStates.Run},
	{keyboardInput.addModifier(PS2_KC_CTRL), &internalButtonStates.Run},
	{PS2_KC_F1, &internalButtonStates.SetRunOn},
	{PS2_KC_F2, &internalButtonStates.SetRunOff},
};

KeyboardInput::KeyMapItem hurimKeyMap[] = 
{
	{PS2_KC_F1, &internalButtonStates.SetRunOn},
	{PS2_KC_F2, &internalButtonStates.SetRunOff},
	{PS2_KC_F3, &f3_selectDefaultBuxyKeyMap},
	{PS2_KC_F4, &f4_selectHurimKeyMap},
	{PS2_KC_F5, &f5_selectPocketZKeyMap},

	// wasd for moving
	{PS2_KC_A, &internalButtonStates.PrimaryLeft},
	{PS2_KC_D, &internalButtonStates.PrimaryRight},
	{PS2_KC_W, &internalButtonStates.PrimaryUp},
	{PS2_KC_S, &internalButtonStates.PrimaryDown},

	// e is jump
	{PS2_KC_E, &internalButtonStates.X},
	// q is shield
	{PS2_KC_Q, &internalButtonStates.ZL},
	// u is grab
	{PS2_KC_U, &internalButtonStates.L},
	// i is another shield
	{PS2_KC_I, &internalButtonStates.ZR},

	// o is B button
	// p is A button
	{PS2_KC_P, &internalButtonStates.A},
	{PS2_KC_O, &internalButtonStates.B},

	// j is c stick left
	// k is c stick down
	// l is c stick up
	// é is c stick right
	{PS2_KC_J, &internalButtonStates.SecondaryLeft},
	{PS2_KC_SEMI, &internalButtonStates.SecondaryRight},
	{PS2_KC_L, &internalButtonStates.SecondaryUp},
	{PS2_KC_K, &internalButtonStates.SecondaryDown},

	// 1 above q is d pad left
	// 2 is d pad up
	// 3 is d pad down
	// 4 is d pad right
	{PS2_KC_1, &internalButtonStates.DPadLeft},
	{PS2_KC_4, &internalButtonStates.DPadRight},
	{PS2_KC_2, &internalButtonStates.DPadUp},
	{PS2_KC_3, &internalButtonStates.DPadDown},

	// extra buttons i like to have is R and space, and ig shift as that comes in reach
	{PS2_KC_R, &internalButtonStates.R},
	{PS2_KC_SPACE, &internalButtonStates.Y},

	// backspace -
	// enter +
	{PS2_KC_BS, &internalButtonStates.Minus},
	{PS2_KC_ENTER, &internalButtonStates.Plus},
	// right shift home
	{PS2_KC_R_SHIFT, &internalButtonStates.Home},
	// print screen screen shot
	{keyboardInput.addModifier(PS2_KC_PRTSCR), &internalButtonStates.Capture},
};

KeyboardInput::KeyMapItem pocketZKeyMap[] = 
{
	{PS2_KC_F1, &internalButtonStates.SetRunOn},
	{PS2_KC_F2, &internalButtonStates.SetRunOff},
	{PS2_KC_F3, &f3_selectDefaultBuxyKeyMap},
	{PS2_KC_F4, &f4_selectHurimKeyMap},
	{PS2_KC_F5, &f5_selectPocketZKeyMap},

	// mozogni a wasd vel szeretnék
	{PS2_KC_A, &internalButtonStates.PrimaryLeft},
	{PS2_KC_D, &internalButtonStates.PrimaryRight},
	{PS2_KC_W, &internalButtonStates.PrimaryUp},
	{PS2_KC_S, &internalButtonStates.PrimaryDown},

	// ütés J
	// special K
	{PS2_KC_J, &internalButtonStates.A},
	{PS2_KC_K, &internalButtonStates.B},

	// shield L
	// grab U
	{PS2_KC_L, &internalButtonStates.ZL},
	{PS2_KC_U, &internalButtonStates.L},

	// ugrani space
	{PS2_KC_SPACE, &internalButtonStates.X},

	// 4 irányt nevezz meg a secondary stickre
	// akkor a fel legyen a H
	// le N
	// Bal Q
	// jobb E
	{PS2_KC_Q, &internalButtonStates.SecondaryLeft},
	{PS2_KC_E, &internalButtonStates.SecondaryRight},
	{PS2_KC_H, &internalButtonStates.SecondaryUp},
	{PS2_KC_N, &internalButtonStates.SecondaryDown},

	// itt nincs olyan hogy walk button igaz?
	// Legyen az i
	{PS2_KC_I, &internalButtonStates.Run},

	// akkor a 4 taunt meg legyen a iop9
	// toljuk eggyel arrébb a d padot
	// öopő
	{PS2_KC_O, &internalButtonStates.DPadLeft},
	{PS2_KC_OPEN_SQ, &internalButtonStates.DPadRight},
	{PS2_KC_0, &internalButtonStates.DPadUp},
	{PS2_KC_P, &internalButtonStates.DPadDown},

	// plusz enter
	// home esc
	// a minus lehet a backspace
	{PS2_KC_ENTER, &internalButtonStates.Plus},
	{PS2_KC_BS, &internalButtonStates.Minus},
	{PS2_KC_ESC, &internalButtonStates.Home},
};

#elif defined MIDI_KEYBOARD
MidiInput::NoteMapItem noteMap[] = 
{
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::C), &internalButtonStates.PrimaryLeft},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::E), &internalButtonStates.PrimaryRight},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Ds), &internalButtonStates.PrimaryUp},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::D), &internalButtonStates.PrimaryDown},

	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::F), &internalButtonStates.SecondaryLeft},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::A), &internalButtonStates.SecondaryRight},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Gs), &internalButtonStates.SecondaryUp},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::G), &internalButtonStates.SecondaryDown},

	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::C), &internalButtonStates.DPadLeft},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::E), &internalButtonStates.DPadRight},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::Ds), &internalButtonStates.DPadUp},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::D), &internalButtonStates.DPadDown},

	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::A), &internalButtonStates.A},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::G), &internalButtonStates.B},
	// {midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::As), &internalButtonStates.X},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::Gs), &internalButtonStates.Y},

	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::Fs), &internalButtonStates.L},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::As), &internalButtonStates.R},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::F), &internalButtonStates.ZL},
	// {midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::B), &internalButtonStates.ZR},

	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::As), &internalButtonStates.Minus},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::B), &internalButtonStates.Plus},
	// {midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Cs), &internalButtonStates.PrimaryStick},
	// {midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Fs), &internalButtonStates.SecondaryStick},
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::Cs), &internalButtonStates.Home},
	// {midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::As), &internalButtonStates.Capture},
	
	{midiInput.buildMidiNote(BASE_OCTAVE + 0, MidiInput::NoteEnum::B), &internalButtonStates.Run},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Cs), &internalButtonStates.SetRunOn},
	{midiInput.buildMidiNote(BASE_OCTAVE + 1, MidiInput::NoteEnum::Fs), &internalButtonStates.SetRunOff},
};
#elif defined PENCON64
bool* joystickButtonMap[] = {
	// Joystick 1: up (also lightpen button), down, left, right, fire
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	// Joystick 2: up, down, left, right, fire
	&internalButtonStates.ZL,
	&internalButtonStates.B,
	&internalButtonStates.Y,
	&internalButtonStates.A,
	&internalButtonStates.Plus,
};
#elif defined VRCON
bool* joystickButtonMap[] = {
	// 3rd byte bit 0, 1, 2, 3, 4
	&internalButtonStates.ZL,
	&internalButtonStates.R,
	nullptr,
	nullptr,
	nullptr,
	// 4th byte bit 0, 1, 2, 3, 4
	&internalButtonStates.A,
	&internalButtonStates.B,
	&internalButtonStates.X,
	&internalButtonStates.Y,
	&internalButtonStates.Plus,
};
#elif defined TELECON
bool* keypadMapDpad[] = {
	// Row 1: 1, 2, 3, Flash
	nullptr,
	&internalButtonStates.DPadUp,
	nullptr,
	&internalButtonStates.Minus,
	// Row 2: 4, 5, 6, B
	&internalButtonStates.DPadLeft,
	&internalButtonStates.DPadDown,
	&internalButtonStates.DPadRight,
	nullptr,
	// Row 3: 7, 8, 9, C
	nullptr,
	&internalButtonStates.DPadDown,
	nullptr,
	nullptr,
	// Row 4: *, 0, #, Redial
	nullptr,
	&internalButtonStates.B,
	&internalButtonStates.A,
	&internalButtonStates.Plus,
};
bool* keypadMapSmash[] = {
	// Row 1: 1, 2, 3, Flash
	&internalButtonStates.SetRunOn,
	&internalButtonStates.PrimaryUp,
	&internalButtonStates.SetRunOff,
	&internalButtonStates.Run,
	// Row 2: 4, 5, 6, B
	&internalButtonStates.PrimaryLeft,
	&internalButtonStates.PrimaryDown,
	&internalButtonStates.PrimaryRight,
	nullptr,
	// Row 3: 7, 8, 9, C
	&internalButtonStates.R,
	&internalButtonStates.Y,
	&internalButtonStates.X,
	nullptr,
	// Row 4: *, 0, #, Redial
	&internalButtonStates.ZL,
	&internalButtonStates.B,
	&internalButtonStates.A,
	&internalButtonStates.Plus,
};
#endif

// Nintendo Switch gamepad output

void updateGamepadAnalogSticks()
{
#if defined(PENCON64) || defined(VRCON)
	gamepadOutput.setXAxis(penCon64Input.getLightPenX());
	gamepadOutput.setYAxis(penCon64Input.getLightPenY());
#else
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
#endif

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
#if defined(PENCON64) || defined(VRCON)
#else
	ledController->setTxLed(internalButtonStates.getRunState());
#endif

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
	keyboardInput.mapKeyCodesToBools(defaultBuxyKeyMap, sizeof(defaultBuxyKeyMap) / sizeof(*defaultBuxyKeyMap));
#elif defined MIDI_KEYBOARD
	// Initialize MIDI input
	midiInput.begin();
	midiInput.mapMidiNotesToBools(noteMap, sizeof(noteMap) / sizeof(*noteMap));
#elif defined(PENCON64) || defined(VRCON)
	penCon64Input.begin(useInvertedSoftwareSerial, baudRate, rxPin, txPin);
	penCon64Input.mapJoystickButtons(joystickButtonMap, sizeof(joystickButtonMap) / sizeof(*joystickButtonMap));
#elif defined TELECON
	// Initialize phone keypad input
	keypadInput.begin(rowPins, colPins);
	keypadInput.mapKeypadToBools(keypadMapDpad, sizeof(keypadMapDpad) / sizeof(*keypadMapDpad));
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
	const bool possibleKeymapChange =
		  !f3_selectDefaultBuxyKeyMap
		& !f4_selectHurimKeyMap
		& !f5_selectPocketZKeyMap;
	const uint8_t pressedKnownKeyCount = keyboardInput.updateInputs();
	KeyboardInput::KeyMapItem* newKeyMap = nullptr;
	uint8_t newSize = 0;
	if (possibleKeymapChange)
	{
		if (f3_selectDefaultBuxyKeyMap)
		{
			newKeyMap = defaultBuxyKeyMap;
			newSize = sizeof(defaultBuxyKeyMap) / sizeof(*defaultBuxyKeyMap);
		}
		else if (f4_selectHurimKeyMap)
		{
			newKeyMap = hurimKeyMap;
			newSize = sizeof(hurimKeyMap) / sizeof(*hurimKeyMap);
		}
		else if (f5_selectPocketZKeyMap)
		{
			newKeyMap = pocketZKeyMap;
			newSize = sizeof(pocketZKeyMap) / sizeof(*pocketZKeyMap);
		}
		if (newKeyMap)
		{
			keyboardInput.clearKeymap();
			keyboardInput.mapKeyCodesToBools(newKeyMap, newSize);
		}
	}
#elif defined MIDI_KEYBOARD
	// Handle MIDI keyboard input
	const uint8_t pressedKnownKeyCount = midiInput.updateInputs();
#elif defined(PENCON64) || defined(VRCON)
	// Handle LightPen + joystick over RS232 input
	const uint8_t pressedKnownKeyCount = penCon64Input.updateInputs();
#elif defined TELECON
	// Handle pulse/tone switch
	switch (digitalRead(pulseToneSwitchPin))
	{
		case pulseToneValuePulseSmash:
			keypadInput.mapKeypadToBools(keypadMapSmash, sizeof(keypadMapSmash) / sizeof(*keypadMapSmash));
			break;
		case pulseToneValueToneDpad:
			keypadInput.mapKeypadToBools(keypadMapDpad, sizeof(keypadMapDpad) / sizeof(*keypadMapDpad));
			break;
	}

	// Handle phone keypad input
	const uint8_t pressedKnownKeyCount = keypadInput.updateInputs();
#endif
	internalButtonStates.updateLatches();

	// Handle Nintendo Switch gamepad output
	updateGamepadAnalogSticks();
	updateGamepadDPad();
	updateGamepadButtons();
	gamepadOutput.sendState();

	// Handle LED states
	updateLiveLeds();
#if defined PS2_KEYBOARD
	ledController->setVoltageLeds((pressedKnownKeyCount + 1 % 6));
#elif defined MIDI_KEYBOARD
	ledController->setVoltageLeds(pressedKnownKeyCount % 6);
#elif defined(PENCON64) || defined(VRCON)
	ledController->setVoltageLeds(pressedKnownKeyCount % 4);
	ledController->setTxLed(pressedKnownKeyCount % 4);
#elif defined TELECON
	ledController->setVoltageLeds(pressedKnownKeyCount % 4);
#endif
}
