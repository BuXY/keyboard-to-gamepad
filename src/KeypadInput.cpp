#include "KeypadInput.h"

void KeypadInput::begin(byte rowPins[], byte colPins[])
{
    char keys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'},
    };

    keypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, static_cast<byte>(COLS));
}

void KeypadInput::mapKeypadToBools(bool* i_keypadMap[], const size_t i_keypadMapSize)
{
    for (size_t i = 0; i < i_keypadMapSize; ++i)
    {
        keyCodeMap[i] = i_keypadMap[i];
    }
}

uint8_t KeypadInput::updateInputs()
{
    pressedKnownKeyCount = 0;

    if (keypad == nullptr)
    {
        return pressedKnownKeyCount;
    }

    if (keypad->getKeys())
    {
        for (int i = 0; i < LIST_MAX; i++)   // Scan the whole key list.
        {
            if (keypad->key[i].kcode >= 0)
            {
                switch (keypad->key[i].kstate)
                {
                    case KeyState::PRESSED:
                    case KeyState::HOLD:
                        if (keyCodeMap[keypad->key[i].kcode] != nullptr)
                            *keyCodeMap[keypad->key[i].kcode] = true;
                        pressedKnownKeyCount++;
                        break;
                    case KeyState::IDLE:
                    case KeyState::RELEASED:
                        if (keyCodeMap[keypad->key[i].kcode] != nullptr)
                            *keyCodeMap[keypad->key[i].kcode] = false;
                        break;
                }
            }
        }
    }

    return pressedKnownKeyCount;
}
