--------------------------------------------------------------------------------
# Arduino Notes

--------------------------------------------------------------------------------
## Program: lcd1602serial.ino

I'm testing the program to have it count seconds, as best I can with out a clock, such as a DS3231.
Each loop, the program runs delay(993), which is close to having a 1 second loop, with logic and displaying the counter to the LCD.
Then I use an offset amount, such as 1030, every 500 seconds to re-sync to actual time.
I am trying various sync delay values to get better accuracy.
````
delay(1060) = -13 seconds in 3 hours (about)
delay(1030) = -08 seconds in 3 hours
delay(1000) = -06 seconds in 3 hours (about, estimated)
delay( 820) = -04 seconds in 3 hours
delay(820);
07:35 =   0
10:35 =  -4 sec, 10796 counter seconds in 3 hours, 10800 actual
          4*3*2 = 24 seconds a day, slower. Off one minute every 2 days.
delay(700);
10:50 =   0
12:50 =   0 sec, in 2 hours, actual:  7200 seconds.
03:50 =  -8 sec, in 4 hours, actual: 14400 seconds, counted: 14392.
         -2 seconds an hour.

delay(600);
09:03 =   0
12:03 =   0 sec, in 3 hours, 10800  seconds, counted: 10800. Actually, less then a second behind.
04:03 = -20 sec, in 7 hours, 25200  seconds, counted: 25180. Actually, less then a second behind.
06:03 = -32 sec, in 9 hours, 32400  seconds, counted: 32368. Actually, less then a second behind.

12:00 =   0

delay(500);
03:09 =   0
05:09 =  +3 sec, in 2 hours, actual:  7200 seconds, counted: 7204.

3600seconds/hour. 2/3600 = .0006, losing 6 milliseconds/second
````

You could make the warning go away by deleting the version of the library you currently have installed and then installing version 1.0.0:
https://github.com/NorthernWidget/DS3231/archive/1.0.0.zip

--------------------------------------------------------------------------------
### Useful components

+ Arduino Nano 3.0 controller, CH340 USB driver
+ ESP8266 ESP-12E NodeMcu Lua V3 CH340G WIFI Lua Wireless Development Board Module

+ LCE 1602 16x2 Serial HD44780 Character LCD Board Display with White on Blue Backlight 5V with IIC/I2C Serial Interface Adapter
+ Infrared IR Wireless Remote Control Module Kits: Keyestudio 38KHz, IR receiver and small remote controller
+ 4 x 4 16 Key Matrix Membrane Switch Keypad
+ 10 Segment LED Bargraph Light Display
+ DS3231: DC 3.3-5.5V, High Precision Real-Time Clock Module, $1.29

MP3 player components:
https://secure.newegg.com/Wishlist/MyWishlistDetail?ID=8188917
+ YDA138-E Audio Amplifier Board 8W+8W 12W+12W Dual Mode HIFI Dual Channel Stereo Digital Amplifier Board DC12V For YAMAHA    $4.99
+ TRIXES Electronics Solderless 400 Point Prototyping Breadboard, $2.39
+ DFPlayer Mini MP3 Player Module, $2.78

+ 3.3V DC 800mA from 4.75V-12V, Voltage Regulator: AMS1117-3.3V
+ 5V DC from 6.0V-12V, Voltage Regulator: AMS1117-5.0V, 2 pins in, 2 pins out voltage
+ 5V power supply module: AMS1117 with DC seat and switch
+ Battery: CR2032, used in the DS3231 clock

#### Sound interesting
+ 3V-5V power model NEO-6M GPS module GY-GPS6MV2 (Blue)
+ LD1117V33 Linear Voltage Regulator 3.3V

#### Parts on Amazon.com
+ $03.54 Nano:
https://www.amazon.com/SODIAL-ATmega328-P-20AU-Module-Board/dp/B00QPUEFNW/ref=sr_1_fkmrnull_1?keywords=SODIAL+nano&qid=1554232072&s=gateway&sr=8-1-fkmrnull
+ $01.66 DFPlayer MP3
https://www.amazon.com/SODIAL-DFPlayer-Player-Module-Arduino/dp/B076BD15MR/ref=sr_1_fkmrnull_1?keywords=SODIAL+DFPlayer+MP3&qid=1554232133&s=gateway&sr=8-1-fkmrnull
+ $02.31 Infrared-Receiver, Pack of 5
https://www.amazon.com/SODIAL-Infrared-Receiver-Wireless-Communication/dp/B077D1DR11/ref=sr_1_fkmrnull_5?keywords=SODIAL+Infrared+IR+Wireless+Remote+Control&qid=1554232355&s=gateway&sr=8-5-fkmrnull
+ $10.96 Yamaha amp
https://www.amazon.com/TOOGOO-YDA138-Amplifier-Channel-Digital/dp/B07MDXK5L3/ref=sr_1_fkmr0_1?keywords=SODIAL+YDA138-E+Audio+Amplifier&qid=1554232178&s=gateway&sr=8-1-fkmr0
+ $02.01 Solderless Breadboard Bread Board 400 Tie Points Contacts
https://www.amazon.com/Solderless-Breadboard-Bread-Points-Contacts/dp/B00JGFDKBQ/ref=sr_1_8?keywords=SODIAL+breadboard&qid=1554232722&s=gateway&sr=8-8
+ $01.51 Male to female, 20cm Long F/F Solderless Flexible Breadboard Jumper Cable Wire 40 Pcs 
https://www.amazon.com/SODIAL-Solderless-Flexible-Breadboard-Jumper/dp/B00U8MK14K/ref=sr_1_7?keywords=SODIAL+jumper+cable+breadboard&qid=1554232815&s=gateway&sr=8-7
+ $2.66 140pcs U Shape Solderless Breadboard Jumper Cable Wire Kit for Arduino Shield 
https://www.amazon.com/SODIAL-140pcs-Solderless-Breadboard-Arduino/dp/B00K67XXSI/ref=sr_1_10?keywords=SODIAL+jumper+cable+breadboard&qid=1554232815&s=gateway&sr=8-10

--------------------------------------------------------------------------------
## Clock

+ Clock timer: battery powered clock, LCD 1602, control pins
+ DS3231: DC 3.3-5.5V, High Precision Real-Time Clock Module, $1.29
+ DS3231 use battery: CR2032, which should last for over a year

i²c (I2C or i2c) bus: SDA and SCL

Pins:
+ VCC to 3.3-5.5V
+ GND to ground
+ SDA to D4 (pin 4) on Uno and Nano
+ SCL to D5 (pin 5) on Uno and Nano

+ Clock using LCD 1602 keypad shield (left, right, up, down, select).
http://ediy.com.my/projects/item/138-arduino-based-lcd-digital-clock
https://youtu.be/Hp1u-akidpM

#### Fix warning message:
```
"WARNING: Category 'Real-time clock' in library DS3231 is not valid. Setting to 'Uncategorized'"
````
You could make the warning go away by deleting the version of the library you currently have installed and then installing version 1.0.0:

https://github.com/NorthernWidget/DS3231/archive/1.0.0.zip

Reference: https://forum.arduino.cc/index.php?topic=477214.0

--------------------------------------------------------------------------------
## 1602 LCD Keypad Shield with an Arduino Uno R3

Program examples:
https://create.arduino.cc/projecthub/electropeak/using-1602-lcd-keypad-shield-w-arduino-w-examples-e02d95

--------------------------------------------------------------------------------
## Use Arduino I2C Serial LCD 16x2

+ Hello World
https://www.instructables.com/id/How-to-Use-Arduino-I2C-Serial-LCD-16x2-Yellow-Back/
````
SDA - A4
SCL - A5
````
--------------------------------------------------------------------------------
## ESP8266 ESP-12E NodeMcu

Adding infrared receiver:
https://www.instructables.com/id/Universal-Remote-Using-ESP8266Wifi-Controlled/
+ Library: IRremoteESP8266?
+ Power on right side, G and 3V.
+ D4 or D2 for receiver. Sample uses: TSOP1738 IR reciever or some other IR recievers.
+ The article also shows how to send infrared signals.

+ Video I used to get it working:
https://www.youtube.com/watch?v=p06NNRq5NTU
++ I added ESP8266 boards as an option in the IDE.
++ I uploaded and ran the basic blink program.

Version I bought:
+ ESP8266 ESP-12E NodeMcu Lua V3 CH340G WIFI Lua Wireless Development Board Module

A suggested voltage regulator:
+ LD1117V33 Linear Voltage Regulator 3.3V

+ Webserver project
https://lastminuteengineers.com/creating-esp8266-web-server-arduino-ide/

---------------
+ I used Boards Manger, search for ESP and installed: ESP8266 Community version2.5.2.
+ Alternative that didn't work: Preference, 
URL to JSON file for adding in the IDE:
http://arduino.esp8266.com/stable/package_esp8266com_index.json

+ Search: ESP8266 ESP-12E CH340
+ Search: ESP8266 ESP-12E CP2102
+ Looks like Arduino UNO uses FTDI USB drivers (FTDI FT232/FT232R that Arduino uses), not CH340 nor CP2102.

LoLin version uses USB CH340, which same as my Arduino Nano chips
CH340 just fine without needing a driver
Amica v.2 and DOIT v.3 use CP2102.

Producers: Amica (see ‘NodeMCU and Amica‘ below), DOIT/SmartArduino, and LoLin/WeMos.
````
A.) SiliconLabs CP2102 Drivers for Windows 7/8/8.1
https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx#windows
CP2102 USB-to-TTL (Drivers for Mac, Windows,
http://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver.zip
http://www.silabs.com/Support%20Documents/Software/CP210x_VCP_Windows.zip
````
B.) WCH CH340G Drivers for Windows 7/8/8.1
http://www.arduined.eu/ch340g-converter-windows-7-driver-download/

Also, don’t be put off if the name of the driver file is “CHG341SER.zip”, as the ‘341 is fully software compatible to the ‘340. 
So, go ahead and use the ‘341 version of the drivers. 
I use them on my LoLin, WeMOs D1 & D1/R2 boards as well as for my Arduino Nano boards (those cheaper Chinese clones).

+ Search: infrared remote control kit
https://www.ebay.com/itm/New-Infrared-IR-Wireless-Remote-Control-Module-Kits-for-Arduino-Industrial-Tool/233096413655?hash=item3645a101d7:g:b5sAAOSwh8NbSY9U

hotodiode + Thermistor + Sound Detection Sensor Kit for Arduino
Humidity  Temperature Barometric Pressure Sensor Module BME280

wall plug 5v power adaptor
https://www.ebay.com/itm/5V-1A-Mini-USB-Home-Travel-Wall-Charger-Power-Charging-Adaptor-US-Plug-Hot-Sale/112542999293?hash=item1a3415e2fd%3Ag%3ArCwAAOSwXf9Zo8Mh&LH_BIN=1

Texas Instruments 74LS47 7447 SN74HC47N IC BCD to Seven-Segment Decoder/Driver

ENC28J60 spi interface Ethernet network module

sim800l gprs gsm module micro-sim core board quad-band for arduino
https://www.ebay.com/itm/SIM800L-gprs-gsm-module-micro-sim-core-board-quad-band-for-arduinor-K5H1/263370283725?epid=26010975359&hash=item3d52179acd:g:iZIAAOSwEaBaKC03


--------------------------------------------------------------------------------
## Twilio SMS with using an ESP8266

+ To order:
+ Cable: SSD to USB so that I can read my old computers files.
+ DF Player and Yamaha amp. For my room.
+ 3 NodeMCU
+ 3 infrared receivers

--------------------------------------------------------------------------------
## Set up on my MacBook:

In the IDE menu, select:
+ Tools/Boards: Arduino Nano
+ Processor: ATmega328P (Old Bootloader)
+ Port: /dev/cu.wchusbserial14120

--------------------------------------------------------------------------------
## Projects

### Working

+ 120v plugin controller: Arduino, relay, electrical supplies from Home Depot

+ MP3 player: Arduino, DFPlayer MP3 Player, Yamaha Audio Amplifier, infrared keypad
https://github.com/tigerfarm/arduino/tree/master/samples/mp3v2
+ Without infrared
https://github.com/tigerfarm/arduino/tree/master/samples/mp3v1

+ Pong: Arduino, Potentiometer (10k or 1k?), RCA video cable
+ Need to adjust edge bounce: right and bottom
https://github.com/tigerfarm/arduino/tree/master/samples/pongv2

+ Infrared Wireless Remote Control Kit for Arduino DIY Project
https://tutorial45.com/arduino-ir-receiver/
+ Receiver has 3 pins: ground(-), +5v, control (S).
https://github.com/tigerfarm/arduino/tree/master/samples/InfraredRead
+ 1 x ($1.08) SODIAL Infrared Wireless Remote Control Kit for Arduino DIY Project $1.08

+ Keypad
+ 1 x ($1.00) 4 x 4 16 Key Matrix Membrane Switch Keypad $1.00
http://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
https://github.com/tigerfarm/arduino/tree/master/samples/keypadv1

-------------
### Other Projects to Do

+ Example use: turn off and on lights using the 120v plugin controller

+ Weather report
+ Use the LCD 1602 with Arduino.
+ Environment monitor: temp, humidity, pressure

+ Card reader to store and read state
+ Bluetooth update to the old AM radio

-------------
### Complicated project: LED Clock

+ Parts:
++ Arduino Nano, 2 x bar graph LEDs, 2 x 7 Segment LED, chips
+ Hours counter
++ 12 hours using 2 x bar graph LEDs, 4011 CMOS chip
++ Debounce button to set the hour.
https://www.arduino.cc/en/Tutorial/Debounce
+ Minutes counter
++ 00-59 minutes, in binary, bar graph LEDs
++ 00-59 minutes, using 2 x 7 Segment LED(0-5 & 0-9)
++ Debounce button to set the minutes.
+ Keep the time when powered down
++ Add battery powered clock timer to set the clock automatically
++ Or WiFi option to set the clock automatically

-------------
### Other

+ Ethernet:
++ Arduino Compatible ENC28J60 Ethernet LAN Network Module for STM32 / 51 / AVR / LPC
https://create.arduino.cc/projecthub/Sourcery/how-to-connect-the-enc28j60-to-an-arduino-efd0dd
https://www.youtube.com/watch?v=M4mVDnlnzSA
+ Server
https://www.tweaking4all.com/hardware/arduino/arduino-enc28j60-ethernet/
+ Client
https://www.tweaking4all.com/hardware/arduino/arduino-ethernet-data-push/

+ RGB 4 pin LED:
http://educ8s.tv/arduino-rgb-led-tutorial/
https://www.youtube.com/watch?v=5Qi93MjlqzE

--------------------------------------------------------------------------------
## Weather project

+ Arduino Nano with BME280 for Temperature, Humidity and Pressure

+ BME280 6 pins labels (pins up):
+ VCC GND|SCK SCL|SDA SDI|CSB SDO

Likely pin actual:
+ 3.3V GND SCK/SCL SDA/SDI CSB SDO
+ Which means:
+ 3.3V GND SCL SDA CSB SDO
+ Or, depending on the diagram online
+ 3.3V GND SCK SDI CSB SDO

+ BME280 pins down, and following: https://www.youtube.com/watch?v=PidE-pJiIXY
````
---------------
| 1 3.3V      | To Arduino Nano, pin 3.3V
| 2 GND       | To Arduino Nano, pin GND
| 3 SCK/SCL   | To Arduino Nano, pin A5
| 4 SDA/SDI   | To Arduino Nano, pin A4
| 5 CSB       | Not used.
| 6 SDO       | Not used.
---------------
````

Library:
+ Grove - Barometer Senor BME280 by Seed Studio, version 1.02

+ Arduino Nano with BME280 for Temperature, Humidity and Pressure
https://www.instructables.com/id/Arduino-Nano-and-Visuino-Display-BME280-Temperatur/
+ Pressure: BME280
1 x ($1.02) SODIAL Breakout Humidity/Digital Temperature/Barometric Pressure Sensor Module BME280

https://www.instructables.com/id/Arduino-Easy-Weather-Station-With-BME280-Sensor/

--------------------------------------------------------------------------------
## Information and sample project links

+ Arduino Nano with BME280 for Temperature Humidity and Pressure
https://www.instructables.com/id/Arduino-Nano-and-Visuino-Display-BME280-Temperatur/
+ Pressure: BME280
1 x ($1.02) SODIAL Breakout Humidity/Digital Temperature/Barometric Pressure Sensor Module BME280
$1.02
https://www.youtube.com/watch?v=PidE-pJiIXY
+ BMP180 sensor: Read pressure, temperature, and altitude
https://www.youtube.com/watch?v=i1LQaQmFutE
+ BMP180 Barometric Pressure Sensor on an Arduino
http://www.circuitbasics.com/set-bmp180-barometric-pressure-sensor-arduino/
+ Humidity sensor
https://www.youtube.com/watch?v=OogldLc9uYc

-------------
+ I2C with 1602 LCD Display Screen (blue) Board Module for Arduino - BLUE 134427401
https://www.gearbest.com/other-accessories/pp_216639.html?wid=1433363&currency=USD&vip=14891995&gclid=EAIaIQobChMI8cGxuv_M3QIVVluGCh2RuQUAEAQYAiABEgJHE_D_BwE

+ 1602 16x2 Character LCD Display Module
https://www.youtube.com/watch?v=dZZynJLmTn8

-------------
+ Read/Write to SD card
https://www.youtube.com/watch?v=8MvRRNYxy9c

+ Set Up a Keypad on an Arduino
http://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/

-------------
+ Bluetooth
https://www.youtube.com/watch?v=sXs7S048eIo

+ Infra red transmitter/receiver project:
https://youtu.be/2n13NzbhTMI

+ Relay control of 120 ACV
http://www.circuitbasics.com/setting-up-a-5v-relay-on-the-arduino/
+ Relay control of 120 ACV, with thermistor (temperature sensor)
http://www.circuitbasics.com/wp-content/uploads/2015/12/Arduino-Temperature-Sensitive-Relay-Switch.jpg
+ Turn Any Appliance into a Smart Device with an Arduino Controlled Power Outlet
http://www.circuitbasics.com/build-an-arduino-controlled-power-outlet/

--------------------------------------------------------------------------------
+ SDA and SCL on Arduino Nano, are A4 and A5.
++ Use SDA is pin A4 and SCL is pin A5.
++ In the program, #include <Wire.h>
https://www.lehelmatyus.com/691/sda-scl-arduino-nano-connecting-i2c-devices-arduino-nano

+ How to Program IC Atmega328p using Arduino UNO:
https://www.youtube.com/watch?v=35hB-sn0ddI
+ How to program a Arduino ATmega328 chip: Program Standalone BreadBoard Arduino of ATmega328, using custom board defination
https://www.youtube.com/watch?v=CzY0dB2wfTw
+ Chip: 556-ATMEGA328-PU 
https://www.mouser.com/ProductDetail/Microchip-Technology-Atmel/ATMEGA328-PU?qs=lwdSMh1%2FoYJT8tfItQpQtA%3D%3D&gclid=Cj0KCQjwlqLdBRCKARIsAPxTGaVAng66TAi8ZMJGaHWAWSangJ8xoNZytGaFpu8iEvosSVPFPZo3bwkaAv9JEALw_wcB

--------------------------------------------------------------------------------
## MP3 Player

+ Works.
+ I rewrote the control program.

https://www.youtube.com/watch?v=ZwBmx8fpXCY
+ Arduino
+ 5 buttons
+ MDFPlayer chip
+ resister
+ speaker: mono

Arduino Project: MP3 player using Arduino and DFPlayer mini MP3 player module from banggood.com
https://www.youtube.com/watch?v=UodfePdNfg8

+ About the DFPlayer
https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299

+ Mini MP3 Player Module with Simplified Output Speaker for Arduino UNO 
https://www.newegg.com/Product/Product.aspx?Item=9SIADG45WH6777&cm_re=arduino_uno-_-9SIADG45WH6777-_-Product

https://www.instructables.com/id/MP3-Player-With-Arduino/

--------------------------------------------------------------------------------
## Pong

Works.
+ Re-organized the control program and added features:
+ Game pause
+ When game over, press button to start a new game.

https://www.instructables.com/id/Arduino-Pong-4/
https://www.makeuseof.com/tag/how-to-recreate-the-classic-pong-game-using-arduino/
+ Code:
https://github.com/Avamander/arduino-tvout
https://code.google.com/archive/p/arduino-tvout/downloads
+ How to make player controls
http://uczymy.edu.pl/wp/blog/2016/09/08/gra-video-arduino/

+ Pong on an 8x8 dot matrix:
https://randomnerdtutorials.com/guide-for-8x8-dot-matrix-max7219-with-arduino-pong-game/

-----------------------------
+ From comments on https://www.makeuseof.com/tag/google-chrome-multitasking-extensions/

````
Things to do on your computer / software:

S 0) If you have the Arduino IDE running: exit it.
S 1) Download "TVoutBeta1.zip" from https://code.google.com/p/arduino-tvout/downloads/list
S 2) Copy all three directories contained in "TVoutBeta1.zip" to the "\libraries" directory of your Arduino IDE (in my case: c:\arduino-1.0.1\libraries).
After this, there should be the three new directories and several more.
S 3) Start the Arduino IDE and check if you have the menu-entry "File > Examples > tvout". If not, restart the IDE and check if you put everything in the right place.

Things to do with hardware/your Arduino:

TV-Connection
H 1) Take your RCA plug/Cinch-Cable and strip the outer rubber layer away. Gather up the shielding, twist and put away to one side. This is your ground (GND).
H 2) Strip the plastic/rubber from the inner cable as well. This is your SIGNAL.
H 3) Solder BOTH the 470 ohm and 1k ohm resistors to SIGNAL.
H 4) The 470 ohm (Yellow|Purple|Brown) connects to Digital D07 on the Arduino
H 5) The 1k ohm (Brown|Black|Red) connects to Digital D09 on the Arduino
H 6) The shielding (GND) connects to a ground/GND on the Arduino
H 7) Load the examples (from S 3 above) into your Arduino, connect the RCA/Cinch Cable to your TV-Video-In (yellow connector on your TV-set) and add power - you should see some lovely output on your TV.

Paddles
H 8) Connect the center pin of one 10k ohm potentiometer to Analog A0 on the Arduino and the center pin of the other potentiometer to Analog A1 on the Arduino.
H 9) Connect one of the outer pins of each 10k ohm potentiometer to GND on the Arduino.

Startbutton
H 10) Startbutton: connect one of the two pins of the pushbutton to +5V on the Arduino, connect the other pin to Digital D02 on the Arduino AND to a 10k ohm resistor.
H 11) Connect the other end of the 10k ohm resistor to ground/GND on the Arduino.
(https://www.arduino.cc/en/uploads/Tutorial/button_schem.png)

S 4) Start your Arduino IDE and copy the sourcecode from Pastebin to the workspace, compile and upload everything to the Arduino. Connect the Arduino to your tv and you're ready play!
````

----------
A friend and I figured out how to adjust the ball speed without glitching the game.
if(frame % 3 == 0) adjust the three there to a two or even one to make the game crisper and faster.
for example: if(frame % 1 == 0)

--------------------------------------------------------------------------------

Consider the code to blink the LED from the first activity.
We used the digitalWrite()function to turn the the LED HIGH or LOW.
Because we would like to display a range of colors, this system of writing to the LED is no longer sufficient.
Instead we use the analogWrite() function which takes a number [0,255] instead of HIGH or LOW.
An example of this is: analogWrite(ledPin, 126);.
In the case of the LED, these numbers correspond to the brightness of a diode (red, blue or green).
With analogWrite()we therefore can write 256^3 = 16777216 different colors to the LED instead of the 2^3 = 8 available with digitalWrite()'s HIGH and LOW system. 

--------------------------------------------------------------------------------

Arduino Send Data to Google Docs (Spreadsheet)
https://www.youtube.com/watch?v=MFF0mdl_zBU

Google Sheets API - HTML Form Data to Google Sheet
https://www.youtube.com/watch?v=BxqfwfQi0jk

Reading & Writing Cell Values 
https://developers.google.com/sheets/api/guides/values

Google Apps Script Quickstart 
https://developers.google.com/sheets/api/quickstart/apps-script

+ FM Transmitter, $4 version
https://www.instructables.com/id/Build-Your-FM-transmission-Station-with-Arduino/
+ Or $19.95
https://www.youtube.com/watch?v=6HArlDIf-NI
https://www.adafruit.com/product/1958?gclid=EAIaIQobChMIo6-txJz63gIVilmGCh0rjwVgEAkYByABEgKZw_D_BwE

--------------------------------------------------------------------------------
## Fritzing for creating Network Diagrams

Steps:
+ Download and install.
+ Once started, update the components and restart.

Start Fritzing. Click the Breadboard tab. This is where diagrams are created.
+ Video I briefed over to get started:
https://www.youtube.com/watch?v=HNHu0jtS0cs
+ How to create a circuit diagram starts at 3:20.

--------------------------------------------------------------------------------
#### Other hardware devices

+ Particle Electron
https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-particle-electron
https://www.youtube.com/watch?v=D4qfer8kLc8
https://www.youtube.com/watch?v=T60GEfMFwIY

+ 433MHz Wireless Antenna for CC1101+PA+LNA A710 CC1100+PA Arduino Mega UNO R3 
https://www.newegg.com/Product/Product.aspx?Item=9SIA86V2Z33928&cm_re=ardiuno_uno-_-9SIA86V2Z33928-_-Product

Arduino Tutorial: ATTiny85 board A First Look and review.
https://www.youtube.com/watch?v=fOBGdSYi318
+ ATtiny85 is a microcontroller, it requires a Arduino Uno/Nana to program it.
+ Nana and ATtiny
https://www.youtube.com/watch?v=vH5sx8qphdk
+ Programming ATtiny85 with Arduino Uno - Breadboard
https://create.arduino.cc/projecthub/arjun/programming-attiny85-with-arduino-uno-afb829

--------------------------------------------------------------------------------
#### Build an Arduino UNO
https://www.youtube.com/watch?v=sNIMCdVOHOM

Nano V3 vs Arduino UNO:
+ Nano lacks a DC power jack
+ Works with a Mini-B USB cable instead of a standard one.
+ For Uno SDA use Nano pin A4, and Uno SCL is Nano pin A5.
https://components101.com/microcontrollers/arduino-nano

+ About various Arduino versions:
https://blog.hackster.io/picking-the-right-arduino-341a0a9550c7

-----------------------
### Arduino Nano

+ Nano Pin Vin: Input voltage to Arduino when using an external power source (6-12V).
+ Pin out description:
http://www.circuitstoday.com/arduino-nano-tutorial-pinout-schematics

+ Use the IDE on a Mac
https://medium.com/@thuc/connect-arduino-nano-with-mac-osx-f922a46c0a5d

--------------------------------------------------------------------------------
## Altair 8800

+ How to build one.
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6
+ Build your own.
https://spectrum.ieee.org/geek-life/hands-on/build-your-own-altair-8800-personal-computer

+ Assembled & Tested, $279.95 – $349.95
https://www.adwaterandstir.com/product/altair-assembled/

+ Uses the Arduino Due which is an Arduino for more powerful larger scale projects.
https://store.arduino.cc/usa/due
++ The Arduino Due is the first Arduino board based on a 32-bit ARM core microcontroller.
++ 54 digital input/output pins, 12 analog inputs, a 84 MHz clock

+ Intro to the Arduino Due
https://www.theengineeringprojects.com/2018/09/introduction-to-arduino-due.html

+ Code repository
https://github.com/dhansel/Altair8800

+ Altair 8800 Instructional Videos
https://www.youtube.com/playlist?list=PLB3mwSROoJ4KLWM8KwK0cD1dhX35wILBj

Google forums:
https://groups.google.com/forum/#!forum/altair-duino

+ Manual
https://grantmestrength.github.io/RetroComputerInstructionManual/
https://github.com/GrantMeStrength/RetroComputerInstructionManual

--------------------------------------------------------------------------------
+ Excellent Arduino overview:
https://www.youtube.com/watch?v=nL34zDTPkcs

+ Twilio, Send SMS and MMS Messages From the ESP8266 in C++ 
https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp
++ On GitHub
https://github.com/TwilioDevEd/twilio_esp8266_arduino_example

+ Standard Libraries
https://www.arduino.cc/en/reference/libraries
++ WiFi
https://www.arduino.cc/en/Reference/WiFi
````
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
https://github.com/witnessmenow/HTTPS-directly-from-ESP8266/blob/master/implentation/implentation.ino
char host[] = "api.reddit.com";
if (client.connect(host, 443)) {
    Serial.println("connected");
...
}
````
https://www.youtube.com/watch?v=hMZdM0Wlt54

--------------------------------------------------------------------------------
## Twilio SMS with using an ESP8266

Send SMS and MMS Messages From the ESP8266 in C++ 
https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp
+ On GitHub
https://github.com/TwilioDevEd/twilio_esp8266_arduino_example

Includes code to connect to a WiFi network and verify the SHA1 signature of api.twilio.com.

How to receive an SMS:
https://www.twilio.com/docs/sms/tutorials/how-to-receive-and-reply

Controlling a light on/off/status, using esp8266.
https://www.youtube.com/watch?v=0U2Hqux1b88

--------------------------------------------------------------------------------
## Chips
````
CMOS: HEF4511BP BCD to 7-segment latch/decoder/driver

TA7205AP NTE Equvilent NTE1155 5.8W integrated audio power amplifier.

TTL:  LM556 Dual timing circuit is a highly stable controller capable of producing accurate time delays or oscillation.
TTL:  DM7404. Hex Inverting Gates

CMOS: MC14049B Hex Inverter/Buffer
CMOS: LM339N Comparator Differential
CMOS: MC14011BCP Gates 3-18V Quad NAND 2-Input 
CMOS: HEF4011BP is a quad two input NAND gate
CMOS: MC14049BCP ON Semiconductor Buffers & Line Drivers 3-18V Hex CMOS

CMOS: LM386 Low Voltage Audio Power Amplifier.
LM741CN Texas Instruments Operational Amplifiers - Op Amps Operational Amplifier
LM1458 and the LM1558 are general purpose dual operational amplifiers. 
BA10324A ROHM Semiconductor Operational Amplifiers - Op Amps

SN7476N Texas Instruments Flip Flops Dual J-K Positive-Edge-Triggered Flip-Flops with Preset and Clear
CD4027BCN Fairchild Flip Flops Dual J-K Flip-Flop
CD4049BCN CMOS Hex Buffer/Converter 

T106C1 SCR
````
--------------------------------------------------------------------------------
### My first order:
````
Sales Order Number:       292111684
Sold by SODIAL
Sales Order Date:              10/4/2018 10:36:09 PM
Shipping Method:               Ship from Overseas(8-15 bus. days)
Estimated to Ship Out On:   10/5/2018 by End of Day

6 x ($3.21) SODIAL Nano V3.0 ATmega328P CH340G 5V 16M MINI USB Nano V3.0 Compatible to Arduino Blue $19.26
1 x ($6.40) SODIAL UNO R3 Development Board microcontroller MEGA328P ATMEGA16U2 Compat for Arduino blue $6.40
--------------------
1 x ($1.02) SODIAL Breakout Humidity/Digital Temperature/Barometric Pressure Sensor Module BME280 $1.02
1 x ($1.00) SPI Reader Micro SD Memory Card TF Memory Card Shield Module for Arduino $1.00
1 x ($1.00) 4 x 4 16 Key Matrix Membrane Switch Keypad $1.00
1 x ($1.29) DC 3.3-5.5V DS3231 High Precision Real-Time Clock Module for MCU $1.29
2 x ($1.06) SODIAL 5V 1 channel H / L Level Trigger Relay Optocoupler Module for Arduino $2.12
1 x ($1.00) DC 5V Coil Relay Module for SCM Development $1.00
--------------------
1 x ($1.08) SODIAL Infrared Wireless Remote Control Kit for Arduino DIY Project $1.08
1 x ($1.00) SODIAL For Arduino Bluetooth Module Serial Board Transceiver Transmitter Receiver $1.00
1 x ($1.00) New HC-SR04 Distance Measuring Transducer Sensor Ultrasonic Module for Arduino $1.00
--------------------
1 x ($6.86) SODIAL 3pcs MB-102 Breadboard 830 Point Solderless Prototype PCB Board Kit for Arduino Proto Shield Distribution Connecting Blocks $6.86
1 x ($4.00) SODIAL 3X Mini 400 Points Prototype PCB Solderless Breadboard Protoboard $4.00
1 x ($2.00) SODIAL 3X MB102 Breadboard Power Supply Module 3.3V 5V for Arduino Bread Board $2.00
1 x ($1.30) SODIAL DC 9V PP3 Battery Holder Box Case Wire Lead ON/OFF Switch Cover + 2.1mm Plug $1.30
--------------------
1 x ($2.00) 1602 16x2 Character LCD Display Module Blue Blacklight $2.00
1 x ($2.26) SODIAL 10 Segment LED Bargraph Light Display (5PCS RED) $2.26
1 x ($2.00) MAX7219 Red Dot Matrix Module MCU Control LED Display DIY w Cable $2.00
1 x ($1.30) 100 Pcs 5mm Red Green White Blue LED Light Emitting Diodes DC 2.5V-3V $1.30
1 x ($1.12) 5 pcs Common Cathode 10 Pin 2 Bit 7 Segment 0.4" Red LED Display Digital Tube $1.12
1 x ($1.22) 10 Pcs 10 Pin 1 Bit 7 Segment 0.5" Red LED Display Digital Tube $1.22
--------------------
2 x ($1.39) SODIAL DFPlayer Mini MP3 Player Module For Arduino Black $2.78
1 x ($0.87) SODIAL PAM8403 mini 5V digital power amplifier board with switch potentiometer can USB power supply $0.87
1 x ($2.59) SODIAL YDA138-E YAMAHA 12W+12W Dual Channel Digital Audio Amplifier Board DC 12V $2.59
2 x ($1.81) SODIAL 2 Pcs 5W 8 Ohm Magnetic Speaker Loudspeaker 70mm x 30mm for TV Player $3.62
--------------------
1 x ($0.36) 10 Pcs 4 Pins Tactile Round Push Button Switch Momentary Tact $0.36
1 x ($1.00) 2Pcs Single Linear Knurled Shaft Volume Rotary Potentiometers 1K Ohm $1.00
4 x ($0.42) SODIAL Potentiometer Single Linear Potentiometer Resistance:10K $1.68
2 x ($0.77) SODIAL Replacement 1.41 inchcap top diameter 6mm shaft hole non-slip potentiometer knob $1.54
1 x ($3.00) SODIAL 1% Precision 1 / 4W 30 Values ??Metal Resistor Resistance Kit, Set of 600 pcs $3.00
1 x ($2.00) New 5 Pcs AC 250V/3A 120V/6A On-Off 2 Position Spdt Self Locking Toggle Switch $2.00
--------------------
2 x ($1.00) 40 x 10 male - female jumper plug / socket jumpers jumper wires $2.00
1 x ($1.43) SODIAL 65 x Jumper Wires Asked Jumpers Breadboard $1.43
1 x ($1.29) 65PCS Male to Male Solderless Breadboard Jumper Cable $1.29
1 x ($1.19) SODIAL 9v Battery Clip Snap Accessories with 2.1 X 5.5mm Male Dc Plug for Arduino(5 Pack) $1.19
1 x ($0.77) SODIAL 1pcs MINI USB Cable for Arduino NANO Controller Board $0.77

1 x (-$4.32) DISCOUNT FOR PROMOTION CODE
-$4.32
Subtotal:	$82.03
Tax:	$6.36
Shipping and Handling:	$0.00
Total Amount:	$88.39

Other parts
+ LM317 DC-DC 1.5A 1.2-37V Adjustable Power Supply Board DC Converter Buck Step Down Module Adjustable Linear
+ Breadboard Power Supply Module 2 Channel: 3.3v and 5v
+ Breadboard Power Supply Module MB102 with switch and DC female plug: 3.3v and 5v, Input voltage: 6.5-12 V (DC) or USB power supply
+ 1602 LCD with Keypad Shield Board Blue Backlight Module for Arduino Duemilanove
+ Nano V3.0 ATmega328P controller compatible for arduino nano CH340 USB driver NO CABLE NANO 3.0
````
--------------------------------------------------------------------------------
eof