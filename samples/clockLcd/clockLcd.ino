/*
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
  The 24C32 EEPROM (32K pin) uses I2C interface for communication and shares the same I2C bus as DS3231.

  Library:
  Filter your search by typing ‘rtclib’. There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
  Note, in the library source, uint8_t is the same as a byte: a type of unsigned integer of length 8 bits.
  Time and date units are are declared as, uint8_t.
*/
// -----------------------------------------------------------------------------
#include "RTClib.h"
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

// -----------------------------------------------------------------------------
// For the display

#include <Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int displayColumns = 16;
//                        1234567890123456
String clearLineString = "                ";

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

void printByte(int theColumn, int theRow, char theByte) {
  int iByte = (char)theByte;
  lcd.setCursor(theColumn, theRow);    // Column, Row
  if (iByte<10) {
    lcd.print("0");
    lcd.setCursor(theColumn+1, theRow);
  }
  lcd.print(iByte);
}

// -----------------------------------------------------------------------------
void setup ()
{
  Serial.begin(9600);
  delay(1000);        // wait for console opening
  Serial.println("+++ Setup.");

  lcd.init();
  lcd.backlight();
  Serial.println("+ print: Hello there.");
  //                 1234567890123456
  displayPrintln(0, "Hello there.");
  delay(2000);

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

  lcd.setCursor(0, 0);
  lcd.print(clearLineString);
  displayPrintln(0, "Tmp:");
}

int theCursor;
char charBuffer[4];
void loop ()
{
  delay(1000);

  // -----------------------------------------------------------------------------
  // Temperature
  
  float celcius = rtc.getTemperature() - 2.5; // subtract to get the actual.
  float fahrenheit = celcius * 9.0 / 5.0 + 32.0;
  //
  lcd.setCursor(5, 0);    // Column, Row
  dtostrf(celcius, 4, 1, charBuffer);
  lcd.print(charBuffer);
  lcd.setCursor(9, 0);    // Column, Row
  lcd.print("c");
  //
  lcd.setCursor(11, 0);
  dtostrf(fahrenheit, 4, 1, charBuffer);
  lcd.print(charBuffer);
  lcd.setCursor(15, 0);    // Column, Row
  lcd.print("f");

  // -----------------------------------------------------------------------------
  // Date

  DateTime now = rtc.now();

  // --- To do: Day of the week.
  theCursor = 0;
  lcd.setCursor(theCursor, 1);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, 1);    // Column, Row
  lcd.print(":");
  printByte(++theCursor, 1, now.month());
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printByte(++theCursor, 1, now.day());
  
  // -----------------------------------------------------------------------------
  // Time
  
  theCursor = 8;
  printByte(theCursor, 1, now.hour());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printByte(++theCursor, 1, now.minute());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printByte(++theCursor, 1, now.second());

  // -----------------------------------------------------------------------------
}
