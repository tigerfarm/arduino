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

  --------------------

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
  //
  pinMode(13, OUTPUT);    //clock
  pinMode(11, OUTPUT);    //data
  pinMode(4, OUTPUT);     //latch
  pinMode(2, INPUT);      //Input from buttons
  //
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
*/
// -----------------------------------------------------------------------------
// Shift Register
/*
  const int latchPin =  8;  // Nano Pin 08 connected to pin 12 74HC595: ST_CP.
  const int dataPin  = 11;  // Nano Pin 11 connected to pin 14 74HC595: DS.
  const int clockPin = 12;  // Nano Pin 12 connected to pin 11 74HC595: SH_CP.
*/
const int latchPin = 5;           // Latch pin of 74HC595 is connected to Digital pin 5
const int dataPin = 4;            // Data pin of 74HC595 is connected to Digital pin 4
const int clockPin = 6;           // Clock pin of 74HC595 is connected to Digital pin 6

byte dataByte = B01010101;

void updateShiftRegister() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
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
  delay(300);
  Serial.println("+ Connection to the 595 is set.");

  dataByte = B01010101;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, dataByte);  // MSBFIRST or LSBFIRST
  digitalWrite(latchPin, HIGH);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  Serial.println("+ Looping");
  delay(500);
  dataByte = 0;  // Initially turns all the LEDs off, by giving the variable 'leds' the value 0
  updateShiftRegister();
  delay(500);
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    digitalWrite(latchPin, HIGH);
    delay(60);
  }
}
// -----------------------------------------------------------------------------
