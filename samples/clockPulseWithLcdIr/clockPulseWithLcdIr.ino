// -----------------------------------------------------------------------------
/*
  Connect the DS3231 Clock and the 1602 LCD display, to the Nano:
  + VCC to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + SDA to Nano D4 (pin 4), same on Uno.
  + SCL to Nano D5 (pin 5), same on Uno.

  Connect infrared receiver, pins from top left to right:
    Left most (next to the X) - Nano pin 9
    Center - 5V
    Right  - ground

    9 + -   - Nano connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------

  // -----------------------------------------------------------------------------
  DS3231 Clock Library:
  Filter your search by typing ‘rtclib’. There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
  Note, the library uses uint8_t, which is the same as a byte: an unsigned 8 bit integer.
  Time and date units are are declared as, uint8_t.
*/
// -----------------------------------------------------------------------
// For the infrared receiver.
#include <IRremote.h>
int IR_PIN = 9;
IRrecv irrecv(IR_PIN);
decode_results results;

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
  // Serial.print("*theRest=");
  // Serial.print(theRest);
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest > 0) {
      printString = printString + " ";
      theRest--;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
  /*
    Serial.print(":theString=");
    Serial.print(theString);
    Serial.print(":printString=");
    Serial.print(printString);
    Serial.print(":theRest=");
    Serial.print(theRest);
    Serial.println("*");
    // Hello there.    :
    // 1234567890123456:
  */
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

int theCursor;
const int printRowClockDate = 0;
const int printColClockDate = 0;
const int printRowClockPulse = 0;
const int thePrintColHour = 8;
const int thePrintColMin = thePrintColHour + 3;
const int thePrintColSec = thePrintColMin + 3;

// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

void syncCountWithClock() {
  theCounterYear = now.year();
  theCounterMonth = now.month();
  theCounterDay = now.day();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
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
// Toggle the LCD backlight on/off.
boolean theToggle = true;
void toggleLcdBacklight() {
  if (theToggle) {
    theToggle = false;
    Serial.println("+ Toggle: off.");
    lcd.noBacklight();
  } else {
    theToggle = true;
    Serial.println("+ Toggle: on.");
    lcd.backlight();
  }
}

// -----------------------------------------------------------------------
// Menu to set the clock.

int theSetRow = 1;
int theSetCol = 0;
int theSetMin = 0;
int theSetMax = 59;
int setValue = 0;
int setClockValue = 0;
void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF5AA5:
    case 0xE0E046B9:
      Serial.print("+ Key > - next");
      Serial.print(", set clock value");
      setClockValue++;
      if (setClockValue > 6) {
        setClockValue = 0;
      }
      switch (setClockValue) {
        case 0:
          Serial.print("Cancel set");
          displayPrintln(theSetRow, "");
          break;
        case 1:
          Serial.print("seconds");
          displayPrintln(theSetRow, "Set:");
          theSetMax = 59;
          theSetMin = 0;
          theSetCol = thePrintColSec;
          setValue = theCounterSeconds;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
        case 2:
          Serial.print("minutes");
          displayPrintln(theSetRow, "Set:");
          theSetMax = 59;
          theSetMin = 0;
          theSetCol = thePrintColMin;
          setValue = theCounterMinutes;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
        case 3:
          Serial.print("hours");
          displayPrintln(theSetRow, "Set:");
          theSetMax = 24;
          theSetMin = 0;
          theSetCol = thePrintColHour;
          setValue = theCounterHours;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
        case 4:
          Serial.print("day");
          displayPrintln(theSetRow, "Set day:");
          theSetMax = 31;
          theSetMin = 1;
          theSetCol = thePrintColMin;
          setValue = theCounterDay;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
        case 5:
          Serial.print("month");
          displayPrintln(theSetRow, "Set month:");
          theSetMax = 12;
          theSetMin = 1;
          theSetCol = thePrintColMin;
          setValue = theCounterMonth;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
        case 6:
          Serial.print("year");
          displayPrintln(theSetRow, "Set year:");
          theSetMax = 2525; // In the year 2525, If man is still alive, If woman can survive...
          theSetMin = 1795; // Year John Keats the poet was born.
          theSetCol = thePrintColMin;
          setValue = theCounterYear;
          printClockInt(theSetCol, theSetRow, setValue);
          break;
      }
      Serial.println(".");
      break;
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.print("+ Key < - previous");
      Serial.println(".");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      Serial.print("+ Key up");
      if (setClockValue) {
        Serial.print(", increment");
        setValue++;
        if (setValue > theSetMax) {
          setValue = theSetMin;
        }
        printClockInt(theSetCol, theSetRow, setValue);
      }
      Serial.println(".");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.print("+ Key down");
      if (setClockValue) {
        Serial.print(", decrement");
        setValue--;
        if (setValue < theSetMin) {
          setValue = theSetMax;
        }
        printClockInt(theSetCol, theSetRow, setValue);
      }
      Serial.println(".");
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      Serial.print("+ Key OK");
      if (setClockValue) {
        Serial.print(", set ");
        switch (setClockValue) {
          case 1:
            Serial.print("seconds");
            theCounterSeconds = setValue;
            printClockInt(theSetCol, printRowClockPulse, setValue);
            break;
          case 2:
            Serial.print("minutes");
            theCounterMinutes = setValue;
            printClockInt(theSetCol, printRowClockPulse, setValue);
            break;
          case 3:
            Serial.print("hours");
            theCounterHours = setValue;
            printClockInt(theSetCol, printRowClockPulse, setValue);
            break;
          case 4:
            Serial.print("day");
            theCounterDay = setValue;
            printClockDate();
            break;
          case 5:
            Serial.print("month");
            theCounterMonth = setValue;
            printClockDate();
            break;
          case 6:
            Serial.print("year");
            theCounterYear = setValue;
            break;
        }
        rtc.adjust(DateTime(theCounterYear, theCounterMonth, theCounterDay, theCounterHours, theCounterMinutes, theCounterSeconds));
        displayPrintln(theSetRow, "Value is set.");
        delay(2000);
        displayPrintln(theSetRow, "");
      }
      Serial.println(".");
      //
      setClockValue = false;
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.print("+ Key * (Return): ");
      if (setClockValue) {
        Serial.println("Cancel set.");
        displayPrintln(theSetRow, "");
        setClockValue = false;
      }
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.print("+ Key # (Exit): ");
      Serial.println("Toggle display on/off.");
      toggleLcdBacklight();
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0xFFA25D:
    case 0xE0E020DF:
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("");
      break;
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("");
      break;
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("");
      break;
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("");
      break;
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("");
      break;
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      Serial.println("");
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      Serial.println(results.value, HEX);
      // -----------------------------------
  } // end switch

}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // RTC: Real Time Clock
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, need to reset the time.");
    // Set the RTC to the date & time this sketch was compiled, which is only seconds behind the actual time.
    // While not exact, it is close to actual.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // --- To manually adjust the time ---
  // Uncomment out the rtc line.
  // Set to the next minute's time.
  // Upload the change.
  // Turn the circuit off.
  // Start the circuit at 0 seconds. The 3 seconds is the time to reach this program step.
  // Once set, comment out the line and upload the change.
  // rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
  Serial.println("+ Clock set.");

  lcd.init();
  lcd.backlight(); // backlight on
  //
  Serial.println("+ Print, Hello there, to LCD.");
  //                 1234567890123456
  displayPrintln(0, "Hello there.");
  displayPrintln(1, "Start in 3 secs.");
  delay(3000);
  lcd.clear();
  //
  now = rtc.now();
  printClockDate();
  syncCountWithClock();

  irrecv.enableIRIn();
  Serial.println("+ Infrared receiver enabled.");

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(100);

  processClockNow();

  // -------------------------------
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }

  // -----------------------------------------------------------------------------
}
