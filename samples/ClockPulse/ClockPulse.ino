// -----------------------------------------------------------------------------
/*
  Connect DS3231 Clock, and the LCD display, pins to the Nano:
  + VCC to 3.3-5.5V
  + GND to ground
  + SDA to D4 (pin 4) on Uno and Nano
  + SCL to D5 (pin 5) on Uno and Nano

  // -----------------------------------------------------------------------------
  DS3231 Clock Library:
  Filter your search by typing ‘rtclib’. There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
  Note, in the library source, uint8_t is the same as a byte: a type of unsigned integer of length 8 bits.
  Time and date units are are declared as, uint8_t.
*/
// -----------------------------------------------------------------------------
// For the clock board.

#include "RTClib.h"
RTC_DS3231 rtc;

char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

DateTime now;
int theCursor;
int thePrintRowDt = 0;

// -----------------------------------------------------------------------------
// For the LCD.

#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
int displayColumns = 16;

void displayPrintln(int theRow, String theString) {
  // To overwrite anything on the current line.
  String printString = theString;
  int theRest = displayColumns - theString.length();
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest < displayColumns) {
      printString = printString + " ";
      theRest++;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
}

// -----------------------------------------------------------------------------
int theCounterSeconds = 0;
int theCounterMinutes = 0;
int theCounterHours = 0;

void syncCountWithClock() {
  theCounterSeconds = now.second();
  theCounterMinutes = now.minute();
  theCounterHours = now.hour();
  //
  theCursor = 8;
  printClockInt(theCursor, 1, theCounterHours);  // Column, Row
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, 1, theCounterMinutes);
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, 1, theCounterSeconds);
  //
  Serial.print("+ syncCountWithClock,");
  Serial.print(" theCounterHours=");
  Serial.print(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
}

void printPulseHour() {
  Serial.print(" theCounterHours= ");
  Serial.println(theCounterHours);
  printClockInt( 8, 1, theCounterHours);
}

void printPulseMinute() {
  // Serial.print(" theCounterMinutes= ");
  // Serial.println(theCounterMinutes);
  printClockInt(11, 1, theCounterMinutes);
}

void printPulseSecond() {
  // Serial.print("+ theCounterSeconds = ");
  // Serial.println(theCounterSeconds);
  printClockInt(14, 1, theCounterSeconds);  // Column, Row
}

void printClockInt(int theColumn, int theRow, int theInt) {
  lcd.setCursor(theColumn, theRow);    // Column, Row
  if (theInt < 10) {
    lcd.print("0");
    lcd.setCursor(theColumn + 1, theRow);
  }
  lcd.print(theInt);
}

// -----------------------------------------------------------------------------
void printClockDate() {
  // --- To do: Day of the week.
  theCursor = 0;
  lcd.setCursor(theCursor, thePrintRowDt);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, thePrintRowDt);    // Column, Row
  lcd.print(":");
  printClockByte(++theCursor, thePrintRowDt, now.month());
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printClockByte(++theCursor, thePrintRowDt, now.day());
}

void printClockTime() {
  theCursor = 8;
  printClockByte(theCursor, thePrintRowDt, now.hour());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printClockByte(++theCursor, thePrintRowDt, now.minute());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printClockByte(++theCursor, thePrintRowDt, now.second());
}

void printClockByte(int theColumn, int theRow, char theByte) {
  int iByte = (char)theByte;
  lcd.setCursor(theColumn, theRow);    // Column, Row
  if (iByte < 10) {
    lcd.print("0");
    lcd.setCursor(theColumn + 1, theRow);
  }
  lcd.print(iByte);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // RTC: Real Time Clock
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    //
    // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
  //
  lcd.init();
  lcd.backlight();
  Serial.println("+ print: Hello there.");
  //                1234567890123456
  displayPrintln(0, "Hello there.");
  String theLine = "Start in 3 secs.";
  displayPrintln(1, theLine);
  delay(3000);
  lcd.clear();
  //
  // Set seconds
  now = rtc.now();
  printClockDate();
  syncCountWithClock();
  //
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(100);
  //
  now = rtc.now();
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a clock second pulse.
    theCounterSeconds = now.second();
    printPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a clock minute pulse.
      theCounterMinutes = now.minute();
      printPulseMinute();
      if (theCounterMinutes == 0) {
        // When the clock minute value changes to zero, that's a clock hour pulse.
        theCounterHours = now.hour();
        printPulseHour();
        if (now.hour() == 0) {
          // When the clock hour value changes to zero, that's a clock day pulse.
          // Reprint the date, at time: 00:00:00.
          printClockDate();
        }
      }
    }
    printClockTime();
  }

  // -----------------------------------------------------------------------------
}
