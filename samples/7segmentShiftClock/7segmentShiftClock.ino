// -----------------------------------------------------------------------------
/*
  Clock with LED lights for diplay.

  12 lights, one for each hour.
  3 lights for the tens part of the minutes.
  4 lights for the ones part of the minutes.
  Example, 23 minutes is: 010 0011.
  + 2, binary 010, for the tens.
  + 3, binary 0011, for the ones. 

  Connect the DS3231 Clock and the 1602 LCD display, to the Nano:
  + VCC to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + SDA to Nano D4 (pin 4), same on Uno.
  + SCL to Nano D5 (pin 5), same on Uno.
*/
/*
  Using a 7 segment digit display, to display numbers from 0-9.
  Using a SN74HC595N shift register for serial to multiple pin outs.

  For a 1 digit display, numbers from 0-9,
  + Only requires 3 digital Arduino pins to control the 8 digital display pins.

  This technic will work for multiple displays, using 1 shift register per display.
  + The shift registers are daisy chained together.
  For a 2 digit display, numbers from 00-99,
  + Only requires 3 digital Arduino pins to control the 16 digital display pins.

  74HC595 is a SIPO (Serial-In-Parallel-Out) shift registers,
  + 74HC595 pin 16: 5V+
  + 74HC595 pin 15: LED 0   DP "."
  + 74HC595 pin 14: Data    pin (SRCLK) to Nano pin 4, data transfer from Nano to 595.
  + 74HC595 pin 13: Ground (-)
  + 74HC595 pin 12: Latch   pin (RCLK) to Nano pin 5, which does the Parallel-Out task to the 8 output pins.
  + 74HC595 pin 11: Clock   pin (SER)  to Nano pin 6, clock signal to say that the data is ready.
  + 74HC595 pin 10: 5V+
  + 74HC595 pin 09: Daisy chain to next 74HC595, pin 14 (data). Not used in single 74HC595.
  + 74HC595 pin 08: Ground (-)
  ------------------------ Segment
  + 74HC595 pin 07: LED 7.  G
  + 74HC595 pin 06: LED 6.  F
  + 74HC595 pin 05: LED 5.  E
  + 74HC595 pin 04: LED 4.  D
  + 74HC595 pin 03: LED 3.  C
  + 74HC595 pin 02: LED 2.  B
  + 74HC595 pin 01: LED 1.  A
  + 74HC595 pin 15: LED 0   DP "."

  Segment pins for common cathode display (-).
    1 2 3 4 5 (Pin 5 is top right when facing the display)
    G F - A B : middle pin goes to resister, to ground.
        A
       ---
     F|   |B
       ---   G is the middle bar.
     E|   |C
       --- .
        D
    E D - C DP
    1 2 3 4 5

  ----------------------------------------------------------------------
  Cable wire mapping from clock segment display pins to the shift register pins.

  + Pins from the back.
  12345 67890
  12345 12345
  BA-FG BA-FG
  PC-DE PC-DE

  BA-FG BA-FG
  12345 12345 - Segment display top pins
  12345 67890 - Cable wires
  21-67 21-67 - Shift register pins
  + Examples,
  ++ Cable wire 1, goes from segment #1 top pin 1, to shift register #1 pin 2.
  ++ Cable wire 2, goes from segment #1 top pin 2, to shift register #1 pin 1.
  ...
  ++ Cable wire 6, goes from segment #2 top pin 1, to shift register #2 pin 2.
  ++ Cable wire 7, goes from segment #2 top pin 2, to shift register #2 pin 1.
  ...

  P  C-DE P  C-DE
  1  2345 1  2345 - Segment display bottom pins
  1  2345 6  7890 - Cable wires
  15 3-45 15 3-45 - Shift register pins

*/
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Shift Register
const int dataPin = 4;      // Connected to 74HC595 Data  pin 14.
const int latchPin = 5;     // Connected to 74HC595 Latch pin 12.
const int clockPin = 6;     // Connected to 74HC595 Clock pin 11.

byte dataByte = B01010101;

void updateShiftRegister(byte dataByte) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
}

void updateShiftRegister2(byte dataByte1, byte dataByte2) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte1);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte2);
  digitalWrite(latchPin, HIGH);
}

void displayDigit(int theDigit) {
  /*
    Segment pins for common cathode display (-).
      G F - A B : middle pin goes to resister, to ground.
          A
         ---
       F|   |B
         ---   G is the middle bar.
       E|   |C
         --- .
          D
      E D - C DP
  */
  switch (theDigit) {
    case 0:
      //                  0:ABCDEFG
      updateShiftRegister(B01111110);
      break;
    case 1:
      updateShiftRegister(B00110000);
      break;
    case 2:
      updateShiftRegister(B01101101);
      break;
    case 3:
      updateShiftRegister(B01111001);
      break;
    case 4:
      updateShiftRegister(B00110011);
      break;
    case 5:
      updateShiftRegister(B01011011);
      break;
    case 6:
      updateShiftRegister(B00011111);
      break;
    case 7:
      updateShiftRegister(B01110000);
      break;
    case 8:
      updateShiftRegister(B01111111);
      break;
    case 9:
      updateShiftRegister(B01111011);
      break;
    default:
      // Display "E." for error.
      updateShiftRegister(B11001111);
  }
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const int dataPinLed = 7;     // pin 14 Data pin.
const int latchPinLed = 8;    // pin 12 Latch pin.
const int clockPinLed = 9;    // pin 11 Clock pin.

void lightsStatusAddressData( byte status8bits, byte address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  // Use LSBFIRST or MSBFIRST to map the bits to LED lights.
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, address16bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  digitalWrite(latchPinLed, HIGH);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

void syncCountWithClock() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  Serial.print("+ syncCountWithClock,");
  Serial.print(" theCounterHours=");
  Serial.print(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
}

// -----------------------------------------------------------------------------
void processClockNow() {
  // Serial.println("+ Call processClockNow");
  //
  now = rtc.now();
  //
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a second pulse.
    theCounterSeconds = now.second();
    // clockPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a minute pulse.
      theCounterMinutes = now.minute();
      if (theCounterMinutes != 0) {
        // clockPulseMinute();
        Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
        Serial.println(theCounterMinutes);
        // ----------------------------------------------
        displayTheTime( theCounterMinutes, theCounterHours );
      } else {
        // When the clock minute value changes to zero, that's an hour pulse.
        // clockPulseHour();
        theCounterHours = now.hour();
        Serial.print("++ clockPulseHour(), theCounterHours= ");
        Serial.println(theCounterHours);
        // ----------------------------------------------
        displayTheTime( theCounterMinutes, theCounterHours );
      }
    }
  }
}

// ------------------------------------------------------------------------
// Display hours and minutes on LED lights.

void displayTheTime(byte theMinute, byte theHour) {
  byte theMinuteOnes = 0;
  byte theMinuteTens = 0;
  byte theBinaryMinute = 0;
  byte theBinaryHour1 = 0;
  byte theBinaryHour2 = 0;

  // ----------------------------------------------
  // Convert the minute into binary for display.
  if (theMinute < 10) {
    theBinaryMinute = theMinute;
    displayDigit(0);
    displayDigit(theMinute);
  } else {
    // There are 3 bits for the tens: 0 ... 5 (00, 10, 20, 30, 40, or 50).
    // There are 4 bits for the ones: 0 ... 9.
    // LED diplay lights: ttt mmmm
    // Example:      23 = 010 0011
    //            Tens(t) & Minutes(m): B-tttmmmm
    //                                  B00001111 = 2 ^ 4 = 16
    // theMinute = 10, theBinaryMinute = 00010000
    theMinuteTens = theMinute / 10;
    theMinuteOnes = theMinute - theMinuteTens * 10;
    theBinaryMinute = 16 * theMinuteTens + theMinuteOnes;
    displayDigit(theMinuteTens);
    displayDigit(theMinute);
  }

  // ----------------------------------------------
  // Convert the hour into binary for display.
  // Use a 12 hour clock value rather than 24 value.
  if (theHour > 12) {
    theHour = theHour - 12;
  } else if (theHour == 0) {
    theHour = 12; // 12 midnight, 12am
  }
  switch (theHour) {
    case 1:
      theBinaryHour1 = B00000010; // Note, on the shift register, B00000001 is not wired, not used.
      theBinaryHour2 = 0;
      break;
    case 2:
      theBinaryHour1 = B00000100;
      theBinaryHour2 = 0;
      break;
    case 3:
      theBinaryHour1 = B00001000;
      theBinaryHour2 = 0;
      break;
    case 4:
      theBinaryHour1 = B00010000;
      theBinaryHour2 = 0;
      break;
    case 5:
      theBinaryHour1 = B00100000;
      theBinaryHour2 = 0;
      break;
    case 6:
      theBinaryHour1 = B01000000;
      theBinaryHour2 = 0;
      break;
    case 7:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000010;
      break;
    case 8:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000100;
      break;
    case 9:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00001000;
      break;
    case 10:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00010000;
      break;
    case 11:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00100000;
      break;
    case 12:
      theBinaryHour1 = 0;
      theBinaryHour2 = B01000000;
      break;
  }
  // ----------------------------------------------
  lightsStatusAddressData(theBinaryMinute, theBinaryHour1, theBinaryHour2);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------------------------------------
  // ------------------------------------------------------------
  // Segment configurations
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  Serial.println("+ Connection to the 595 is set, v2b.");

  displayDigit(1);
  displayDigit(2);

  // ------------------------------------------------------------
  // ----------------------------------------------------
  // LED lights
  
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  lightsStatusAddressData(0, 0, 0);
  Serial.println(F("+ Front panel LED shift registers ready."));

  // ----------------------------------------------------
  // Initialize the Real Time Clock (RTC).
  if (!rtc.begin()) {
    Serial.println("--- Error: RTC not found.");
    while (1);
  }
  syncCountWithClock();
  displayTheTime( theCounterMinutes, theCounterHours );
  Serial.println("+ Clock set and synched with program variables.");

  // ------------------------------------------------------------
  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  delay(100);
  processClockNow();
}
// -----------------------------------------------------------------------------
