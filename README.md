# IRMP on Pico - a USB-HID-Keyboard IR receiver/sender/power switch with wake-up timer

A remote control receiver with many functions based on cheap hardware.

<img src="img/RP2040-One+TSOP.jpg" width="33%"> [1]  
<img src="img/IR-Sensor_1.jpg" width="96%"> [2]  

## Introduction
For boards with RP2040 and RP2350. An open source firmware with many functions will be flashed on them.

## Features
* connection via USB 
* registers as USB HID Keyboard device *and* appears as /dev/hidraw or "Custom HID device", no drivers needed
* transfers the data over USB2 in hardware at fullspeed 
* IR receiver (about 40 protocols) 
* power on PC via remote control from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard switch (+5V required, on USB or from PSU or motherboard) 
* power on PC via built-in timer from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard switch (+5V required, on USB or from PSU or motherboard) 
* IR transmitter (about 40 protocols) 
* the configuration is stored in the emulated eeprom
* macro support (a sequence of IR commands will be sent, when a certain key is received)

## Software Linux
* GUI configuration tool irmpconfig_gui for configuration: set, get and reset wakeups, macros, irdata, keys, alarm time, repeat. It is possible to program wakeups, macros and irdata via remote control. Create the eeprom map with remote control and mouse. Comprehensive debug messages. Firmware update.
* interactive command line program irmpconfig for configuration: set, get and reset wakeups, macros, irdata, keys, alarm time, repeat and  send IR. It is possible to program wakeups, macros and irdata by remote control.
* irmpalarm to set and read alarm time via script

## Software Windows
* GUI configuration tool irmpconfig_gui (same as linux)
* interactive command line programm irmpconfig (same as linux)
* irmpalarm to set and read alarm time via script

## Creating an eeprom map
irmpconfig_gui can be used to create an eeprom map using the keyboard and the remote control.  
There are three methods.  
  
(1)  
![keyboard + irdata mode](img/keyboard+irdata_mode.jpg)  
First method: Click on a line, click on "set by remote - keyboard + irdata", press the modifier or key on your keyboard, press the key if the first was a modifier, press the button on the remote.  
The red text below the keyboard + irdata button will guide you through the process.  
Repeat as many times as necessary.  
  
(2)  
![setting irdata in template map](img/setting_irdata_into_template_map.jpg)  
Second method: Open the template keymap. Remove lines with keys you don't need.  
Click on the line with the name of the key, click "set by remote - irdata", press the button on the remote - repeat this for all buttons.  
Click on "flash eeprom".  
  
(3)  
![building eeprom map](img/building_eeprom_map.jpg)  
Third method: Click a line, click "set by remote - irdata", press the button on the remote.  
Click a line, type the key (and modifier), click "set - key".  
Repeat as many times as necessary.  

If you press the keyboard + irdata-button twice, you can test your keyboard keys and on Windows even already configured buttons on the remote.  

On Linux you can test the eeprom map with evtest: After pressing the button on the remote, evtest should show the corresponding key.

It is recommended to start with a fresh eeprom by pressing "eeprom map - reset eeprom".

## Emulated Eeprom
Any configuration made by one of the configuration programs goes first into cache only. To permanently save
these changes  to flash, you have to do an eeprom commit.
Exception: the first wakeup is committed by the firmware for backward compatibility.

## Learning wakeup
If the wakeup is empty, the first IR data received will be stored in the wakeup.  
Change wakeup with irmpconfig_gui: press set by remote - wakeup, press button on remote control.  
Important for testing: wakeup only happens, if the PC is switched off.  
If the PC is powered on, only the key is sent to the PC (so you can use the same button to switch on and off).  

## Sending IR

The specific characteristics of the protocols must be taken into account. These include inverted bits, checksums, etc.

## Building from source
See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started)  
-> Get the SDK and examples  
-> Install the toolchain  
-> Build "Blink"  
-> Load and run "Blink"  

## Flashing the firmware
Disconnect from USB.
Attach to USB while holding down the BOOTSEL button, then release. The device will appear as a mass storage (USB ID 2e8a:0003).  
Drag and drop the firmware.uf2 file onto it. The device will reboot and start as an IRMP HID Device.

If there is already an older firmware on the device, irmpconfig -> b will put the device into mass storage mode, and
[picotool](https://github.com/raspberrypi/picotool) load -v -x firmware.bin will flash the firmware, verify and start it. Instead of picotool you can use picoflash. Or drag and drop.

The most convenient option is to use irmpconfig_gui.  

## Motherboard test
If one of the powerswitch pins is on ground and the other on ca. +3,3V or +5V, then the motherboard is suitable for the following simple wiring.  
If not (very rare), you need an optocoupler.

## Solder and connect cables
Split a 10 cm dupont cable, connect both ends and the 220 ohm resistor, connect the other end of the resistor with the 20 cm dupont cable, from which one socket is cut off. Shrink the solder connections and attach the other 10 cm cable with shrink tube.  
The TSOP is connected directly to the 3,3V, GND and IR_IN pins, the pair of cables is inserted between the power-on pins of the mainboard and the connector from the power button.  

![cables](img/cables.jpg)
![connected](img/connected.jpg)
![Zero with plug](img/RP2040-Zero+plug.jpg)
![Zero with cabling](img/RP2040-Zero+cabling.jpg)
[3]  

## Pin-Out
See /src/config.h.

## eventlircd, udev and systemd
See /configuration/README.

## 5 V from power supply instead via USB
If you need to power the device from the power supply, it must not be powered via USB in order to avoid cross-current.
One way is to cut the copper on the pcb:  

![cut VBUS](img/RP2040-One_VBUS_trennen.jpg)

## Signals from LEDs
The pico(2) has a regular led, the one and the zero have an RGB led, and the XIAO-RP2350 has a dual and an RGB led.  
An external led or external RGB led (WS2812 or APA106) can be connected.  
They show what is happening inside the firmware.  
Then there is the Status led (controlled over hidraw or via HID-keyboard), which shows status messages from the vdr-plugin-statusleds2irmp (and blinks on power-on, storage of first wakeup and reboot).

| Receiver              | Board/External RGB-LED                           | Board/External LED | External Status-LED                              |
|-----------------------|--------------------------------------------------|--------------------|--------------------------------------------------|
| disconnected          | off                                              |                    |                                                  |
| USB resumed           | white (or custom)                                |                    |                                                  |
| USB suspended         | orange                                           |                    |                                                  |
| IR reception          | flickers blue                                    | flickers           |                                                  |
| save wakeup           | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Wakeup                | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Reboot                | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Send IR               | short yellow                                     | short on           |                                                  |
| VDR running           | red(*)                                           |                    | on(*)                                            |
| VDR recording         | flashes red according to number of recordings(*) |                    | flashes according to number of recordings(*)     |
| configuration command | short green                                      | short blink        |                                                  |
| firmware upgrade      |                                                  | short blink        | short blink                                      |

(*) needs vdr-plugin-statusleds2irmp

## External RGB-LEDs
It is recommended to use two resistors in the data cable, see http://stefanfrings.de/ws2812/.

## Schematics
https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf  
https://datasheets.raspberrypi.com/pico/pico-2-datasheet.pdf  
https://files.waveshare.com/upload/4/4c/RP2040_Zero.pdf  
https://files.waveshare.com/upload/9/90/RP2040-One.pdf  
https://files.seeedstudio.com/wiki/XIAO-RP2350/res/Seeed-Studio-XIAO-RP2350-v1.0.pdf  

## Thanks to
Frank Meyer for IRMP. [1]  
Ole Ernst for code review and new protocol. [2]  
Manuel Reimer for SimpleCircuit. [3]  
Martin Neubauer for integration into EasyVDR. [4]  
Alexander Grothe for integration into yaVDR and much help with difficult questions. [5]  
Helmut Emmerich for selling ready-to-use receiver sets and user support. [6]  
Claus Muus for integration into MLD. [7]  
All Users for questions, feature requests, feedback and ideas.  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] https://github.com/olebowle  
[3] https://github.com/M-Reimer  
[4] https://www.vdr-portal.de/user/4786-ranseyer/, https://github.com/ranseyer/STM32-IRMP-Hardware  
[5] https://www.vdr-portal.de/user/24681-seahawk1986/  
[6] https://www.vdr-portal.de/user/13499-emma53/  
[7] https://www.vdr-portal.de/user/942-clausmuus/  

## Discussion and further information
Comments, questions and suggestions can be sent to https://www.vdr-portal.de/forum/thread/137133-irmp-auf-pico-ein-usb-hid-keyboard-ir-empfänger-sender-einschalter-mit-wake-up-t/.

Have fun with IRMP on Pico!

Copyright (C) 2014-2026 Jörg Riechardt

##
  \
[1] Waveshare RP2040-One with TSOP by clausmuus, see https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220  
[2] Waveshare RP2040-One with TSOP by FireFly, see https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419  
[3] Waveshare RP2040-Zero with plug and cabling by Emma53, see https://www.vdr-portal.de/forum/thread/136344-usb-rp-2040-zero-i-r-empf%C3%A4nger-komplett/
