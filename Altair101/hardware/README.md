--------------------------------------------------------------------------------
# Altair 101 Hardware Notes

--------------------------------------------------------------------------------
#### Arduino Due

USB and Power:
````
A DC connector for voltage supply between 7V and 12V.
The USB Programming port, next to the DC connector, is for uploading sketches and communicating with the Arduino.
The Native USB port can be used as a virtual serial port using the "SerialUSB" object in the Arduino programming language.
The Native USB port has other uses as well.
````

Add Arduino Due board option to the IDE: https://www.arduino.cc/en/Guide/Cores
````
Click on Tools menu and then Tools/Board/Boards Manager.
Install Arduino SAM Boards (32-bits ARM Cortex-M3) by Arduino, boards: Arduino Due.
I installed version 1.6.12.
Once installed, select: Tools/Arduino ARM/Arduino Due
````

Configure the IDE: https://www.arduino.cc/en/Guide/ArduinoDue
````
I powered the Arduino Due on and off a few times before the new port showed up.
Loaded the auduinoTest.ino program into the IDE and clicked Upload.
The program loaded and ran fine.
  Sketch uses 11164 bytes (2%) of program storage space. Maximum is 524288 bytes.
  ...
Onboard LED light blinks on and off.
The Loop counter messages shows in the IDE monitor window.
````

Issue compiling Processor.ino for Due:
````
Need to change A12 and A14, as those are not Due pins. Due has A0 to A11.
Need to remove LCD references, to compile.
    WARNING: library LiquidCrystal_I2C claims to run on avr architecture.
Should use "#define SETUP_LCD 1", to remove all LCD actions.
````

Arduino Due [pinouts graphic](https://components101.com/sites/default/files/component_pin/Arduino-Due-Pinout.png),
[info](https://components101.com/microcontrollers/arduino-due)

--------------------------------------------------------------------------------
### Desktop Hardware To Do List
````
  ------------
  + Connect and test an ESP8266 NodeMCU for internet and SMS access.
  + Mount, connect, and test an LCD, such as a 1602 LCD.
  ------------
  Design and wire a central power source:
  + Add internal 120AC socket for 3 devices: 1) Mega 5V wall adapter, 2) MP3 5V wall adapter, 3) stearo amp plug,
  + Use the Mega to control an On/off relay switch for 120AC socket.
  + Use the Mega to control an On/off relay switch for the stearo amp's 120AC adapter.
  + Later, add the stearo amp power supply inside the case.
  ------------
  + Done: Cut a glue Spider-Man paper to panels: 2 sides, bar top, and top panel.
  + Done: Cut separation on the top for easy internal access.
  + Done: Install, wire, and test the front panel wit the Mega.
  + Done: Panel LED lights all display correctly.
  + Done: Toggle functions all work for Altair 8800 emulation.
  + Done: Test new serial module using the tablet. Then install it in the box.
  + Done: Panel LED lights and toggle functions all work for the clock.
  + Done: Panel LED lights and toggle functions all work for the MP3 player.
  + Done: Wire up the MP3 player using separate USB wall plug to reduce static noise.
  + Done: Add RCA female plugs for L/R external MP3 player output to an amp.
````
--------------------------------------------------------------------------------
### Connect the Mega to the Desktop Front Panel
````
  Total = 14 Mega pin wire connections to the front panel, pluse 6 for power.

  Non-power pins:
  ---
  + 2 pins for the WAIT and HLDA LED lights.
  + 3 74HC595 daisy chain, LED output: data, clock, and latch
  ---
  + 2 I2C: PCF8574 daisy chain, toggle inputs
  + 1 Interrupt pin for the PCF8574 modules
  ---
  + 3 SPI: SD Card module pins
  + 1 Control pin (CS) for SPI SD Card module
  ---
  + 1 TX to MP3 player module
  + 1 RX to MP3 player module
  ---

  ---------------------------------------------
  Mega to Front Panel details

  2 Mega power pins to the front panel breadboard,
  + 1 wire for +5V
  + 1 wire for Ground

  2 Mega pins for the WAIT and HLDA LED lights.
  + Pin A9 for WAIT_PIN
  + Pin A10 for HLDA_PIN

  3 Mega pins to 74HC595 daisy chain, LED output,
  + Mega pin A14 to 74HC595 14 Data pin.
  + Mega pin A12 to 74HC595 12 Latch pin.
  + Mega pin A11 to 74HC595 11 Clock pin.

  5 Mega pins to PCF8574 daisy chain, toggle inputs,
  + I2C SDA to Mega SDA
  + I2C SCL to Mega SCL
  --- Plus ---
  + Pin 2 to the front panel breadboard for wires to Control and AUX interrupts.
  --- Plus ---
  + I2C +5V
  + I2C Ground

  ---------------------------------------------
  2 Mega pins to connect to the MP3 player,
  + Mega pin 18(RX) to player pin 3(TX)
  + Mega pin 19(TX) to external resister to player pin 2(RX)
  --- Plus ---
  + Independent separate power supply: +5V and ground.

  ESP8266 ESP-12E NodeMCU cannot replace a Mega because not enough GPIO pins.
````

--------------------------------------------------------------------------------
### Parts List

Includes notes for the difference between the tablet and the desktop models.
````
Tablet:
+ Clip board
+ 3 x breadboards

Desktop:
+ Front panel, sticker, case
+ 1 full sized breadboard for the player
+ 1 half sized breadboard for the front panel

+ Arduino Mega ATmega2560, CH340G
+ 4 x Shift Register SN74HC595N, 20pcs on sale for $2
+ 34 x Red LED 5mm, 100pcs for $1.50
+ 34 x 5k resistors for LED lights.
+ Tablet: 1 x LED bar graph display in place of 8 red LED lights.

+ Desktop: 4 x PCF8574 I2C modules for the switches.
+ 8 x SPDT On/Off/On 3 Position Momentary toggles, Diameter: 6mm (0.2inch), Micro mini, 10pcs for $6.79
+ 16 x SPDT On/Off 2 Position Mini Toggle Switch, Diameter: 6mm (0.2inch), 10Pcs for $2.68.
+ Tablet: 2 x PCF8574 I2C modules for the switches, parallel input to serial output.
+ 8 x SPDT On/Off 2 Position Mini Toggle Switch, Diameter: 6mm (0.2inch), 10Pcs for $2.68.
+ Small panel board to hold the toggles.

+ DS3231 clock module
+ 1602 LCD with serial module
+ Micro SD Card SPI module
+ USB serial component
+ USB mini hub
+ DFPlayer module

Tablet:
+ 9V battery case with on/off switch
+ 10 x Machine screws and 13 x nuts, size #4 x 3/4".
+ Mounting tape, 2 sided.
+ Felt padding
------

Cables,

1 x 40pcs/pack 20cm Female to Female Ribbon Breadboard Cable
+ 8x, Between PCF8574 and toggles.

1 x 40pcs/pack 20cm Male to Female Ribbon Breadboard Cable
+ 4x, Between USB serial module and breadboard.
+ 4x, Between 1602 LED serial module and breadboard.

1 x 40pcs/pack 20cm Male to Male Ribbon Breadboard Cable
+ 16x, Between SN74HC595N chips and breadboard (to LED lights).
+ 2x, Between breadboard to breadboard (to clock module).
+ 2x, Between breadboard to breadboard (I2C on the Mega).
+ 2x, Between breadboard to breadboard (USB to Mega).

1 x 40pcs/pack 10cm Female to Female Ribbon Breadboard Cable
+ 8x, Between PCF8574 and toggles.

1 x 40pcs/pack 10cm Male to Female Ribbon Breadboard Cable
+ 6 for SD card module
+ 4x, Between PCF8574 and breadboard.

1 x 40pcs/pack 10cm Male to Male Ribbon Breadboard Cable
+ 12x, for various breadboard connections, such as 5V and ground between Mega and breadboards.

+ Box of breadboard wires.
````
------------------------------------
#### Costs
````
Tablet module,
+ Clipboard, $5
+ Arduino Mega, $15
+ Toggles: on/off/on momentary toggles + on/off toggles + PCF8574 I2C module = $5 + 5 + $6 = $11
+ LED light display: Red LEDs + 20pcs 74HC595 + 5K resisters = $1 + $3 + $2 = $7
+ Modules: USB serial, SD card, 1602 LED, clock + battery, USB mini hub = 5 x $3 = $15
+ Breadboards + cables + battery case + battery + tape + felt = $6 + $8 + $2 + $2 + $6 = $24
+ Total, about  $75 =  $5 + $70, $70 = $15 + $11 + $7 + $15 + $22

Desktop model,
+ Front panel, sticker, case, and shipping = $18.00 + $5 + $35 + $16 = $74
+ Total, about $144 = $74 + $70, $70 = $15 + $11 + $7 + $15 + $22
````
--------------------------------------------------------------------------------
## Build History

Your mobile phone has a touch screen display. Your computer has a keyboard(input) and monitor(output).
In 1975, the Altair 8800, the first available popular computer only had toggles(input) and lights(output).
It launched the home computer revolution.
I wanted one, I want one, so I designed the Altair 101 to have the same front panel of toggles and LED lights.

My hardware design centers around my [Altair 8800 clone front panel](https://www.adwaterandstir.com/product/front-panel/).

[<img width="360px"  src="../FrontPanel01a.jpg"/>](https://www.adwaterandstir.com/product/front-panel/)

I completed my goal was to design and build a machine to emulate the base Altair 8800 machine.
The emulator also has modern components such as a micro SD card module for storage and a 32 character LCD.
I kept the cost low, the final machine cost is about $149.
I kept soldering to a minimum. The only soldering is wire connectors for the toggle switches.
The desktop version will require basic soldering of the LED lights to ground.

I watched Dan Eater's videos on
[how to Build an 8-bit computer from scratch](https://www.youtube.com/watch?v=HyznrdDSSGM&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=14)
And so, where other emulator designs used a printed PCB board, I used breadboards.

Most clones, replicas, use an Arduino Due.
I started with an Arduino Nano, but due to increasing memory requirements, I moved to the Mega 2560 (ATmega2560).

I have completed a development machine, it's refined, tuned, and tested.
The components are mounted on a clipboard which makes it a steampunk tablet.
The desktop model design will be based on the tablet machine.
The major difference between the machines, is that the tablet model has 8 address toggles where the desktop model will have 16 address toggles.

--------------------------------------------------------------------------------
Cheers