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
  Another source said, the battery will keep the clock going for over 1 year.
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

char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

// -----------------------------------------------------------------------------
// For the LCD.

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

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, need to reset the time.");
    // Set the RTC to the date & time this sketch was compiled, which is only seconds behind the actual time.
    // While not exact, if compiled before running, it is close to actual.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // --- To manually adjust the time ---
  // Serial.println("+ Manually set real time clock time.");
  // Uncomment out the rtc line.
  // Set to the next minute's time.
  // Upload the change.
  // Turn the circuit off.
  // Start the circuit at 0 seconds. The 3 seconds is the time to reach this program step.
  // Once set, comment out the line and upload the change.
  // rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  displayPrintln(0, "Tmp:");
  
  Serial.println("+ Start looping.");
}

int theCursor;
char charBuffer[4]; // For temperature calculations
int thePrintRowTmp = 0;
int thePrintRowDt = 1;
void loop ()
{
  delay(1000);

  // -----------------------------------------------------------------------------
  // Temperature
  
  float celcius = rtc.getTemperature() - 2.25; // subtract to get the actual for my specific DS3231.
  float fahrenheit = celcius * 9.0 / 5.0 + 32.0;
  //
  theCursor = 5;
  lcd.setCursor(theCursor, thePrintRowTmp);    // Column, Row
  dtostrf(celcius, 4, 1, charBuffer);
  lcd.print(charBuffer);
  theCursor = theCursor + 4;
  lcd.setCursor(theCursor, thePrintRowTmp);    // Column, Row
  lcd.print("c ");
  //
  theCursor = theCursor + 2;
  lcd.setCursor(the, thePrintRowTmp);
  dtostrf(fahrenheit, 4, 1, charBuffer);
  lcd.print(charBuffer);
  theCursor = theCursor + 4;
  lcd.setCursor(the, thePrintRowTmp);    // Column, Row
  lcd.print("f");

  // -----------------------------------------------------------------------------
  // Date

  DateTime now = rtc.now();

  // --- To do: Day of the week.
  theCursor = 0;
  lcd.setCursor(theCursor, thePrintRowDt);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, thePrintRowDt);    // Column, Row
  lcd.print(":");
  printByte(++theCursor, thePrintRowDt, now.month());
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printByte(++theCursor, thePrintRowDt, now.day());
  
  // -----------------------------------------------------------------------------
  // Time
  
  theCursor = 8;
  printByte(theCursor, thePrintRowDt, now.hour());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printByte(++theCursor, thePrintRowDt, now.minute());
  // ---
  theCursor = theCursor + 2;
  lcd.print(":");
  printByte(++theCursor, thePrintRowDt, now.second());

  // -----------------------------------------------------------------------------
}
