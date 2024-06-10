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
