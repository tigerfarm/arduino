// -----------------------------------------------------------------------------
/*
   When syncing with a clock, start 8 seconds before the top,
    i.e. turn the power on at 52 seconds.

  Connect LCD to Nano:
    SDA - A4
    SCL - A5

  DS3231 Clock adn the LCD display, pins:
  + VCC to 3.3-5.5V
  + GND to ground
  + SDA to D4 (pin 4) on Uno and Nano
  + SCL to D5 (pin 5) on Uno and Nano

  // -----------------------------------------------------------------------------
  DS3231 Clock

  Reference URL:
  https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/

  The DS3231 has an internal Temperature Compensated Crystal Oscillator(TCXO) which isn’t affected by temperature.
  It is accurate to a few minutes per year.
  The battery, a CR2032, can keep the RTC running for over 8 years without an external 5V power supply.
  Another source said, the battery will keep the clock going for over 1 year.
  The 24C32 EEPROM (32K pin) uses I2C interface for communication and shares the same I2C bus as DS3231.

  Library:
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

int theCursor;
char charBuffer[4];
// int thePrintRowTmp = 0;
int thePrintRowDt = 0;
DateTime now;

// -----------------------------------------------------------------------------
// For the LCD.

#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
int displayColumns = 16;
//                        1234567890123456
String clearLineString = "                ";

// -----------------------------------------------------------------------------
int theCounterSeconds = 0;
int theCounterMinutes = 0;
int syncCounterMinutes = 0;

void syncCountWithClock() {
  int clockSeconds = now.second();
  int clockMinutes = now.minute();
  theCounterSeconds = ++clockSeconds;
  theCounterMinutes = clockMinutes;
  Serial.print("+ syncCountWithClock, theCounterSeconds=");
  Serial.print(theCounterSeconds);
  Serial.print(" clockSeconds=");
  Serial.print(clockSeconds);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" clockMinutes=");
  Serial.println(clockMinutes);
}

void displayOneSecondCount() {
  //
  // With a delay of ?, my Arduino Nano is a small bit slower than actual time.
  delay(957);
  //
  if (theCounterSeconds >= 59) {
    theCounterSeconds = 0;
    theCounterMinutes++;
    syncCounterMinutes++;
    // if (theCounterMinutes == 60) {
    //   syncCountWithClock();
    // }
  }
  if (syncCounterMinutes == 3) {
    syncCounterMinutes = 0;
    syncCountWithClock();
  } else {
    theCounterSeconds++;
  }
  theCursor = 11;
  printClockInt(theCursor, 1, theCounterMinutes);
  // ---
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, 1, theCounterSeconds);  // Column, Row

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

// -----------------------------------------------------------------------------
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
  syncCountWithClock();
  theCounterSeconds--;
  //
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  now = rtc.now();
  printClockDate();
  printClockTime();
  //
  displayOneSecondCount();

  // -----------------------------------------------------------------------------
}
