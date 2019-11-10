--------------------------------------------------------------------------------
# Arduino Notes

To do:
+ I'm getting a warning message when compiling.
To remove the warning, deleting the version of the library you currently have installed and then installing version 1.0.0:
https://github.com/NorthernWidget/DS3231/archive/1.0.0.zip
Reference: https://forum.arduino.cc/index.php?topic=477214.0

--------------------------------------------------------------------------------
### General info

A microcontroller unit (MCU) includes a processor, memory, and input/output (I/O) pins on a single chip.

Computer microprocessors are designed to maximize compute power on the chip,
with internal bus connections--rather than direct I/O--to supporting hardware such as RAM and serial ports.

Simply put, coffee makers use microcontrollers; desktop computers use microprocessors.

On Ebay,
https://www.ebay.com/itm/MINI-USB-Nano-V3-0-ATmega328P-CH340G-5V-16M-Micro-controller-board-for-Arduino/383093281539
Note, there are differences Nano boards. The processors are ATmega328P, however they use various USB connection chips. I use the cheap one, CH340 USB driver. This required me to install a driver. Here is a link to how I got my MacBook to communicate with my Nano CH340 boards:
https://github.com/tigerfarm/arduino/tree/master/MacInstall
On Windows, I installed a CH340 driver and all was good.
One other note, I have the cheap cheap ones, I set my IDE to use “Processor ATmega328P (Old Bootloader)“. Some of the ones I got for a little over $3, they use “Processor ATmega328P”. Either way, they work fine, I don’t see any noticeable difference. I’ve bought over a dozen Nano boards, their my favorite, great for use on a breadboard.

--------------------------------------------------------------------------------
### Set up on my MacBook:

In the Arduino IDE menu, select:
+ Tools/Boards: Arduino Nano
+ Processor: ATmega328P (Old Bootloader), or with some Nano boards, use ATmega328P.
+ Port: /dev/cu.wchusbserial14120

--------------------------------------------------------------------------------
### Steps to Install Libraries

+ Either from the library manager: IDE menu Tools/Manage Libraries
+ Or by downloading a Zip file.
Then, IDE menu Sketch/Include Library/Add ZIP Library. Select the Zip file.
+ Or download a Zip file.
Move the unzipped files directory, into the directory: Users/.../Documents/Arduino/libraries.
IDE menu Library manager (Sketch/Include library/Manage libraries), install: tvout.

List of available libraries:
https://www.arduinolibraries.info/categories/signal-input-output

--------------------------------------------------------------------------------
## Build next and projects in progress

#### Programmable LCD Clock project: WiFi set time

Project to set time using infrared:
+ Set Programmable Clock, components: DS3231 clock, 1602 LCD, infrared receiver, Nano board,
    USB micro cable for power, and full sized breadboard.
+ Set date(year and month) and time (hours, minutes, seconds) using the infrared receiver and a remote control.

#### Digital Clock project

+ Digital clock, components: DS3231 clock, 2 2x7-segment digit displays, 2 Nano boards,
    USB micro cable for power, and full sized breadboard.
+ DS3231 clock is set by using the Programmable LCD Clock.
+ Need a program set the clock using the 2 2x7-segment digit displays. This approach would also work with a TM1637 display.
+ Or, add a NodeMCU board to make a request to a server to get the time, and then use that time to set the clock.
+ Use TM1637 to replace the 2 2x7-segment digit displays and the second Nano board which displays the hours.
+ Alarms: https://github.com/kriswiner/DS3231RTC/blob/master/DS3231RTCBasicExample.ino

#### Error handling for Non-I2C Nano to Nano communications between Nano boards.

Non-I2C Nano to Nano communications works well in the Programmable Clock project.

However, no error handling. If anything goes wrong, the circuit needs to be reset.

I have updated my data communications program for bi-directional data:
+ A Nano can send and receive data to/from another Nano that uses the same sketch.
+ I should implement bi-directional data, by adding data acknowledgement:
Minutes Nano sends hour byte to the Hour Nano.
After receiving the hour byte value, reply with an ack.
Or if there was a data error, reply with a resend request.
+ Maybe add parity check.

#### Other projects

+ MP3 player for the new Douk Audio amp.
+ Update pong: align the boarders, and add cables for paddles for playing.
+ Timer to turn an electrical light on and off using a 120v control relay.
    This project will use the Programmable Clock to control the 120v control relay.
+ Test BME280 weather device: Temperature, Humidity and Pressure.
+ Weather module: Temperature, Humidity and Pressure.
    Requires: Nano with USB micro cable, breadboard, LCD, BME280

+ MP3 player
    components: Nano with a USB micro cable for power, IR receiver, DFPlayer, half size breadboard, and amplifier
+ Minute clock pulse for complex clock.
    Requires DS3231 clock board and LCD, Nano with a USB micro cable for power.

+ SD card reader to store and read state

--------------------------------------------------------------------------------
#### DS3231 clock board EEPROM Read/write

+ Read/write to/from the 32K EEPROM of the AT24C32 chip on the DS3231 clock.
+ The AT24C32 has 32K bits of serial EEPROM memory organized as 4096 words of 8 bits each.
````
#include <Wire.h>

void setup() {
  Serial.begin (115200);
  ...
  Serial.println ();
  Serial.println ("+ I2C device scanner, scanning...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("++ Address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      }
  }
  Serial.print ("+ Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
````
AT24C32 EEPROM write and read program.
````
#define AT24C32_I2C_ID 0x57   //I2C address for the AT24C32 EEPROM chip on the DS3231 RTC board

void setup() {
  Serial.begin(9600);
  Wire.begin();
  writeEEPROM(0,98);  		// Write 98 to EEPROM location 0
  delay(5);                     // need this delay after a write before a read
  byte c = readEEPROM(0);	// Read from location 0
  Serial.print("+ Byte read from location 0 is ");
  Serial.println(c);
}

void loop() {
}

void writeEEPROM(int address, byte data) {
  Wire.beginTransmission(AT24C32_I2C_ID);
  // send memory address to write
  // the memory address consists of 12 bits (2^12 = 4096 = 32K bytes)
  Wire.write(address >> 8);    // Address MSB = upper 4 bits
  Wire.write(address & 0xff);  // Address LSB = last 8 bits
  // write data
  Wire.write(data);
  Wire.endTransmission();
}

byte readEEPROM(int address) {
  // send address to read
  Wire.beginTransmission(AT24C32_I2C_ID);
  Wire.write(address >> 8);    // Address MSB = upper 4 bits
  Wire.write(address & 0xff);  // Address LSB = last 8 bits
  Wire.endTransmission();
  // request 1 byte to read
  Wire.requestFrom(AT24C32_I2C_ID, 1);
  return Wire.read();
}
````

#### Parts I have for building

+ 1 Nano boards
+ 4 [NodeMCU ESP8266](https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/), with ESP12E for WiFi, boards
+ 1 Uno
+ 1 Uno clone, for which I need to figured out a driver
+ Breadboards: 1/2 and full sized.
+ 3 DS3231 clock boards
+ 4 rotary encoders
+ 3 relay switches
+ 3 BME280 boards
+ NRF24L01 wireless communication boards
+ USB cables to use a power source with USB wall plugs.
+ 5 5v power adapters
+ Voltage Regulator: AMS1117-3.3v

--------------------------------------------------------------------------------
#### TTL UART CP2102 Converter for serial communications through USB.

+ CP2102 can be used to migrate legacy serial port based devices to USB.
+ RS232/Serial Communication protocol, to build USB devices very easily.
+ The module creates a virtual COM port using USB on your computer which can support various standard Baud Rates for serial communication.
+ After driver installation, plug the module into any USB port of your PC.
Driver [link](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).
+ A new COM port is made available to the PC. Convenient for TTL level data i/o.
+ The Rx and Tx pin can be connected directly to the MCUs pins (assuming 5v i/o).



Pinouts
This module has 6 pin breakout which includes 
+    TXD = Transmit Output - Connect to Receive Pin(RXD) of Micro controller. This pin is TX pin of CP2102 on board.
+    RXD = Receive Input - Connect to Transmit Pin(TXD) of Micro controller. This pin is RX pin of CP2102 on board.
+    GND = Should be common to microcontroller ground.
+    3V3 = Optional output to power external circuit upto 50mA.
+    5V = Optional output to power external circuit upto 500mA
+    DTR/RST = Optional output pin to reset external microcontrollers like Arduino.

--------------------------------------------------------------------------------
#### 7-segment display

7-Segment Displays on the Arduino Setup for 1 and4 digits.
http://www.circuitbasics.com/arduino-7-segment-display-tutorial/
+ Use a 1k resister/digit to limit current.
+ Uses library: SevSeg.
+ For 1 digit, uses Nano digital pins 2-8, 6 pins.
+ My program sample for 1 digit:  7segment1digit.ino
+ For 2 digit, uses Nano digital pins 4-12, 9 pins.
+ My program sample for 2 digits: 7segment2digit.ino

4 Nano pins for 1 7-segment display using a 74LS47 which is binary to display.
Reference:
https://www.electronics-tutorials.ws/counter/bcd-counter-circuit.html

Texas Instruments 74LS47 7447 SN74HC47N IC BCD to Seven-Segment Decoder/Driver.
+ On eBay.com, 5 for about $3.
+ The display requires resisters: 220-470 ohm. 
Use bitRead to get a specific bit of a number.
Loop through the number of bits, to get all the bits.
````
lcd.setCursor(0,1); // set to first character bottom line
for (int i=15; i>=0; i--)
  if (bitRead(value,i)) lcd.print ("1"); else lcd.print ("0");
````
Reference:
https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/bitread/

00 to 99 on 2 separate Seven-Segment Displays:
https://create.arduino.cc/projecthub/KVLakshmiSri/00-to-99-on-seven-segment-displays-1ca7e0
+ Uses all digital pins 1-13.
+ no library required.

+ Serial printing formats:
https://www.arduino.cc/reference/en/language/functions/communication/serial/print/

--------------------------------------------------------------------------------
#### TM1637: 4 x 7-segment display

Pins:
+ Clock
+ Data I/O
+ 5+
+ Ground

http://www.zonnepanelen.wouterlood.com/10-four-digit-7-segment-led-display-for-arduino-based-on-the-tm1637-driver/
````
#include <TM1637Display.h> // The library has the option to create custom characters.
const int CLK = 9;         // set the CLK pin connection to the display
const int DIO = 8;         // set the DIO pin connection to the display
int i = 0;
TM1637Display display(CLK, DIO);
void setup(){
    display.setBrightness(0x0a); // set the display to maximum brightness
}
void loop(){
    for(i = 0; i < 9999; i++){
        display.showNumberDec(i);
        delay (250);
    }
}
````
--------------------------------------------------------------------------------
## Hardware Component Notes

### Interrupt pins

Pins 2 and 3 are the interrupt pins on Nano, Uno, and other 328-based boards.
NodeMCU based ESP8266 has interrupt feature on GPIO pins D0-D8.

Example setup for the rotary encoder, using Nano pin 2:
````
attachInterrupt (0, rotarydetect, CHANGE);
--- or ---
attachInterrupt (digitalPinToInterrupt(2), rotarydetect, CHANGE);
````
Click [here](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) for Nano documentation.
Click [here](https://www.electronicwings.com/nodemcu/nodemcu-gpio-interrupts-with-arduino-ide) for NodeMCU documentation.

NodeMCU sample interrupt program.
Setup connections: NodeMCU 3.3v to button to D2 to resister to ground.
````
uint8_t GPIO_Pin = D2;
void setup() {
 Serial.begin(9600);
 attachInterrupt(digitalPinToInterrupt(GPIO_Pin), IntCallback, RISING);
}
void loop() {
}
void IntCallback(){
 Serial.print("Stamp(ms): ");
 Serial.println(millis());
}
````

--------------------------------------------------------------------------------
### I2C Bus synchronous serial protocol for component communications

The DS3231 Clock and 1602 LCD display both communicate with the Nano using I2C.
````
Nano pins 4 (SDA) and 5 (SCL) are used for I2C.
SDA is the serial data pin.
SCL is the serial clock pin.
The clock signal synchronizes data transfer between devices.
````
Only two wires are required for communication between up to almost 128 (112) devices when using 7 bits addressing.
The 8th bit is used for indicating whether the master will write to the slave (logic low) or read from it (logic high).

I2C (Inter-Integrated Circuit), pronounced I-two-C or I-squared-C.
I²C is appropriate for peripherals where simplicity and low manufacturing cost are more important than speed.
https://en.wikipedia.org/wiki/I%C2%B2C

#### Connecting 2 Nano boards

Reference:
https://www.arduino.cc/en/Tutorial/MasterWriter

Connect pin 5 (SCL) and pin 4 (SDA) on the master Nano (write) to the slave Nano (receive).
Make sure that both boards share a common ground.
Connect the 5V output of the Master to the VIN pin on the slave.

1. Master write data to the slave.
````
#include <Wire.h>
void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
}
byte x = 0;
void loop() {
  Wire.beginTransmission(8);    // transmit to device #8
  Wire.write("x is ");          // sends five bytes
  Wire.write(x);                // sends one byte
  Wire.endTransmission();       // stop transmitting
  x++;
  delay(500);
}
````
Slave receives the data.
````
#include <Wire.h>
// Event function that executes whenever data is received.
void receiveEvent(int howMany) {
  while (1 < Wire.available()) {    // loop through all but the last
    char c = Wire.read();           // receive byte as a character
    Serial.print(c);                // print the character
  }
  int x = Wire.read();              // receive byte as an integer
  Serial.println(x);                // print the integer
}
void setup() {
  Wire.begin(8);                    // join i2c bus with address #8
  Wire.onReceive(receiveEvent);     // register event function
  Serial.begin(9600);               // start serial for output
}
void loop() {
  delay(100);
}
````

2. Master request data from the slave, and then receives the data.
````
#include <Wire.h>
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}
void loop() {
  Wire.requestFrom(8, 6);       // request 6 bytes from slave device #8
  while (Wire.available()) {    // slave may send less than requested
    char c = Wire.read();       // receive a byte as character
    Serial.print(c);            // print the character
  }
  delay(500);
}
````
Slave write data, when requested by the master:
````
#include <Wire.h>
// Event function that executes whenever data is requested by master.
void requestEvent() {
  //          123456
  Wire.write("hello ");         // respond with request message of length 6 bytes.
}
void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}
void loop() {
  delay(100);
}
````

--------------------------------------------------------------------------------
+++ GPIO Port Extenders chip: PCF8574

+ Can connect up to 8, 8-bit GPIO Port Extenders (PCF8574), that run on the I2C bus.

+ I2C uses simple addressing, only two lines per bus one clock and one data for all connected devices.
+ I2C communication speeds are 100 kHz and 400 kHz for fast mode. I have seen 1.2 MHz though.
+ PCF8574 has only 1 power connection to drive all LED simultaneously, which may not be enough.

Nice wiring sample:
https://www.youtube.com/watch?v=mXMkgQf3fqU
+ Sketch for 2 chips.
https://github.com/GadgetReboot/Arduino/blob/master/Uno/PCF8574/PCF8574.ino

+ Using only the Wire.h library.
https://www.youtube.com/watch?v=kQZU8kE3ksU
````
#include "Arduino.h"
#include <Wire.h>
void setup() {
// I2C Two Wire initialisation
Wire.begin();
// Turn OFF all pins by sending a high byte (1 bit per byte)
Wire.beginTransmission(0x027);
Wire.write(0xF);
Wire.endTransmission();
}
loop {
//Simple LED blink! Turn ON P0 bit by setting LOW (to zero)
Wire.beginTransmission(0x027);
Wire.write(0b11111110);
Wire.endTransmission();
delay (500);
// Now turn OFF P0 and turn P1 ON
Wire.beginTransmission(0x027);
Wire.write(0b11111101);
Wire.endTransmission();
}
```
+ Using the PCF8574.h library.
https://www.instructables.com/id/PCF8574-GPIO-Extender-With-Arduino-and-NodeMCU/
+ Supporting video:
https://www.youtube.com/watch?v=-CXSEWlMYFs
+ About the library:
https://playground.arduino.cc/Main/PCF8574Class/
````
include the "PCF8574.h" library
PCF8574(uint8_t address);
pcf8574.pinMode(P0, OUTPUT);
pcf8574.pinMode(P1, INPUT);
//
PCF8574.digitalWrite(P1, HIGH); // or LOW
int p1 = PCF8574.digitalRead(P1); // reads pin P1
// Read all pins:
 PCF8574::DigitalInput di = PCF8574.digitalReadAll();
Serial.print(di.p0); Serial.print(" - ");
Serial.print(di.p1); Serial.print(" - ");
...
Serial.println(di.p7);
// Function interrupt to monitor for key press
void keyPressedOnPCF8574();
// Set i2c HEX address
PCF8574 pcf8574(0x20, ARDUINO_UNO_INTERRUPT_PIN, keyPressedOnPCF8574);
````

--------------------------------------------------------------------------------
### Wireless Communication – NRF24L01

Steps:
https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/

Video:
https://www.youtube.com/watch?v=7rcVeFFHcFM

Other video:
https://www.youtube.com/watch?v=o74kQSlpGn4
https://www.youtube.com/watch?v=D40cgHyBLL4

--------------------------------------------------------------------------------
### BME280 board for Weather projects

Use an Nano with BME280 for Temperature, Humidity and Pressure
+ BME280 6 pins labels (pins up):
+ Pins: VCC GND|SCL SDA|CSB SDO
+ BME280 pins down, and following: https://www.youtube.com/watch?v=PidE-pJiIXY
````
---------------
| 1 3.3V      | To Arduino Nano, pin 3.3v
| 2 GND       | To Arduino Nano, pin GND
| 3 SCK/SCL   | To Arduino Nano, pin A5
| 4 SDI/SDA   | To Arduino Nano, pin A4
| 5 CSB       | Not used.
| 6 SDO       | Not used.
---------------
````
Library suggested: Grove - Barometer Senor BME280 by Seed Studio, version 1.02

Projects:
https://www.instructables.com/id/Arduino-Nano-and-Visuino-Display-BME280-Temperatur/
https://www.instructables.com/id/Arduino-Easy-Weather-Station-With-BME280-Sensor/

TV output of temperature.
https://www.onetransistor.eu/2018/01/arduino-thermometer-with-tv-output.html

--------------------------------------------------------------------------------
### DS3231 Clock board

DS3231 is a high precision real-time clock module.
+ The DS3231 has an internal Temperature Compensated Crystal Oscillator(TCXO) which isn’t affected by temperature.
It is accurate to a few minutes per year.
+ The battery, a CR2032, can keep the RTC running for over 8 years without an external 5V power supply.
Another source said, the battery will keep the clock going for over 1 year.
+ The 24C32 EEPROM (32K pin) uses I2C interface for communication and shares the same I2C bus as DS3231.

DS3231 Pins:
````
VCC to 3.3-5.5V
GND to ground
SDA to D4 (pin 4) on Uno and Nano
SCL to D5 (pin 5) on Uno and Nano
````
The DS3231 connects to the Nano i²c (I2C or i2c) bus: SDA and SCL.

+ Project: Clock timer which uses a battery powered DS3231 clock and an LCD 1602.

+ Clock using LCD 1602 keypad shield (left, right, up, down, select).
http://ediy.com.my/projects/item/138-arduino-based-lcd-digital-clock
https://youtu.be/Hp1u-akidpM

Reference: https://forum.arduino.cc/index.php?topic=477214.0

--------------------------------------------------------------------------------
### I2C Serial LCD 1602 (16 characters x 2 lines)

Use I2C Serial, only 2 microcontroller pins, to display messages on the LCD 1602.

Connect LCD to Nano:
````
SCL - A5
SDA - A4
VCC - 5V
GND - GND
````

#### I2C communications between Nano boards.

Works, but not when the project also includes a DS3231 clock board which uses I2C.

See projects: i2cSlave or i2cSlave2digits, and i2cMasterRotaryEncoder.

#### Non-I2C Nano to Nano communications between Nano boards.

Works well. However, no error handling.
Currently, if anything goes wrong, the sender and the receiver circuit needs to be reset.

See projects: dataReceiver or dataReceiver2digits, and dataSenderRotaryEncoder.

--------------------------------------------------------------------------------
### Useful components

+ Arduino Nano 3.0 controller, CH340 USB driver
+ ESP8266 ESP-12E NodeMcu Lua V3 CH340G WIFI Lua Wireless Development Board Module

+ LCD 1602 16x2 Serial HD44780 Character LCD Board Display with White on Blue Backlight 5V with IIC/I2C Serial Interface Adapter
+ Infrared IR Wireless Remote Control Module Kits: Keyestudio 38KHz, IR receiver and small remote controller
+ 4 x 4 16 Key Matrix Membrane Switch Keypad
+ 10 Segment LED Bargraph Light Display
+ DS3231: DC 3.3-5.5V, High Precision Real-Time Clock Module, $1.29
+ BME280: Humidity, Temperature, Barometric Pressure Sensor

MP3 player components:
https://secure.newegg.com/Wishlist/MyWishlistDetail?ID=8188917
+ YDA138-E Audio Amplifier Board 8W+8W 12W+12W Dual Mode HIFI Dual Channel Stereo Digital Amplifier Board DC12V For YAMAHA    $4.99
+ TRIXES Electronics Solderless 400 Point Prototyping Breadboard, $2.39
+ DFPlayer Mini MP3 Player Module, $2.78

+ 3.3V DC 800mA from 4.75V-12V, Voltage Regulator: AMS1117-3.3V
+ 5V DC from 6.0V-12V, Voltage Regulator: AMS1117-5.0V, 2 pins in, 2 pins out voltage
+ 5V power supply module: AMS1117 with DC seat and switch
+ Battery: CR2032, used in the DS3231 clock

What range of voltage should be expected from the Nano '5V' pin?
+ Maximum 5.5v
+ Sticking my multi meter directly up the USB port in the desktop gives me 5.11V.
+ You need 6.5V or so to get the on-board regulator working properly.
+ If you power the board from an external power supply of 7 to 12 V on the VIN pin, the onboard regulator would be used and you may find the 5V voltage is closer to 5V. 
+ ATmega328P will run at any voltage between 1.8 and 5.5 V depending configurations

#### Sound interesting
+ 3V-5V power model NEO-6M GPS module GY-GPS6MV2 (Blue)
+ LD1117V33 Linear Voltage Regulator 3.3V

+ OLED
https://www.youtube.com/watch?v=PrIAnDZ9dp8
+ Detail video
https://www.youtube.com/watch?v=7x1P80X1V3E

+ Fix Bluetooth hum: I am replacing the simple board amp with a 
https://www.amazon.com/dp/B07QRRQDDW
+ Another amp option, which looks better, Price: $39.99 & FREE Shipping:
https://www.amazon.com/dp/B07HR7PRV5

+ DS3231 clock board: DC 3.3-5.5V, High Precision Real-Time Clock Module, $1.29

+ Keypad
+ 1 x ($1.00) 4 x 4 16 Key Matrix Membrane Switch Keypad $1.00
http://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/

+ 1602 LCD Keypad Shield for an Arduino Uno R3
+ Program examples:
https://create.arduino.cc/projecthub/electropeak/using-1602-lcd-keypad-shield-w-arduino-w-examples-e02d95

--------------------------------------------------------------------------------
## ESP8266 ESP-12E NodeMcu

Adding infrared receiver:
https://www.instructables.com/id/Universal-Remote-Using-ESP8266Wifi-Controlled/
+ Library: IRremoteESP8266?
+ Power on right side, G and 3V.
+ D4 or D2 for receiver. Sample uses: TSOP1738 IR receiver or some other IR receivers.
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
+ I used Boards Manager, search for ESP and installed: ESP8266 Community version2.5.2.

+ Search: ESP8266 ESP-12E CH340
+ Search: ESP8266 ESP-12E CP2102
+ Looks like Arduino UNO uses FTDI USB drivers (FTDI FT232/FT232R that Arduino uses), not CH340 nor CP2102.

LoLin version uses USB CH340, which same as my Arduino Nano chips
CH340 just fine without needing a driver with setup.
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

ENC28J60 spi interface Ethernet network module

sim800l gprs gsm module micro-sim core board quad-band for arduino
https://www.ebay.com/itm/SIM800L-gprs-gsm-module-micro-sim-core-board-quad-band-for-arduinor-K5H1/263370283725?epid=26010975359&hash=item3d52179acd:g:iZIAAOSwEaBaKC03

--------------------------------------------------------------------------------
## Projects

### Working

+ Rotary Encoder
https://github.com/tigerfarm/arduino/tree/master/samples/RotaryEncoder
+ Rotary Encoder with 2 LED digit 7 segment display
https://github.com/tigerfarm/arduino/tree/master/samples/RotaryEncoder2digits

+ LED digit 7 segment display
https://github.com/tigerfarm/arduino/tree/master/samples/7segment1digit
+ 2 LED digit 7 segment display
https://github.com/tigerfarm/arduino/tree/master/samples/7segment2digit

+ 120v plugin controller: Arduino, relay, electrical supplies from Home Depot.
I need to find the program I used to test.

+ DS3231 Clock: Clock pulses: second, minutes, hours, days.
++ With option to toggle the LCD display on and off.
https://github.com/tigerfarm/arduino/blob/master/samples/clockPulseWithLcdToggle/clockPulseWithLcdToggle.ino
+ DS3231 Clock and the LCD display
https://github.com/tigerfarm/arduino/blob/master/samples/clockLcd/clockLcd.ino

+ Serial 1602 LCD display
https://github.com/tigerfarm/arduino/tree/master/samples/lcd1602serial
+ Program to count seconds, as best I can without a clock such as a DS3231.
Each loop, the program runs delay(993), which is close to having a 1 second loop, with logic and displaying the counter to the LCD.
The accuracy various widely. Only good when approx time is okay.

+ Bar graph
https://github.com/tigerfarm/arduino/tree/master/samples/BarGraph

+ Thin plastic Keypad
https://github.com/tigerfarm/arduino/tree/master/samples/keypadv1

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

-------------
### Other Projects to do next

+ Bluetooth player board, update to the old AM radio

-------------
### Complex Clock project: LED Clock

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
## Information and sample project links

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

+ Using an Arduino and a SIM800L to send SMS:
https://drive.google.com/file/d/1HGcKj3CmnLDYkvfE0SdCQt91t06j1807/view
+ By Brian Mgrdichian, Senior Solutions Engineer.
+ Video: How to use a GSM module to send and receive SMS messages : SIM800L : AT Commands
https://www.youtube.com/watch?v=sA4w6LaSmjM
+ I've been working on programming the attiny10 in assembly so I can use it on some custom PCBs
kinda wanna make a "mute all" IR button that runs on a CR2023 and sends codes to my sound bar, TV, etc on button press
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
#### Parts on Amazon.com

+ Search: YDA138-E Audio Amplifier
+ $2.78 & FREE Shipping 
https://www.amazon.com/Semoic-YDA138-Channel-Digital-Amplifier/dp/B07MKX4FL4/ref=sr_1_7?keywords=YDA138-E+Audio+Amplifier

+ TDA7492P Digital Amplifier Board

+ Search: Infrared Receiver IR Sensor Module
+ Sold 2 at a time, 4 for: $4.78 + Free Shipping, $1.20/each(about)
https://www.amazon.com/barterine-KY-022-Infrared-Receiver-Arduino/dp/B01MG1LFA0/ref=pd_day0_hl_201_2/133-5806447-0800931?_encoding=UTF8&pd_rd_i=B01MG1LFA0&pd_rd_r=cfe3b8b2-ca32-477b-a433-1e69ab7a5e3f&pd_rd_w=RFsGg&pd_rd_wg=Y9yhL&pf_rd_p=cd872437-0036-44da-b76a-718df210c36e&pf_rd_r=DPFTD1HBT427SWZBGF1A&psc=1&refRID=DPFTD1HBT427SWZBGF1A
+ For 3, $1.29 + $2.97 shipping = $4.26 for 3, $1.40/each(about)
https://www.amazon.com/Manakayla-37-9KHz-Infrared-Receiver-Arduino/dp/B07R781JSD/ref=sr_1_3?keywords=Infrared+Receiver+IR+Sensor+Module&qid=1568131755&s=gateway&sr=8-3

+ DS3231 clock, $2.94 & FREE Shipping, Jiasijieke for Arduino DS3231 ZS042 AT24C32 IIC Module Precision RTC Real Time Clock Memory New 
https://www.amazon.com/Jiasijieke-Arduino-DS3231-AT24C32-Precision/dp/B07MLRFRKF/ref=sr_1_14?crid=XGO2MU3QU4D6&keywords=ds3231+real+time+clock+module&qid=1568132528&s=electronics&sprefix=ds32%2Celectronics%2C200&sr=1-14

+ LCD 1602 Serial IIC I2C, For 2: $8.99 & FREE Shipping
https://www.amazon.com/gp/product/B0711WLVP9/ref=ox_sc_act_title_1?smid=A304913WPKF57E&psc=1

+ DFPlayer MP3, 5 for: $9.98 & FREE Shipping.
https://www.amazon.com/DAOKI-DFPlayer-Player-Module-Arduino/dp/B01MQD5IIA/ref=sr_1_14?keywords=dfplayer&qid=1568135351&s=pc&sr=1-14

---------------
+ $02.01 Solderless Breadboard Bread Board 400 Tie Points Contacts
https://www.amazon.com/Solderless-Breadboard-Bread-Points-Contacts/dp/B00JGFDKBQ/ref=sr_1_8?keywords=SODIAL+breadboard&qid=1554232722&s=gateway&sr=8-8
+ $01.51 Male to female, 20cm Long F/F Solderless Flexible Breadboard Jumper Cable Wire 40 Pcs 
https://www.amazon.com/SODIAL-Solderless-Flexible-Breadboard-Jumper/dp/B00U8MK14K/ref=sr_1_7?keywords=SODIAL+jumper+cable+breadboard&qid=1554232815&s=gateway&sr=8-7
+ $2.66 140pcs U Shape Solderless Breadboard Jumper Cable Wire Kit for Arduino Shield 
https://www.amazon.com/SODIAL-140pcs-Solderless-Breadboard-Arduino/dp/B00K67XXSI/ref=sr_1_10?keywords=SODIAL+jumper+cable+breadboard&qid=1554232815&s=gateway&sr=8-10

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
+++ Creating boards (PCB)

+ Fritzing's PCB
https://fritzing.org/learning/tutorials/designing-pcb/

Turning A Fritzing Board Into A PCB.
+ Right at the bottom of the screen, you can find a ‘Fabricate’ button that will send your board to a fab house in Berlin.
The cost for my board is €6.26 for one. Of course, you can export a Fritzing board as a Gerber,
and send that off to any fab house on the planet. For my board, OSH Park will give me three for $7.15.
I could get ten of these boards made by the Fritzing fab for €55.44, but I already bought twenty of them for $36.29 from Seeed Studio.
+ Reference:
https://hackaday.com/2016/10/11/creating-a-pcb-in-everything-friends-dont-let-friends-use-fritzing/

+ Tutorial: Creating A PCB In Everything: Introduction
https://hackaday.com/2016/09/21/creating-a-pcb-in-everything-introduction/

+ Video
https://www.hackster.io/jithinsanal1610/online-pcb-assembly-from-jlcpcb-b47458
https://youtu.be/twbgW19gwr4

+ JLC PCB reviews:
https://pcbshopper.com/easyeda-reviews/
+ Get quote from JLC PCB
https://jlcpcb.com/quote#/?orderType=1&stencilLayer=2
+ 10 boards, 80x150mm (about 3" x 6"), Engineering fee: $4.00 (one time), Boards: $4.10 for 5, $8.10 for 10.
+ Shipping: PostLink US Registered Mail  $6.76
+ Total = $12.10 + $6.76 = $18.86 for 10 boards.
+ Suggestion: choose 2oz copper as you will get a better board with less trace faults.
+ With 2oz copper weight, add $16.00, total: Engineering fee: $4.00 (one time), Copper Weight: $16.00, Board:$11.00
+ Total = $31.00 + $10.31 = $41.31 for 10 boards. 
+ Gerber upload of design.
+ Easyeda is a free online tool that we provide to design the PCB, and you can place your order on JLCPCB easily and quickly.
+ Can use online for free:
https://easyeda.com/

+ Review on Elecrow for creating PCB:
https://pcbshopper.com/elecrow-reviews/

--------------------------------------------------------------------------------
eof