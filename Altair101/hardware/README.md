--------------------------------------------------------------------------------
# Altair 101 Build Plan

Modern computers have a keyboard and monitor. A mobile phone has a touch screen display.
My Altair 101 has a front panel of toggles and LED lights.
That's what the original Altair 8800 had in 1975, toggles and lights.

My goals are keep soldering to minimum, design for adding modern components, and a low cost.
The Altair 8800 simulator clone uses an Arduino Due. I will use an Arduino Nano.
All other Altair 8800 clones use a PCB board. I will use breadboards.
I got the idea of building a computer on breadboards from Dan Eater's videos on 
[how to Build an 8-bit computer from scratch](https://www.youtube.com/watch?v=HyznrdDSSGM&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=14)

My hardware designed around my [Altair 8800 clone front panel](https://www.adwaterandstir.com/product/front-panel/).

[<img width="360px"  src="../FrontPanel01a.jpg"/>](https://www.adwaterandstir.com/product/front-panel/)

--------------------------------------------------------------------------------
## Nano Board Pin Usage

Has 30 pins.
````
                           -----
                       ---| USB |---
 SD card: SPI bus D13 |   |     |   | D12 SPI bus: SD card
                  3V3 |   |     |   | D11 SPI bus: SD card
                  REF |    -----    | D10 SD card SPI enable/disable
  Address toggles  A0 |             | D9  Address toggles
Infrared receiver  A1 |             | D8  Address toggles
                   A2 |             | D7  Address toggles
                   A3 |             | D6  Front panel LED lights
          I2C bus  A4 |             | D5  Front panel LED lights
          I2C bus  A5 |             | D4  Front panel LED lights
                   A6 |             | D3  
                   A7 |             | D2  
                   5V |             | GND Join with external power ground
                  RST |             | RST
                  GND |             | RX0 Used for Serial communications
                  VIN |             | TX1 Used for Serial communications
                       -------------

RX0 and TX1 are in use, for example: to upload program sketches and Serial.print().
D2 and D3 are Digital interrupt pins.
A0 to A3 can be used as digital pins.
I/O devices:
+ SD card:  SPI bus + a digital enable pin.
+ Clock:    12C bus
+ 1602 LCD: 12C bus 
+ DFPlayer: 2 digital pins.
+ 4 x 7-segment digits: 2 digital pins.
+ SRAM 32K: SPI bus + a digital enable pin.
+ Rotary encoder: 1 digital pin and an interrupt pin.
````
--------------------------------------------------------------------------------
### Development Boards and Sample Programs

Front panel toggle input test board:
+ 6 input buttons for 6 data bits.
+ 6 diodes to control current flow.
+ Nano, using 4 pins.
+ A 74HC595 Shift Register to Expand Digital Inputs.
+ Serial monitor log output.
+ To do: 2 input On/Off/On momentary toggles for to control 4 data bit settings.
+ To do: Test with a PCF8574 board
+ Program [link](../shiftRegisterInput/shiftRegisterInput.ino)

Front panel 16 LED output test board:
+ Use 3 pins of a Nano board
+ 2 x 74HC595 Shift Register
+ 2 x LED bar-graph (10 LEB bars) and 16 resistors.
+ Program [link, one shift register](../shiftRegisterOne/shiftRegisterOne.ino)
+ Program [link, two shift registers](../shiftRegisterTwo/shiftRegisterTwo.ino)

SD card board test:
+ Nano, using 4 pins: 3 SPI + 1 for enable/disable.
+ SD card module
+ Program [link](../sdCard/sdCard.ino)

Clock board, #1:
+ Nano board, using 3 pins: A4 & A5 for I2C communications 
+ DS3231 clock module
+ 1602 LC display
+ Program to display and set time.
+ Program [link](../../samples/clockSet/clockSet.ino)
Clock board, #2:
+ 2 Nano boards
+ DS3231 clock module
+ 2 x 2-7-segment digit displays
+ Program to display time, and communicate between Nano boards.
+ Program [link](../../samples/clock2x2digitsMin/clock2x2digitsMin.ino)
+ Program [link](../../samples/clock2x2digitsHours/clock2x2digitsHours.ino)

Rotary encoder board:
+ Nano, using 4 pins: 3 SPI + 1 for enable/disable.
+ SD card module
+ Program [link](../../samples/RotaryEncoder/RotaryEncoder.ino)
+ Program [link](../../samples/RotaryEncoder2digits/RotaryEncoder2digits.ino)

MP3 player:
+ Nano, using 2 pins. Need to test by using RXD and TX1.
+ DFPlayer module.
+ Infrared receiver for play/pause/next/previous. Input is from a remote controller.

0 pins for power off/on toggle.

3 pins for LED lights using SN74HC595N chips:
+ D4, D5, D6
+ Each SN74HC595N chip handles 8 bits, for 8 LED lights.
+ 1 chip for status LED lights, one of which will be used for the WAIT light.
+ 1 chip for data LED lights.
+ 2 chips for address LED lights.

3 pins for switches using SN74HC595N chips:
+ D7, D8, D9
+ 2 chips for address toggles.
+ 1 chip for control on/off/on toggles: STOP, RUN, SINGLE STEP, EXAMINE PREVIOUS, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT.

1 pin for infrared.
+ Need to test it on another pin, currently using: D10.

4 pins for SD card module,
+ D10 pin for enable/disable.
+ SPI digital pins: D11, D12, D13.

Need to test: 3 pins for control outputs, using a SN74HC595N chip:
+ Need to test with pins: A0, A2, A3.

Not used:
+ D2, D3, the digital interrupt pins

Nano pins:
+ D2...D13 digital pins.
+ A0, A2, A3 work as digital pins.
+ A4 and A5 for IC2 communications with LCD and clock modules.
+ A1, A6 and A7 didn't work for LED on and off. Probably not work as digital pins.
+ RX0 (D0) and TX1 (D1), for for RS232 serial communications, example: uploading programs to the Nano, or Serial.print commands
    Serial.begin() will cause pins 0(D0) and 1(D1) to be serial pins.
    Topic: Serial Input Basics, https://forum.arduino.cc/index.php?topic=396450

Hardware components:
+ Nano
+ 7 SN74HC595N chips
+ 36 LED lights. Not all used.
+ 17 toggles.
+ 8 on/off/on toggles.

Other modules to add:
+ Clock: I2C (A4 & A5).
+ Rotary encoder: requires an interrupt pin (2 or 3).

--------------------------------------------------------------------------------
### Altair 101 Case

I bought the Altair 8800 simulater Pro case which is part of the [expansion upgrade](https://www.adwaterandstir.com/product/upgrade/).
I plan to modify mine for ease of maintenance, and to cover the blue with something a bit more to my style.

[<img width="360px"  src="https://www.adwaterandstir.com/wp-content/uploads/2019/07/AD1.jpg"/>](https://www.adwaterandstir.com/product/upgrade/)

The cost of my front panel, sticker, case, and shipping, was a reasonable $74.
However, my target was to keep the cost to about $100, which didn't leave much for the electronics.
I used Ebay.com to select and order parts, mostly from China. The total is $101.
Wow, $101. An auspicious cost for an Altair 101.

Following is the Original Altair 8800 price list.
The [Altar 8800 Clone site](https://altairclone.com/) used the original price list, as base for their
[price list](https://altairclone.com/ordering.htm), (assembled) $621.
The red markups are notes regarding their product offering.

[<img width="300px"  src="originalPricelist.jpg"/>](https://altairclone.com/ordering.htm)

My Altair 101 parts are listed [below](#altair-101-parts-list).

--------------------------------------------------------------------------------
### Build steps,

In the following video, the person is building an Altair 8800 Replica using a strategy that is similar to my plan.
https://www.youtube.com/watch?v=zqRILp6srBk&t=830s

Solder wires to the toggles and add the toggles onto the front panel.

[<img width="300px"  src="AltairReplica00.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

Wire and add LEDs onto the front panel.

[<img width="300px"  src="AltairReplica01.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

Front panel with wired toggles and LED lights.

[<img width="300px"  src="AltairReplica02.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

Put the completed front panel into the top part of the computer case.

[<img width="300px"  src="AltairReplica03.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

Wire the front panel components to the Arduino Due.

[<img width="300px"  src="AltairReplica04.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

[<img width="300px"  src="AltairReplica05.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

[<img width="300px"  src="AltairReplica06.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

[<img width="300px"  src="AltairReplica09.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

[<img width="300px"  src="AltairReplica10.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

With the Arduino connected to the front panel components, successful program tests were run.

[<img width="300px"  src="AltairReplica11.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

It was completed by putting the case together with the electronics enclosed.

[<img width="300px"  src="AltairReplica12.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

[<img width="300px"  src="AltairReplica14.jpg"/>](https://www.youtube.com/watch?v=zqRILp6srBk&t=830s)

--------------------------------------------------------------------------------
### Front Panel I/O Hardware Breadboards

Build a toggle input test board:
+ 4 input toggles for 4 data bits.
+ 2 input On/Off/On momentary toggles for to control 4 data bit settings.
+ Use 3 Nano pins. Use a chip or board to Expand Digital Inputs.
+ Test using a 74HC595 Shift Register
+ Test with a PCF8574 board

Build an 8 LED output test board:
+ Use 3 pins of a Nano board
+ 74HC595 Shift Register
+ 1 LED bar-graph (10 LEB bars) and 8 resistors.

https://www.youtube.com/watch?v=cAT07gy4DII

The development Altair 101 will,
+ Use 3 Nano pins to turn the LEDs on and off.
+ 1 x 74HC595 Shift Register, 8 data LEDs, resistors
+ 2 x 74HC595 Shift Register, 16 address LEDs, resistors
+ Use 1 Nano pin for 1 LED to display that the computer is in the WAIT state or not. When power is first turned on, the WAIT LED comes on.
+ Final computer will also have a HLT LED and 10 operational LEDs, for a total of 36 LEDs (8+16+1+1+10).

Note, using 74HC595 chips or a PCF8574 board, power them from outside the Nano.
This means there is plenty of power to run the chips and LEDs.
+ +5V to Nano 5V.
+ Ground to Nano ground.
+ +5V and ground to each 74HC595.
+ Not powering the 74HC595 and LEDs from the Nano +5V pin when the power comes into the Nano from the USB port.
+ Using the +5V pin when the power comes into the Nano from the USB port, will be okay to build a single 74HC595 chip test board.

--------------------------------------------------------------------------------
#### LED Outputs

Build an LED output test board:
+ 8 LEDs for data bits.
+ 3 Nano pins, 74HC595 Shift Register, resistors, 1 LED bar for testing.

https://www.youtube.com/watch?v=cAT07gy4DII

Altair 101 breadboards for 36 LEDs (2+8+16+10):
+ 1 Nano
+ Use 3 Nano pins to turn the LEDs on and off.
+ 8 data LEDs, 3 Nano pins,
    1 x 74HC595 Shift Register, resistors
+ 16 address LEDs, 3 Nano pins,
    2 x 74HC595 Shift Register, resistors
+ 8 operational LEDs to display with the computer is in WAIT or not,
    1 x 74HC595 Shift Register, resistors

Using 74HC595 chips, means there is plenty of power to run the LEDs.

--------------------------------------------------------------------------------
#### Toggle switch Inputs

I need to work out a method for reading toggles:
+ Memory address and data toggles to set on or off.
+ On/Off/On momentary toggles which are momentarily flipped, verses toggle set on or off.

Build an input test board:
+ 4 input toggles for 4 data bits.
+ 2 input On/Off/On momentary toggles for to control 4 data bit settings.
+ Use 3 Nano pins. Use a chip or board to Expand Digital Inputs.
+ Test using a 74HC595 Shift Register
+ Test using a 74HC165 parallel to serial chip
+ Test with a PCF8574 board

Altair 101 board for 25 toggles (1+16 toggles + 8 momentary toggles):
+ 1 Nano
+ Use 3 Nano pins for reading the 16 input sense switches (address/data toggles), 
    2 x 74HC595 Shift Register or use 2 Port Extender boards: PCF8574?
+ Use 3 pins for reading On/Off/On momentary toggles. Each toggle has 2 settings.
+ To start, need 2 On/Off/On momentary toggles for Examine/Examine and Next/Deposit Next,
    1 x 74HC595 Shift Register, 4 bits total.
+ 1 On/Off/On momentary toggle for Reset and CLR (clear),
    which is 2 more bits on the above 595.

https://www.ebay.com/itm/5-x-74HC165-74165-IC-8-BIT-SHIFT-REGISTER-FREE-SHIPPING/251118499363
https://www.ebay.com/itm/10pcs-74HC165-SN74HC165N-8-Bit-Parallel-Load-Shift-Registers-DIP-16/181847051341

--------------------------------------------------------------------------------
### Front Panel Cables Required for My Build

Male to Male Ribbon Cables
+ 1 power toggle:
    2 x 20cm Male to Male Ribbon
+ 16 address toggles:
    16 x 20cm Male to Male Ribbon for control + 1 x 20cm Male to Male Ribbon for input into 74HC595
    15 x 10cm Male to Male Ribbon for ground connection from one toggle to the next + 1 x 20cm Male to Male Ribbon to ground
+ 8 control (16 on/off) on/off/on momentary toggles: 
    16 x 20cm Male to Male Ribbon for control + 1 x 20cm Male to Male Ribbon for input into 74HC595
     7 x 10cm Male to Male Ribbon for ground connection from one toggle to the next + 1 x 20cm Male to Male Ribbon to ground
+ LEDs: 8 data + 16 address:
    24 x 20cm Female to Male Ribbon for control + 1 x 20cm Female to Male Ribbon for input into 74HC595
    23 x 10cm Female to Female Ribbon for ground connection from one LED to the next + 1 x 20cm Female to Male Ribbon to ground
+ LEDs: 2 state + 8 status:
    10 x 20cm Female to Male Ribbon for control + 1 x 20cm Female to Male Ribbon for input into 74HC595
     9 x 10cm Female to Female Ribbon for ground connection from one LED to the next + 1 x 20cm Female to Male Ribbon to ground
+ LED: 2 x on/off (WAIT + HLDA):
     4 x 20cm Female to Male Ribbon for control + 1 x 20cm Female to Male Ribbon for input into 74HC595

Totals:
+ Toggles: 20cm Male to Male Ribbon:
    2 + 18 + 18
+ Toggles: 10cm Male to Male Ribbon:
    0 + 15 +  7
+ LEDs: 20cm Female to Male Ribbon:
    26 + 12 + 4
+ LEDs: 10cm Female to Female Ribbon:
    23 +  9 + 0
+ 20cm Male to Male Ribbon: 38
+ 10cm Male to Male Ribbon: 22
+ 20cm Female to Male Ribbon: 42
+ 10cm Female to Female Ribbon: 32

--------------------------------------------------------------------------------
### Altair 101 Parts List

For reference, I used the following parts list from a clone:
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6

+ Front panel, sticker, case, and shipping = $18.00 + $5 + $35 + $16 = $74
+ Nano + on/off/on momentary toggles + on/off toggles + red LEDs = $3 + $7 + $2 + $2
+ 20pcs 74HC595 + pin expander module PCF8574 = $2 + $1
+ Breadboards + cables = $6 + $4
+ Total = $101 = $74 + $3 + $7 + $2 + $2 + $2 + $1 + $6 + $4

+ Ordered Altair 8800 Front panel with sticker, standard case, and shipping: $74
https://www.adwaterandstir.com/product/front-panel/

+ Nano
https://www.ebay.com/itm/MINI-USB-Nano-V3-0-ATmega328P-CH340G-5V-16M-Micro-controller-board-for-Arduino/383093281539

+ SPDT On/Off/On 3 Position Momentary toggles, Diameter: 6mm (0.2inch), Micro mini, 10pcs for $6.79
https://www.ebay.com/itm/10pcs-Red-3-Pin-3-Position-ON-OFF-ON-SPDT-Micro-Mini-Momentary-Toggle-Switch/223490809691

+ SPDT On/On 2 Position Mini Toggle Switch, Diameter: 6mm (0.2inch), 10Pcs for $2.68
https://www.ebay.com/itm/10Pcs-AC-250V-1-5A-125V-3A-SPDT-3-Pin-On-On-2-Position-Mini-Toggle-Switch-Blue/272882765795

+ Red LED 5mm, 100pcs for $1.50
https://www.ebay.com/itm/100Pcs-LED-Lights-Emitting-Diodes-Lamp-Parts-3mm-5mm-for-Electronics-Arduino-DIY/202692215007?var=502802895935

+ 220Ω, 470Ω, 560Ω, 1k, or 3k resistors?
+ Need to test to get the right ohms for brightness and wattage for physical size.
+ Use A09 Network Resistor 9-pin module?

+ Shift Register SN74HC595N, 20pcs for $2
https://www.ebay.com/itm/5-10-20pcs-Chip-Stable-2-0-6-0-V-74HC595-8-Bit-IC-Shift-Register-SN74HC595N/173212029799?var=471929852731

+ 10uf capacitor across positive and ground when using chips: SN74HC595N.

Cables on order,
+ 1 x 40pcs/pack 20cm Male to Male Ribbon Breadboard Cable
+ 1 x 40pcs/pack 10cm Male to Male Ribbon Breadboard Cable
+ 2 x 40pcs/pack 20cm Female to Male Ribbon Breadboard Cable
+ 1 x 40pcs/pack 10cm Female to Female Ribbon Breadboard Cable
+ 140pcs Solderless Breadboard Jumper Cable Wire Kit Box, $2.46

----------------
#### Parts to test

+ GPIO Port Extenders chip: PCF8574 should work for toggles.

+ An option to consider, is addressable LEDs that come on strips.
+ Arduino – Controlling a WS2812 LED strand with NeoPixel or FastLED
https://www.tweaking4all.com/hardware/arduino/arduino-ws2812-led/

--------------------------------------------------------------------------------
### Reference Links

Ben Eater's kit, [Build an 8-bit computer from scratch](https://eater.net/8bit/kits).

+ How to build one.
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6
+ Build your own.
https://spectrum.ieee.org/geek-life/hands-on/build-your-own-altair-8800-personal-computer
+ About, with parts list
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6

+ Assembled & Tested, $279.95 – $349.95
https://www.adwaterandstir.com/product/altair-assembled/

+ Google forums:
https://groups.google.com/forum/#!forum/altair-duino

+ Getting started with the Arduino Due
https://www.arduino.cc/en/Guide/ArduinoDue
+ The Arduino Due is a microcontroller board based on the Atmel SAM3X8E ARM Cortex-M3 CPU.
+ It is the first Arduino board based on a 32-bit ARM core microcontroller.
+ The microcontroller mounted on the Arduino Due runs at 3.3V
+ 54 digital input/output pins, 12 analog inputs
+ Uses CH340G drivers
+ DUE R3 Board SAM3X8E 32-bit ARM Cortex-M3 Control Board Module For Arduino, US $14.64
https://www.ebay.com/itm/DUE-R3-Board-SAM3X8E-32-bit-ARM-Cortex-M3-Control-Board-Module-For-Arduino/141976885203
+ Intro to the Arduino Due
https://www.theengineeringprojects.com/2018/09/introduction-to-arduino-due.html
+ Uses the Arduino Due which is an Arduino for more powerful larger scale projects.
https://store.arduino.cc/usa/due
++ The Arduino Due is the first Arduino board based on a 32-bit ARM core microcontroller.
++ 54 digital input/output pins, 12 analog inputs, a 84 MHz clock

--------------------------------------------------------------------------------
### Reference links

+ Instruction set.
http://brooknet.no-ip.org/~lex/altair/_altair88b/manual/instructMain.html
+ Example: jump (jmp) 11 000 011 

+ How to load software onto the Arduino Due.
https://www.adwaterandstir.com/install/
+ Code repository
https://github.com/dhansel/Altair8800

+ How to build one.
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6
+ Build your own.
https://spectrum.ieee.org/geek-life/hands-on/build-your-own-altair-8800-personal-computer
+ About, with parts list
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6

+ Getting started with the Arduino Due
https://www.arduino.cc/en/Guide/ArduinoDue
+ The Arduino Due is a microcontroller board based on the Atmel SAM3X8E ARM Cortex-M3 CPU.
+ It is the first Arduino board based on a 32-bit ARM core microcontroller.
+ The microcontroller mounted on the Arduino Due runs at 3.3V
+ 54 digital input/output pins, 12 analog inputs
+ Uses CH340G drivers
+ DUE R3 Board SAM3X8E 32-bit ARM Cortex-M3 Control Board Module For Arduino, US $14.64
https://www.ebay.com/itm/DUE-R3-Board-SAM3X8E-32-bit-ARM-Cortex-M3-Control-Board-Module-For-Arduino/141976885203
+ Intro to the Arduino Due
https://www.theengineeringprojects.com/2018/09/introduction-to-arduino-due.html
+ Uses the Arduino Due which is an Arduino for more powerful larger scale projects.
https://store.arduino.cc/usa/due
++ The Arduino Due is the first Arduino board based on a 32-bit ARM core microcontroller.
++ 54 digital input/output pins, 12 analog inputs, a 84 MHz clock

+ Assembled & Tested, $279.95 – $349.95
https://www.adwaterandstir.com/product/altair-assembled/

Google forums:
https://groups.google.com/forum/#!forum/altair-duino

+ Manual
https://grantmestrength.github.io/RetroComputerInstructionManual/
https://github.com/GrantMeStrength/RetroComputerInstructionManual

+ Original Altair manual
http://www.classiccmp.org/dunfield/altair/d/88opman.pdf

+ Altair 8800 Clone, Ordering Information, (assembled) $621
https://altairclone.com/ordering.htm

+ MITS Altair Simulator, written entirely in Javascript.
+ and with thanks to Martin Maly and Chris Double for their 8080.js Intel 8080 emulator. 
https://s2js.com/altair/
https://s2js.com/altair/sim.html
https://github.com/maly/8080js

Following are reference links,
+ Altair 101 parts listed [below](https://github.com/tigerfarm/arduino/tree/master/Altair101#parts-to-order-to-build-my-altair-101).
+ Click [here](https://altairclone.com/downloads/)
    for the website of a high end Altair 8800 clone.
+ Click [here](https://altairclone.com/downloads/)
    for Altair 8800 original and clone documents.
+ Click [here](https://altairclone.com/downloads/interview.mp3)
    to hear an interview with the Altair 8800 clone creator, Mike Douglas.
+ Altair 8800 Instructional Videos
    https://www.youtube.com/playlist?list=PLB3mwSROoJ4KLWM8KwK0cD1dhX35wILBj

Arduino SPI Library
https://www.arduino.cc/en/reference/SPI

--------------------------------------------------------------------------------
Cheers