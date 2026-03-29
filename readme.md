# USB keyboard to Nintendo Switch gamepad bridge

## Overview

**Targeted at Arduino ATmega32U4 (Micro, Leonardo, Pro Micro, PD Micro)**

This project receives inputs from a standard USB keyboard and converts them to a Nintendo Switch Pro Gamepad compatible output.

If a standard USB keyboard does not receive USB communication, it falls back to PS/2 keyboard mode. Unfortunately, some gaming keyboards do not support PS/2 keyboard fallback mode.

The Nintendo Switch is quite picky about what kind of gamepad is connected. It does not care about proper USB Report signature (like Windows does) but expects the data blob in a very specific layout. It does not skip the leading Report ID byte, it cannot handle 16-bit axis values, and does not like if the TRANSFER_RELEASE flag is set together with the actual report data.

The project tried to modify the libraries it is using the least as possible, while maintaining readability and compatibility.

## Based on projects

- https://github.com/MHeironimus/ArduinoJoystickLibrary
- https://github.com/playAbilityTech/Arduino-JoyCon-Library-for-Nintendo-Switch
- https://github.com/progmem/Switch-Fightstick
- https://github.com/PaulStoffregen/PS2Keyboard
- https://github.com/ryan-ma/PD_Micro

## Hardware configuration

### USB keyboard in PS/2 mode

- USB connector is viewed from the plug side
- White bar is at top
- Pins hanging downwards
- From left to right

| USB pin connector #     | 1      | 2       | 3       | 4     |
| ----------------------- | ------ | ------- | ------- | ----- |
| USB pin role            | +5V    | D-      | D+      | GND   |
| USB wire color          | red    | white   | green   | black |
| PS/2 pin connector #    | 4      | 1       | 5       | 3     |
| PS/2 pin role           | +5V    | Data    | Clk     | GND   |
| PS/2 wire color         | red    | white   | brown   | black |
| PD Micro pin            | 5V     | D1 (TX) | D0 (RX) | GND   |

If the board is a **PD Micro**, then the `#define PD_MICRO` row in the `.ino` file should be enabled. Comment it out for Arduino Micro and Pro Micro.

### NoteCon

TODO

### NoteCon Duo

TODO

### PenCon64

TODO

### VRcon

TODO

### TeleCon

The keypad should have the following logical layout:

Rows  | Col 1 | Col 2 | Col 3 | Col 4 (probably arranged differently) |
----- | ----- | ----- | ----- | --------------------------------------|
Row 1 |  "1"  |  "2"  |  "3"  | flash                                 |
Row 2 |  "4"  |  "5"  |  "6"  |                                       |
Row 3 |  "7"  |  "8"  |  "9"  |                                       |
Row 4 |  "*"  |  "0"  |  "#"  | redial                                |

The ribbon cable of the keypad found in the Concorde 400 phone has the following pinout:

Ribbon cable pin | Row/Col | Detection restrictions
---------------- | ------- | ---------------------------------- |
1                | Row 1   | Col 4 only                         |
2                | Col 3   | -                                  |
3                | Row 1   | Except Col 4                       |
4                | Col 1   | Row 1 only                         |
5                | Col 2   | Rows 1, 2 only                     |
6                | Row 2   | Col 4 has no physical button       |
7                | Col 1   | Rows 2, 3 only                     |
8                | Row 3   | Col 4 has no physical button       |
9                | Col 2   | Rows 3, 4 only                     |
10               | Col 1   | Row 4 only                         |
11               | Row 4   | -                                  |
12               | Col 4   | Rows 2, 3 have no physical buttons |

Therefore pins 1-3, 4-7-10, 5-9 should be connected.

Connections to the Arduino Micro Pro should be the following:

Arduino pin | Keypad logic | Concorde 400 keypad pins |
----------- | -----------  | ------------------------ |
D2          | Col 3        | 2                        |
D3          | Row 1        | 1, 3                     |
D4          | Row 2        | 6                        |
D5          | Row 3        | 8                        |
D6          | Col 2        | 5, 9                     |
D7          | Col 1        | 4, 7, 10                 |
D8          | Row 4        | 11                       |
D9          | Col 4        | 12                       |

If there is a toggle switch accessible, it should be wired to the Arduino's A3 pin. It will be used as a digital input pin to switch between the following two gamepad layouts.

Switch position | A3 pin voltage | Layout            |
--------------- | -------------- | ----------------- |
Tone            | 0V             | D-pad             |
Pulse           | 5V             | Super Smash Bros  |

The phone hook switch could be used for interrupting the USB 5V power. Alternatively, it could be wired to A2 input pin. There is no function assigned to it currently.

## Arduino library configuration

- Append the contents of `InstallArtifacts\boards.txt` to the `%LocalAppData%\Arduino*\packages\arduino\hardware\avr\*\boards.txt` file

## Compiling and uploading with Visual Studio Code

- Make sure the submodules are initialized and up to date

- In the Extension pane, install the **Arduino Extension** by Microsoft

- Open Preferences (UI), go to **Extensions/Arduino**, and enable the `useArduinoCli` flag

- Restart VScode

- Select the **avrispmkii** programmer, the **Arduino Micro Nintendo Switch Gamepad** board, and your COM port

- Press F1 and choose **Arduino: Upload**

## TODO

- Try making custom VID 0x0e6f, PID 0x0180 work

```
micro_switch.build.vid=0x1209
micro_switch.build.pid=0x0009
micro_switch.build.usb_manufacturer="InterBiometrics https pid.codes"
micro_switch.build.usb_product="BuXY Keyboard to Gamepad bridge"
```

- If custom VID/PID works, acquire one from https://pid.codes/
