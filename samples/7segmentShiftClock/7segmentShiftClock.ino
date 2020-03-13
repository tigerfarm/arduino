// -----------------------------------------------------------------------------
/*
  Clock with LED lights and two 7 segment digit displays.

  12 lights, one for each hour.
  3 lights for the tens part of the minutes.
  4 lights for the ones part of the minutes.
  Example, 23 minutes is: 010 0011.
  + 2, binary 010, for the tens.
  + 3, binary 0011, for the ones.

  Using a 7 segment digit display, to display numbers from 0-9.
  Using a SN74HC595N shift register for serial to multiple pin outs.

  For a 1 digit display, numbers from 0-9,
  + Only requires 3 digital Arduino pins to control the 8 digital display pins.

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
*/
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Shift Register for 7 segment display.

const int dataPin = 4;      // Connected to 74HC595 Data  pin 14.
const int latchPin = 5;     // Connected to 74HC595 Latch pin 12.
const int clockPin = 6;     // Connected to 74HC595 Clock pin 11.

byte dataByte = B01010101;

void updateShiftRegister(byte dataByte) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
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
// For the clock module.

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
    // Don't display the leading 0. For example, display 6, instead of 06.
    // displayDigit(0);
    updateShiftRegister(0);
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
    displayDigit(theMinuteOnes);
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
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  Serial.println("+ Segment display shift registers ready to use.");
  // Clear the digits.
  updateShiftRegister(0);
  updateShiftRegister(0);
  delay(1000);

  // ------------------------------------------------------------
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  lightsStatusAddressData(0, 0, 0);
  Serial.println(F("+ LED shift registers ready to use."));

  // ----------------------------------------------------
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
