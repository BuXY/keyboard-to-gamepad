// #define PS2_KEYBOARD
// #define MIDI_KEYBOARD
#define BASE_OCTAVE 5
#define PENCON64
// #define PD_MICRO

#include "SwitchGamepad.h"
#if defined PS2_KEYBOARD
#include "PS2KeyCode.h"
#include "KeyboardInput.h"
#elif defined MIDI_KEYBOARD
#include "MidiInput.h"
#elif defined PENCON64
#include "PenCon64Input.h"
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
const bool useInvertedSoftwareSerial = false; // If false, then the HardwareSerial rxPin = 0, txPin = 1 is used implicitly
#endif
const uint32_t refreshRateMillis = 10;
const uint32_t liveLedFlashIntervalMillis = 1000;

// Common variables

uint32_t nextMillis = 0;
#if defined PS2_KEYBOARD
KeyboardInput keyboardInput;
#elif defined MIDI_KEYBOARD
MidiInput midiInput;
#elif defined PENCON64
PenCon64Input penCon64Input;
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
	
	{keyboardInput.addModifier(PS2_KC_CTRL), &internalButtonStates.Run},
	{PS2_KC_F1, &internalButtonStates.SetRunOn},
	{PS2_KC_F2, &internalButtonStates.SetRunOff},
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
#endif

// Nintendo Switch gamepad output

void updateGamepadAnalogSticks()
{
#if defined PENCON64
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
#if defined PENCON64
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
	keyboardInput.mapKeyCodesToBools(keyMap, sizeof(keyMap) / sizeof(*keyMap));
#elif defined MIDI_KEYBOARD
	// Initialize MIDI input
	midiInput.begin();
	midiInput.mapMidiNotesToBools(noteMap, sizeof(noteMap) / sizeof(*noteMap));
#elif defined PENCON64
	penCon64Input.begin(useInvertedSoftwareSerial, baudRate, rxPin, txPin);
	penCon64Input.mapJoystickButtons(joystickButtonMap, sizeof(joystickButtonMap) / sizeof(*joystickButtonMap));
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
#elif defined MIDI_KEYBOARD
	// Handle MIDI keyboard input
	const uint8_t pressedKnownKeyCount = midiInput.updateInputs();
#elif defined PENCON64
	// Handle LightPen + joystick over RS232 input
	const uint8_t pressedKnownKeyCount = penCon64Input.updateInputs();
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
#elif defined PENCON64
	ledController->setVoltageLeds(pressedKnownKeyCount % 4);
	ledController->setTxLed(pressedKnownKeyCount % 4);
#endif
}
