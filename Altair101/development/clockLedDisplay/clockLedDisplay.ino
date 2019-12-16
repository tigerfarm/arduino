// -----------------------------------------------------------------------------
/*
  Clock with LED lights for diplay.
*/
// ------------------------------------------------------------------------
byte programCounter = B11111111;
byte dataByte       = B11111111;
byte statusByte     = B11111111;

// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const PROGMEM int dataPinLed = 7;     // pin 14 Data pin.
const PROGMEM int latchPinLed = 8;    // pin 12 Latch pin.
const PROGMEM int clockPinLed = 9;    // pin 11 Clock pin.

void lightsStatusAddressData( byte status8bits, byte address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  // MSBFIRST matches the bit to LED mapping.
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, address16bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  digitalWrite(latchPinLed, HIGH);
}

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

byte theCounterHours = 0;
byte theCounterMinutes = 0;
byte theCounterSeconds = 0;

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
  //
  now = rtc.now();
  //
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a clock second pulse.
    theCounterSeconds = now.second();
    // clockPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a clock minute pulse.
      theCounterMinutes = now.minute();
      if (theCounterMinutes != 0) {
        clockPulseMinute();
      } else {
        // When the clock minute value changes to zero, that's a clock hour pulse.
        theCounterHours = now.hour();
        clockPulseHour();
      }
    }
  }
}

int theHour = 0;
void clockPulseHour() {
  Serial.print("++ clockPulseHour(), theCounterHours= ");
  Serial.println(theCounterHours);
  Serial.println(theCounterHours);
  // Use AM/PM rather than 24 hours.
  if (theCounterHours > 12) {
    theHour = theCounterHours - 12;
  } else if (theCounterHours == 0) {
    theHour = 12; // 12 midnight, 12am
  } else {
    theHour = theCounterHours;
  }
  displayTheTime( theCounterMinutes, theHour );
}
void clockPulseMinute() {
  Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
  Serial.println(theCounterMinutes);
  displayTheTime( theCounterMinutes, theHour );
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
  } else {
    // There are 3 bits for the tens: 0, 10, 20, 30, 40, or 50.
    // There are 4 bits for the ones: 0 ... 9.
    //                         Tens & Minutes: B-tttmmmm
    //                                         B00001111 = 2 ^ 4 = 16
    // theMinute = 10, theBinaryMinute = 00010000
    theMinuteTens = theMinute / 10;
    theMinuteOnes = theMinute - theMinuteTens * 10;
    theBinaryMinute = 16 * theMinuteTens + theMinuteOnes;
  }

  // ----------------------------------------------
  // Convert the hour into binary for display.
  switch (theHour) {
    case 1:
      //                 B11111111
      theBinaryHour1 = B00000010;
      theBinaryHour2 = 0;
      break;
    case 2:
      //                 B11111111
      theBinaryHour1 = B00000100;
      theBinaryHour2 = 0;
      break;
    case 3:
      //                 B11111111
      theBinaryHour1 = B00001000;
      theBinaryHour2 = 0;
      break;
    case 4:
      //                 B11111111
      theBinaryHour1 = B00010000;
      theBinaryHour2 = 0;
      break;
    case 5:
      //                 B11111111
      theBinaryHour1 = B00100000;
      theBinaryHour2 = 0;
      break;
    case 6:
      //                 B11111111
      theBinaryHour1 = B01000000;
      theBinaryHour2 = 0;
      break;
    case 7:
      //                 B11111111
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000010;
      break;
    case 8:
      //                 B11111111
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000100;
      break;
    case 9:
      //                 B11111111
      theBinaryHour1 = 0;
      theBinaryHour2 = B00001000;
      break;
    case 10:
      //                 B11111111
      theBinaryHour1 = 0;
      theBinaryHour2 = B00010000;
      break;
    case 11:
      //                 B11111111
      theBinaryHour1 = 0;
      theBinaryHour2 = B00100000;
      break;
    case 12:
      //                 B11111111
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

  // ----------------------------------------------------
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  lightsStatusAddressData(0, 0, 0);
  Serial.println(F("+ Front panel LED shift registers ready."));

  // ----------------------------------------------------
  syncCountWithClock();
  Serial.println("+ Clock set and synched with program variables.");

  // ----------------------------------------------------
  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Loop for displaying hours and minutes.

void loop() {
  delay(100);
  processClockNow();
}
// -----------------------------------------------------------------------------
