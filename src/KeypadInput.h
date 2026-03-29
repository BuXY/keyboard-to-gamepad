#pragma once

#include <stddef.h>
#include <inttypes.h>

#include "Keypad.h"

class KeypadInput
{
public:
    void begin(byte rowPins[], byte colPins[]);

    // return: Pressed key count
    uint8_t updateInputs();
    void mapKeypadToBools(bool* i_keypadMap[], const size_t i_keypadMapSize);

protected:

	uint8_t pressedKnownKeyCount = 0;

    static const size_t ROWS = 4;
    static const size_t COLS = 4;
	bool* keyCodeMap[ROWS * COLS];

    Keypad* keypad = nullptr;
};