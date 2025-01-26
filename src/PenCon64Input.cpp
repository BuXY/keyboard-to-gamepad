#include "PenCon64Input.h"

#include "Arduino.h"

// FT232 inverts the signal it sends.
// HardwareSerial on RX1=pin0 TX0=pin1 (aka Serial1) cannot be set up to accept inverted signal.
// "Not all pins on the Leonardo and Micro boards support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI)."
// https://docs.arduino.cc/learn/built-in-libraries/software-serial/
// https://www.arduino.cc/reference/cs/language/functions/communication/serial/
// https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro

void PenCon64Input::begin(const bool i_useInvertedInputSoftwareSerial, const uint32_t i_baudRate, const uint8_t i_rxPin, const uint8_t i_txPin)
{
    if (i_useInvertedInputSoftwareSerial)
    {
        SoftwareSerial* pointerThatWillLeak = new SoftwareSerial(i_rxPin, i_txPin, false);
        pointerThatWillLeak->begin(i_baudRate);
        serial = pointerThatWillLeak;
    } else {
        Serial1.begin(i_baudRate, SERIAL_8N1);
        serial = &Serial1;
    }
}

void PenCon64Input::mapJoystickButtons(bool* i_joystickButtonMap[], const size_t i_joystickButtonMapSize)
{
    for (size_t i = 0; i < i_joystickButtonMapSize && i <= JoystickButtonMapOrder::fire2Pressed; ++i)
    {
        joystickButtonMap[i] = i_joystickButtonMap[i];
    }
}

const int32_t PenCon64Input::getLightPenX() const
{
    return lightPenX;
}

const int32_t PenCon64Input::getLightPenY() const
{
    return lightPenY;
}

uint8_t PenCon64Input::updateInputs()
{
    if (serial == nullptr)
        return 0;

    pressedKnownKeyCount = 0;

    while (serial->available() > 0)
    {
        int32_t newData = serial->read();

        // Joystick buttons should have set bits. Fix the offset by jumping forward
        if (byteCounter >= 2 && (newData & 0b11100000) != 0b11100000) {
            byteCounter = 0;
        }

        switch (byteCounter++)
        {
            case 0:
                lightPenXRaw = newData;
                newData -= 94; // Offset to center and convert to signed
                newData = newData * 128 / 100 * 2; // Scale to rough circle, pixel value is divided by 2 in the original register
                newData += 128; // Convert to unsigned
                newData = max(0, min(newData, 255)); // Limit between 0..255
                lightPenX = newData;
                break;
            case 1:
                lightPenYRaw = newData;
                if (newData < 32)
                    newData = 255; // Pre-limit the overflowing value

                newData -= 150; // Offset to center and convert to signed
                newData = newData * 128 / 100; // Scale to full height inside border
                newData += 128; // Convert to unsigned
                newData = max(0, min(newData, 255)); // Limit between 0..255
                lightPenY = newData;
                break;
            case 2:
                joystick1Buttons = newData;
                if (joystickButtonMap[JoystickButtonMapOrder::up1Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::up1Pressed]) =      (joystick1Buttons & 0b00001) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::down1Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::down1Pressed]) =    (joystick1Buttons & 0b00010) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::left1Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::left1Pressed]) =    (joystick1Buttons & 0b00100) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::right1Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::right1Pressed]) =   (joystick1Buttons & 0b01000) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::fire1Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::fire1Pressed]) =    (joystick1Buttons & 0b10000) == 0;
                break;
            case 3:
                joystick2Buttons = newData;
                if (joystickButtonMap[JoystickButtonMapOrder::up2Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::up2Pressed]) =      (joystick2Buttons & 0b00001) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::down2Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::down2Pressed]) =    (joystick2Buttons & 0b00010) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::left2Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::left2Pressed]) =    (joystick2Buttons & 0b00100) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::right2Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::right2Pressed]) =   (joystick2Buttons & 0b01000) == 0;
                if (joystickButtonMap[JoystickButtonMapOrder::fire2Pressed] != nullptr)
                    *(joystickButtonMap[JoystickButtonMapOrder::fire2Pressed]) =    (joystick2Buttons & 0b10000) == 0;
                byteCounter = 0;

                break;
            default:
                byteCounter = 0;
                break;
        }
    }

    pressedKnownKeyCount = byteCounter;

    return pressedKnownKeyCount;
}