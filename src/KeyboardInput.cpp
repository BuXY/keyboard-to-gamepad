#include "KeyboardInput.h"

KeyboardInput::KeyMapItem::KeyMapItem()
{
}
KeyboardInput::KeyMapItem::KeyMapItem(const size_t i_keyCode, bool* i_targetBool)
	: keyCode(i_keyCode)
	, targetBool(i_targetBool)
{
}

void KeyboardInput::begin(const uint8_t i_dataPin, const uint8_t i_irqPin)
{
	ps2Keyboard.begin(i_dataPin, i_irqPin);

	// Clear keymap
	for (size_t i = 0; i < keyMapSize; ++i)
	{
		keyMap[i] = nullptr;
	}
}

void KeyboardInput::mapKeyCodesToBools(KeyboardInput::KeyMapItem i_keyMap[], const size_t i_count)
{
	for (size_t i = 0; i < i_count; ++i)
	{
		KeyMapItem& item = i_keyMap[i];
		mapKeyCodeToBool(item.keyCode, item.targetBool);
	}
}

void KeyboardInput::mapKeyCodeToBool(const size_t i_keyCode, bool* i_targetBool)
{
	keyMap[i_keyCode] = i_targetBool;
}

size_t KeyboardInput::addModifier(const size_t i_keyCode) const
{
	return i_keyCode + modifierShiftAmount;
}

uint8_t KeyboardInput::updateInputs()
{
	uint8_t scanCode = 0;
	static bool isPressed = true;
	static bool isModifier = false;
	bool* keyPointer = nullptr;

	static uint8_t pressedKnownKeyCount = 0;

	while (scanCode = ps2Keyboard.readScanCode(), scanCode != 0)
	{
		if (scanCode == 0xF0)
		{
			isPressed = false;
			continue; // Read next byte
		}

		if (scanCode == 0xE0)
		{
			isModifier = true;
			continue; // Read next byte
		}

		// https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_2
		// Find defined key in map
		keyPointer = keyMap[(int)(scanCode + (isModifier ? modifierShiftAmount : 0))];
		if (keyPointer != nullptr)
		{
			// Update pressed key count LEDs
			if (*keyPointer != isPressed)
			{
				pressedKnownKeyCount += (isPressed ? 1 : -1);
			}

			// Update key state
			*keyPointer = isPressed;
		}

		// Assuming default behavior
		isPressed = true;
		isModifier = false;
	}

	return pressedKnownKeyCount;
}
