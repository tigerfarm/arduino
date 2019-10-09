 // -----------------------------------------------------------------------------
/*
  Connect DS3231 Clock, and the LCD display, pins to the Nano:
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

// -----------------------------------------------------------------------
void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.println("+ Key < - previous");
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      Serial.println("+ Key > - next");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      Serial.println("+ Key up");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.println("+ Key down");
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      Serial.println("+ Key OK - Toggle");
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0xFFA25D:
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
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.println("+ Key * (Return)");
      toggleLcdBacklight();
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.println("+ Key # (Exit)");
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
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Or, set the RTC with an explicit date & time
    // rtc.adjust(DateTime(2019, 10, 9, 13, 42, 20));   // year, month, day, hour, minute, seconds
  }
  // Uncomment out to manually adjust the time.
  // Start the upload at 0 seconds.
  rtc.adjust(DateTime(2019, 10, 9, 13, 46, 16));   // year, month, day, hour, minute, seconds

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

  irrecv.enableIRIn();

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(100);
  //
  toggleLcdBacklight();
  processClockNow();

  // ---------------------------------------------------------------------
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }

  // -----------------------------------------------------------------------------
}
