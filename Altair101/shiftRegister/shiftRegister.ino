// -----------------------------------------------------------------------------
/*
  Shifting to Expand I/O.

Using a 74HC595 Shift Register for serial to multiple pin outs.

How 74HC595 Shift Register Works & Interface it with Arduino
https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/

74HC595 is a SIPO (Serial-In-Parallel-Out) shift registers, example: Texas Instruments SN74HC595N.
+ Data pin (SRCLK) to pin 4 (SDA) on Nano
+ Latch pin (RCLK) to pin 5 on Nano, which does the Parallel-Out task to the 8 output pins.
+ Clock pin (SER) to pin 6 on Nano
+ 8 output pin.
+ Pin to daisy chain 595s.
+ Shift register clear to set the out pins to 0.
+ 5+
+ Ground

+ Sample sketch using a library:
https://github.com/j-bellavance/HC595/blob/master/HC595LED.ino
+ Switch on, off or toggle up to 64 pins using up to 8 x 74HC595 Shift Out Serial Registers
https://github.com/j-bellavance/HC595
+ Documentation:
https://www.arduino.cc/en/Tutorial/ShiftOut
+ Samples
https://www.arduino.cc/en/Tutorial/ShftOut13

3 Nano pins with 2 daisy chained 595s, plus program logic, to control 16 output pins.

+ Video
https://www.youtube.com/watch?v=N7CAboD1jU0

  -------------
Sample Nano pins to use:
````
int latchPin = 5;           // Latch pin of 74HC595 is connected to Digital pin 5
int clockPin = 6;           // Clock pin of 74HC595 is connected to Digital pin 6
int dataPin = 4;            // Data pin of 74HC595 is connected to Digital pin 4

const int latchPin = 11;    // Pin ST_CP (12) of the 74HC595
const int clockPin = 10;    // Pin SH_CP (11) of the 74HC595
const int dataPin = 12;     // Pin DS (14) of the 74HC595

int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

  -------------
  void setup()
  {
  Serial.begin(9600);
  byte myByte = B11000011;
  //byte myByte = 0xC3;
  //byte myByte = 195;
  Serial.println(myByte, DEC);
  Serial.println(myByte, BIN);
  Serial.println(myByte, HEX);
  shiftOut(1,1,LSBFIRST, myByte); // <<<< will shift out the same no matter which of the three assignments you choose above.
  }

  --------------------

  int latchPin = 8;   //Pin connected to ST_CP of 74HC595
  int clockPin = 12;  //Pin connected to SH_CP of 74HC595
  int dataPin = 11;   //Pin connected to DS of 74HC595
  byte myArray[] = {
    B00000000, B10000000, B11100000,
    B01100000, B01000000, B01110000,
    B00110000, B00100000, B00111000,
    B00011000, B00010000, B00011100,
    B00001100, B00001000, B00001110,
    B00000110, B00000100, B00000111,
    B00000011, B00000010, B1000011,
    B10000001, B00000001, B11000001,
    B11000000
  };
  void setup() {
  Serial.begin(9600);
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  }
  void loop() {
  for (int i = 0; i < sizeof(myArray)/sizeof(myArray[0]); i++) {
    // "/sizeof(myArray[0]" not needed when array items are byte size.
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, myArray[i]);
    digitalWrite(latchPin, HIGH);
    delay(100);
  }
  }

// -----------------------------------------------------------------------------
+ Using 74HC595 for inputs:
https://www.theengineeringprojects.com/2018/11/arduino-74hc165-interfacing-increase-input-pins.html

+ Using 74HC595 for inputs:
https://forum.arduino.cc/index.php?topic=163813.0
https://www.youtube.com/watch?v=nXl4fb_LbcI
https://www.youtube.com/watch?v=hR6qOhUeKMc

````
#include <SPI.h>
byte Input, Output, Check=1;
int j;
void setup(){
  pinMode(13, OUTPUT);//clock
  pinMode(11, OUTPUT);//data
  pinMode(4, OUTPUT);//latch
  pinMode(2, INPUT);//Input from buttons
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
  SPI.transfer(255);
  SPI.transfer(0);
  digitalWrite(4, HIGH);
  digitalWrite(4, LOW);
  Serial.begin(9600);
  attachInterrupt(0, pin_read, RISING); 
}
void loop(){}
void pin_read(){
  for(j=0; j<50; j++) 
    delayMicroseconds(1000);

  Check=1;
  for(j=0; j<8; j++){
    SPI.transfer(Check);
    SPI.transfer(Output);
    digitalWrite(4, HIGH);
    digitalWrite(4, LOW);
    delayMicroseconds(500);
    if(digitalRead(2)==HIGH)
      bitWrite(Output, j, 1);
    else
      bitWrite(Output, j, 0);
    Check = Check<<1;
  }  
  SPI.transfer(255);
  SPI.transfer(Output);
  digitalWrite(4, HIGH);
  digitalWrite(4, LOW);
}
// -----------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  Serial to Parallel Shifting-Out with a 74HC595:
    https://www.arduino.cc/en/tutorial/ShiftOut

*/
// -----------------------------------------------------------------------------
// Shift Register

const int latchPin = 11;    // Pin ST_CP (12) of the 74HC595
const int clockPin = 10;    // Pin SH_CP (11) of the 74HC595
const int dataPin = 12;     // Pin DS (14) of the 74HC595

void updateShiftRegister() {
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    // From: https://www.arduino.cc/en/Tutorial/ShftOut11
  Serial.println("+ Display the count from 0 to 25 on 8 LEDs.");
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    // pause before next value:
    delay(200);
  }
  Serial.println("+ Display count completed.");

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

byte leds = 0;
void loop() {
  Serial.println("+ Looping");

  Serial.println("+ Turn all the LEDs on one by one.");
     leds = 0;
    for (int i = 0; i < 8; i++) {
        // Set the bit that controls that LED in the variable 'leds'
        bitSet(leds, i);
        updateShiftRegister();
        delay(500);
    }

  Serial.println("+ Turn all the LEDs off one by one.");
     leds = 1;
    for (int i = 0; i < 8; i++) {
        // Set the bit that controls that LED in the variable 'leds'
        bitSet(leds, i);
        updateShiftRegister();
        delay(500);
    }

  delay(60);
}
// -----------------------------------------------------------------------------
