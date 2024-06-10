#pragma once

#include "PS2Keyboard/PS2Keyboard.h"

class KeyboardInput
{
public:
	struct KeyMapItem
	{
		size_t keyCode = 0;
		bool* targetBool = nullptr;

		KeyMapItem();
		KeyMapItem(const size_t i_keyCode, bool* i_targetBool);
	};

	void begin(const uint8_t i_dataPin, const uint8_t i_irqPin);
	void mapKeyCodesToBools(KeyMapItem i_keyMap[], const size_t i_count);
	void mapKeyCodeToBool(const size_t i_keyCode, bool* i_targetBool);
	size_t addModifier(const size_t i_keyCode) const;

	// return: Pressed key count
	uint8_t updateInputs();

protected:
	static constexpr size_t keyMapSize = 256 * 2;
	static constexpr size_t modifierShiftAmount = keyMapSize / 2;

	bool* keyMap[keyMapSize];
	// PS2Keyboard* ps2Keyboard = nullptr;
	PS2Keyboard ps2Keyboard;
};
