// -----------------------------------------------------------------------------
/*
  Clock with LED lights and two 7 segment digit displays.

  Uses a DS3231 real time clock module for timing and keeping numeric time values.

  12 lights, one for each hour.
  3 lights (left)  for the tens part of the minutes.
  4 lights (right) for the ones part of the minutes.
  Example, 23 minutes is: 010 0011.
  + 2, binary 010, for the tens.
  + 3, binary 0011, for the ones.

  Using two 7 segment digit displays, to display hours from 0-12.
  Using one SN74HC595N shift register for each 7 segment digit display.
  The two SN74HC595N shift registers, only requires 3 digital pins to control the 7 segment digit displays.

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

void writeDigitShiftRegister(byte dataByte) {
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
      //                     0:.ABCDEFG
      writeDigitShiftRegister(B01111110);
      break;
    case 1:
      writeDigitShiftRegister(B00110000);
      break;
    case 2:
      writeDigitShiftRegister(B01101101);
      break;
    case 3:
      writeDigitShiftRegister(B01111001);
      break;
    case 4:
      writeDigitShiftRegister(B00110011);
      break;
    case 5:
      writeDigitShiftRegister(B01011011);
      break;
    case 6:
      writeDigitShiftRegister(B00011111);
      break;
    case 7:
      writeDigitShiftRegister(B01110000);
      break;
    case 8:
      writeDigitShiftRegister(B01111111);
      break;
    case 9:
      writeDigitShiftRegister(B01111011);
      break;
    default:
      // Display "E." for error.
      writeDigitShiftRegister(B11001111);
  }
}

void displayDigitMinute(int theMinute) {
  if (theMinute < 10) {
    writeDigitShiftRegister(0);   // Don't display the leading 0. For example, display 6, instead of 06.
    if (theMinute == 0) {
      writeDigitShiftRegister(0); // Both digit display segments are off for 0 minutes.
    } else {
      displayDigit(theMinute);
    }
  } else {
    int theMinuteTens = theMinute / 10;
    int theMinuteOnes = theMinute - theMinuteTens * 10;
    displayDigit(theMinuteTens);
    displayDigit(theMinuteOnes);
  }
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const int dataPinLed = 7;     // pin 14 Data pin.
const int latchPinLed = 8;    // pin 12 Latch pin.
const int clockPinLed = 9;    // pin 11 Clock pin.

void writeMinuteHourShiftRegisters( byte theBinaryMinute, byte theBinaryHour1, byte theBinaryHour2) {
  // theBinaryMinute: Minute LED lights
  // theBinaryHour1:  hours, 1-6 LED lights
  // theBinaryHour2:  hours, 7-12 LED lights
  digitalWrite(latchPinLed, LOW);
  // Use LSBFIRST or MSBFIRST to map the bits to LED lights.
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, theBinaryHour2);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, theBinaryHour1);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, theBinaryMinute);
  digitalWrite(latchPinLed, HIGH);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// For the DS3231 real time clock module.

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

// ------------------------------------------------------------------------
// Display hours and minutes on LED lights.

void displayMinutesHours(byte theMinute, byte theHour) {
  byte theMinuteOnes = 0;
  byte theMinuteTens = 0;
  byte theBinaryMinute = 0;
  byte theBinaryHour1 = 0;
  byte theBinaryHour2 = 0;

  // ----------------------------------------------
  // Convert the minute into binary for display.
  if (theMinute < 10) {
    theBinaryMinute = theMinute;
    writeDigitShiftRegister(0);   // Don't display the leading 0. For example, display 6, instead of 06.
    if (theMinute == 0) {
      writeDigitShiftRegister(0); // Both digit display segments are off for 0 minutes.
    } else {
      displayDigit(theMinute);
    }
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
  // Hour 0, is 12 midnight.
  if (theHour > 12) {
    theHour = theHour - 12;
  } else if (theHour == 0) {
    theHour = 12;           // 12 midnight, 12am
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
  writeMinuteHourShiftRegisters(theBinaryMinute, theBinaryHour1, theBinaryHour2);
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
        displayMinutesHours( theCounterMinutes, theCounterHours );
      } else {
        // When the clock minute value changes to zero, that's an hour pulse.
        // clockPulseHour();
        theCounterHours = now.hour();
        Serial.print("++ clockPulseHour(), theCounterHours= ");
        Serial.println(theCounterHours);
        // ----------------------------------------------
        displayMinutesHours( theCounterMinutes, theCounterHours );
      }
    }
  }
}

// ------------------------------------------------------------------------
void runDisplayTest() {
  // writeMinuteHourShiftRegisters(theBinaryMinute, theBinaryHour1, theBinaryHour2);
  // theBinaryMinute: Minute LED lights
  // theBinaryHour1:  hours, 1-6 LED lights
  // theBinaryHour2:  hours, 7-12 LED lights
  byte testByte1 = B00000000;
  // ----------------------------
  Serial.println(F("+ LED lights all on, then all off using writeMinuteHourShiftRegisters()."));
  delay(3000);
  for (int i = 0; i < 3; i++) {
    // All on.
    writeMinuteHourShiftRegisters(0, B01111110, B01111110);
    delay(500);
    // All off.
    writeMinuteHourShiftRegisters(0, 0, 0);
    delay(500);
  }
  // ----------------------------
  Serial.println(F("+ All digit LED light segments on, then all off using writeDigitShiftRegister()."));
  delay(3000);
  for (int i = 0; i < 3; i++) {
    // All on.
    writeDigitShiftRegister(B11111111);
    writeDigitShiftRegister(B11111111);
    delay(500);
    // All off.
    writeDigitShiftRegister(0);
    writeDigitShiftRegister(0);
    delay(500);
  }
  /*
    Serial.println(F("+ Run LED counter test using writeMinuteHourShiftRegisters()."));
    for (int digitToDisplay = 0; digitToDisplay < 128; digitToDisplay++) {
    writeMinuteHourShiftRegisters(digitToDisplay, 0 , 0);
    delay(100);
    }
    delay(3000);
  */
  // ----------------------------
  Serial.println(F("+ Run LED hour test, 1...12, using writeMinuteHourShiftRegisters()."));
  delay(3000);
  for (int i = 0; i < 3; i++) {
    testByte1 = B00000001;
    for (int digitToDisplay = 0; digitToDisplay < 6; digitToDisplay++) {
      testByte1 = testByte1 << 1;
      writeMinuteHourShiftRegisters(0, testByte1, 0);
      delay(100);
    }
    testByte1 = B00000001;
    for (int digitToDisplay = 0; digitToDisplay < 6; digitToDisplay++) {
      testByte1 = testByte1 << 1;
      writeMinuteHourShiftRegisters(0, 0, testByte1);
      delay(100);
    }
  }
  // ----------------------------
  Serial.println(F("+ Run LED hour test, 1...12, using displayMinutesHours()."));
  delay(3000);
  for (int i = 0; i < 3; i++) {
    for (int digitToDisplay = 0; digitToDisplay < 12; digitToDisplay++) {
      displayMinutesHours(0, digitToDisplay);
      delay(100);
    }
  }
  // ----------------------------
  Serial.println(F("+ Run LED hour test, 1...12, using displayMinutesHours() and displayDigitMinute()."));
  delay(3000);
  writeMinuteHourShiftRegisters(0, 0, 0); // Clear the LED lights.
  for (int i = 0; i < 2; i++) {
    for (int digitToDisplay = 0; digitToDisplay < 13; digitToDisplay++) {
      displayMinutesHours(0, digitToDisplay);
      displayDigitMinute(digitToDisplay);
      /*
      switch (digitToDisplay) {
        case 10:
          displayDigit(1);
          displayDigit(0);
          break;
        case 11:
          displayDigit(1);
          displayDigit(1);
          break;
        case 12:
          displayDigit(1);
          displayDigit(2);
          break;
        default:
          displayDigit(digitToDisplay);
      }
      */
      delay(1000);
    }
  }
  // ----------------------------
  Serial.println(F("+ Run LED minute test, 0...59, using displayMinutesHours()."));
  writeMinuteHourShiftRegisters(0, 0, 0);
  delay(3000);
  // displayMinutesHours(byte theMinute, byte theHour)
  for (int digitToDisplay = 0; digitToDisplay < 60; digitToDisplay++) {
    displayMinutesHours(digitToDisplay, 0);
    delay(300);
  }
  // ----------------------------
  Serial.println(F("+ Run LED minute test, 0...59, using displayDigitMinute()."));
  writeMinuteHourShiftRegisters(0, 0, 0);
  delay(3000);
  // displayMinutesHours(byte theMinute, byte theHour)
  for (int digitToDisplay = 0; digitToDisplay < 60; digitToDisplay++) {
    displayDigitMinute(digitToDisplay);
    delay(300);
  }
  // ----------------------------
  Serial.println(F("+ Run LED minute light test using writeMinuteHourShiftRegisters()."));
  writeMinuteHourShiftRegisters(0, 0, 0);
  testByte1 = B00000000;
  for (int i = 0; i < 3; i++) {
    testByte1 = B00000001;
    for (int digitToDisplay = 0; digitToDisplay < 7; digitToDisplay++) {
      writeMinuteHourShiftRegisters(testByte1, 0, 0);
      testByte1 = testByte1 << 1;
      delay(100);
    }
    testByte1 = B01000000;
    for (int digitToDisplay = 6; digitToDisplay >= 0; digitToDisplay--) {
      writeMinuteHourShiftRegisters(0, 0, testByte1);
      testByte1 = testByte1 << 1;
      delay(100);
    }
  }
  // ----------------------------
  Serial.println(F("+ Print digit test using displayDigit(), from 0-99."));
  writeMinuteHourShiftRegisters(0, 0, 0);
  delay(3000);
  for (int digit1 = 0; digit1 < 10; digit1++) {
    for (int digit2 = 0; digit2 < 10; digit2++) {
      displayDigit(digit1);
      displayDigit(digit2);
      delay(100);
    }
  }
  Serial.println(F("+ Count down from 12 using displayDigit()."));
  // displayMinutesHours(0, 0); lights the 12th. hour LED.
  delay(3000);
  for (int digit1 = 0; digit1 < 13; digit1++) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  for (int digit1 = 12; digit1 > 0; digit1--) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  for (int digit1 = 0; digit1 < 13; digit1++) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  for (int digit1 = 12; digit1 > 0; digit1--) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  for (int digit1 = 0; digit1 < 13; digit1++) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  for (int digit1 = 12; digit1 > 0; digit1--) {
    displayMinutesHours(0, digit1);
    delay(100);
  }
  delay(3000);
  writeDigitShiftRegister(0);
  displayDigit(0);
  delay(3000);
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
  Serial.println("+ Segment display shift registers ready to use.");
  delay(300);
  // ------------------------------------------------------------
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  Serial.println(F("+ LED shift registers ready to use."));
  delay(300);

  // ----------------------------------------------------
  // Clear the digits.
  writeDigitShiftRegister(0);
  writeDigitShiftRegister(0);
  writeMinuteHourShiftRegisters(0, 0, 0);
  Serial.println(F("+ ALL LED lights and digit segments are off."));
  delay(1000);
  Serial.println(F("+ Cycle around the LED light hours."));
  for (int i = 0; i < 3; i++) {
    for (int digitToDisplay = 0; digitToDisplay < 13; digitToDisplay++) {
      displayMinutesHours(0, digitToDisplay);
      delay(100);
    }
  }
  delay(1000);

  // runDisplayTest();

  // ----------------------------------------------------
  if (!rtc.begin()) {
    Serial.println("--- Error: RTC not found.");
    while (1);
  }
  syncCountWithClock();
  displayMinutesHours( theCounterMinutes, theCounterHours );
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
