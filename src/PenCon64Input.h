#pragma once

#include <stddef.h>
#include <inttypes.h>

#include "SoftwareSerial.h"

class PenCon64Input
{
public:
    void begin(const bool i_useInvertedInputSoftwareSerial, const uint32_t i_baudRate, const uint8_t i_rxPin = 0, const uint8_t i_txPin = 1);

    // return: Pressed joystick button count
    uint8_t updateInputs();
    void mapJoystickButtons(bool* i_joystickButtonMap[], const size_t i_joystickButtonMapSize);
    const int32_t getLightPenX() const;
    const int32_t getLightPenY() const;

protected:
    int32_t lightPenXRaw = 128;
    int32_t lightPenX = 128;
    int32_t lightPenYRaw = 128;
    int32_t lightPenY = 128;
    uint8_t joystick1Buttons = 255;
    uint8_t joystick2Buttons = 255;
	uint8_t pressedKnownKeyCount = 0;

    enum JoystickButtonMapOrder : size_t
    {
        up1Pressed,     // 3rd byte bit 0
        down1Pressed,   // 3rd byte bit 1
        left1Pressed,   // 3rd byte bit 2
        right1Pressed,  // 3rd byte bit 3
        fire1Pressed,   // 3rd byte bit 4
        up2Pressed,     // 4th byte bit 0
        down2Pressed,   // 4th byte bit 1
        left2Pressed,   // 4th byte bit 2
        right2Pressed,  // 4th byte bit 3
        fire2Pressed,   // 4th byte bit 4
    };

    uint8_t byteCounter = 0;

    Stream* serial = nullptr;
    bool* joystickButtonMap[10];
};