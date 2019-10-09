 // -----------------------------------------------------------------------------
/*
  Connect DS3231 Clock, and the LCD display, pins to the Nano:
  + VCC to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + SDA to Nano D4 (pin 4), same on Uno.
  + SCL to Nano D5 (pin 5), same on Uno.

  The BUTTON circuit:
  - Board is either an Arduino Uno, Nano, or a NodeMCU.
  - LED positive is connected to onboard LED pin.
  - Button side 1, connected to Arduino +5V or NodeMCU +3.3V.
  - Button side 2, connected to a 10K resistor which is connected to ground.
  - Button side 2, connected to board pin (BUTTON_PIN), example: D11 on Nano.

  // -----------------------------------------------------------------------------
  DS3231 Clock Library:
  Filter your search by typing ‘rtclib’. There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
  Note, the library uses uint8_t, which is the same as a byte: an unsigned 8 bit integer.
  Time and date units are are declared as, uint8_t.
*/
// -----------------------------------------------------------------------------
// For the clock board.

#include "RTClib.h"

RTC_DS3231 rtc;

DateTime now;

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

int theCursor;
const int printRowClockDate = 0;
const int printColClockDate = 0;
const int printRowClockPulse = 0;
const int thePrintColHour = 8;
const int thePrintColMin = thePrintColHour + 3;
const int thePrintColSec = thePrintColMin + 3;

int theCounterSeconds = 0;
int theCounterMinutes = 0;
int theCounterHours = 0;

void syncCountWithClock() {
  theCounterSeconds = now.second();
  theCounterMinutes = now.minute();
  theCounterHours = now.hour();
  //
  theCursor = thePrintColHour;
  printClockInt(theCursor, printRowClockPulse, theCounterHours);  // Column, Row
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterMinutes);
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterSeconds);
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
  }
}

void printPulseHour() {
  Serial.print("+ printPulseHour(), theCounterHours= ");
  Serial.println(theCounterHours);
  printClockInt(thePrintColHour, printRowClockPulse, theCounterHours);
}

void printPulseMinute() {
  // Serial.print(" theCounterMinutes= ");
  // Serial.println(theCounterMinutes);
  printClockInt(thePrintColMin, printRowClockPulse, theCounterMinutes);
}

void printPulseSecond() {
  // Serial.print("+ theCounterSeconds = ");
  // Serial.println(theCounterSeconds);
  printClockInt(thePrintColSec, printRowClockPulse, theCounterSeconds);  // Column, Row
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
char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

void printClockDate() {
  theCursor = printColClockDate;
  lcd.setCursor(theCursor, printRowClockDate);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, printRowClockDate);    // Column, Row
  lcd.print(":");
  printClockByte(++theCursor, printRowClockDate, now.month());
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printClockByte(++theCursor, printRowClockDate, now.day());
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
// Toggle the LCD backlight each time the button is pressed.

const int BUTTON_PIN = 2;     // Nano D2
boolean theToggle = true;
boolean buttonAction = true;  // Case the button is pressed and held, only toggle once.

void toggleLcdBacklight() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (buttonAction) {
      if (theToggle) {
        theToggle = false;
        Serial.println("+ toggleButton(), turn off.");
        lcd.noBacklight(); // Backlight off
      } else {
        theToggle = true;
        Serial.println("+ toggleButton(), turn on.");
        lcd.backlight(); // backlight on
      }
    }
    buttonAction = false;
  } else {
    buttonAction = true;
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // RTC: Real Time Clock
  if (!rtc.begin()) {
    Serial.println("--- Did not find RTC.");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, reset the time.");
    // Set the RTC to the date & time this sketch was compiled, which is only seconds behind the actual time.
    // While not exact, it is close to actual.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // To manually adjust the time.
  // Uncomment out the rtc line.
  // Set to the next minute's time.
  // Upload the change.
  // Turn the circuit off.
  // Start the circuit at 0 seconds. The 3 seconds is the time to reach this program step.
  // Once set, comment out the line and upload the change.
  // rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds

  lcd.init();
  lcd.backlight(); // backlight on
  //
  Serial.println("+ print: Hello there.");
  //                 1234567890123456
  displayPrintln(0, "Hello there.");
  String theLine = "Start in 3 secs.";
  displayPrintln(1, theLine);
  delay(3000);
  lcd.clear();

  // Set seconds
  now = rtc.now();
  printClockDate();
  syncCountWithClock();
  //
  pinMode(BUTTON_PIN, INPUT);
  //
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(100);
  //
  toggleLcdBacklight();
  processClockNow();

  // -----------------------------------------------------------------------------
}
