--------------------------------------------------------------------------------
# Altair 101 Hardware Notes

````
  ---------------------------------------------
  Desktop Box:
  ------------
  To do:
  + Mount, connect, and test an LCD, such as a 1602 LCD.
  + Connect and test an ESP8266 NodeMCU for internet and SMS access.
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

--------------------------------------------------------------------------------
  Connect the Mega to the desktop front panel:

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