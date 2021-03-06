// -------------------------------------------------------------------------------
/*
  This is a clock control program.
  Functionality:
    ...

  DS3231 - A real time clock hardware module
              that is controlled by this program.

  -----------------------------------------------------------------------------
  Connect the DS3231 Clock and the 1602 LCD display, to the Nano:
  + VCC to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + SDA to Nano D4 (pin 4), same on Uno.
  + SCL to Nano D5 (pin 5), same on Uno.

  ------------------------------------------------------------------------------
  DS3231 Clock Library:
    Filter your search by typing ‘rtclib’.
    There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib

  --------------------------------------------------------------------------------
  Next to implement,

  --------------------------------------------------------------------------------
  Clock module pins

  -----------------------
 |     Curcuit           |
 |     side              |
  -----------------------
   |   |   |   |   |   |
  32K SQW SCL SDA VCC GND

  -----------------------
 |     Battery           |
 |     side              |
  -----------------------
   |   |   |   |   |   |
  GND VCC SDA SCL SQW 32K

  --------------------------------------------------------------------------------

  Infrared receiver pins

   A1 + -   - Arduino pin connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------

*/
// -------------------------------------------------------------------------------

#include "Altair101b.h"

String clockPrompt = "RTC ?- ";
extern int programState;

// -------------------------------------------------------------------------------
// Motherboard Specific setup for Infrared communications

// -----------------------------------------------
#if defined(__AVR_ATmega2560__)
// ------------------
#include <IRremote.h>
// ------------------
// Mega uses a hardware serial port (RX/TX) for communications with the DFPlayer.
// For Arduino Mega, I use pin 24 because it's closer to where I'm doing my wiring.
//  Feel free to use another digital or analog pin.
int IR_PIN = 24;
//
// -----------------------------------------------
#elif defined(__SAM3X8E__)
// ------------------
#include <IRremote2.h>    // Special infrared library for the Due.
// For Arduino Due, I use pin 24 because it's closer to where I'm doing my wiring.
//  Feel free to use another digital or analog pin.
int IR_PIN = 24;
// ------------------
// Due uses a hardware serial port (RX/TX) for communications with the DFPlayer.
//
// -----------------------------------------------
#else
// ------------------
#include <IRremote.h>
// Digital and analog pins work. Also tested with other analog pins.
int IR_PIN = A1;
#endif

// -------------------------------------------------------------------------------
// Infrared Receiver

IRrecv irrecv(IR_PIN);
decode_results results;

// -------------------------------------------------------------------------------
// DS3231 Clock

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// -----------------------------------------------------------------------------
// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
//
int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

void syncCountWithClock() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  /*
  theCursor = thePrintColHour;
  printClockInt(theCursor, printRowClockPulse, theCounterHours);  // Column, Row
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterMinutes);
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterSeconds);
  */
  //
  Serial.print("+ syncCountWithClock,");
  Serial.print(" theCounterHours=");
  Serial.print(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
  //
  printClockDate();
}

// -----------------------------------------------------------------------------
char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

void printClockDate() {
  now = rtc.now();
  theCounterYear = now.year();
  theCounterMonth = now.month();
  theCounterDay = now.day();
  //
  /*
  theCursor = printColClockDate;
  lcd.setCursor(theCursor, printRowClockDate);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, printRowClockDate);    // Column, Row
  lcd.print(":");
  printClockInt(++theCursor, printRowClockDate, theCounterMonth);
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printClockInt(++theCursor, printRowClockDate, theCounterDay);
  */
}

// -----------------------------------------------------------------------------
void processClockNow() {
  //
  now = rtc.now();
  //
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a clock second pulse.
    theCounterSeconds = now.second();
    clockPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a clock minute pulse.
      theCounterMinutes = now.minute();
      clockPulseMinute();
      if (theCounterMinutes == 0) {
        // When the clock minute value changes to zero, that's a clock hour pulse.
        theCounterHours = now.hour();
        clockPulseHour();

        // -------------------------
        // Date pulses.
        if (now.hour() == 0) {
          // When the clock hour value changes to zero, that's a clock day pulse.
          printClockDate(); // Prints and sets the values for day, month, and year.
          clockPulseDay();
          if (theCounterDay == 1) {
            // When the clock day value changes to one, that's a clock month pulse.
            clockPulseMonth();
            if (theCounterMonth == 1) {
              // When the clock Month value changes to one, that's a clock year pulse.
              clockPulseYear();
            }
          }
        }
        // -------------------------
      }
    }
  }
}

void clockPulseYear() {
  Serial.print("+++++ clockPulseYear(), theCounterYear= ");
  Serial.println(theCounterYear);
}
void clockPulseMonth() {
  Serial.print("++++ clockPulseMonth(), theCounterMonth= ");
  Serial.println(theCounterMonth);
}
void clockPulseDay() {
  Serial.print("+++ clockPulseDay(), theCounterDay= ");
  Serial.println(theCounterDay);
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
  // printClockInt(thePrintColHour, printRowClockPulse, theHour);
}
void clockPulseMinute() {
  Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
  Serial.println(theCounterMinutes);
  // printClockInt(thePrintColMin, printRowClockPulse, theCounterMinutes);
}
void clockPulseSecond() {
  Serial.print("+ theCounterSeconds = ");
  Serial.println(theCounterSeconds);
  // printClockInt(thePrintColSec, printRowClockPulse, theCounterSeconds);  // Column, Row
}

// -----------------------------------------------------------------------------
// Initialize the player module.
// This allows it to be reset after the computer is restarted.
//
void setupClock() {
  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized: infrared receiver."));

  // Initialize the Real Time Clock (RTC).
  if (!rtc.begin()) {
    Serial.println("--- Error: RTC not found.");
    while (1);
  }
  //
  // Set the time for testing. Example, test for testing AM/PM.
  // rtc.adjust(DateTime(2019, 10, 22, 23, 59, 56)); // DateTime(year, month, day, hour, minute, second)
  // delay(100);
  //
    Serial.print(F("+ Initialized: clock."));
}

void printClockData() {
  DateTime now = rtc.now();
  Serial.print("+ Current Date: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print("  Time: ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

// -----------------------------------------------------------------------
// Infrared DFPlayer controls

void clockSwitch(int resultsValue) {
  boolean printPrompt = true;
  switch (resultsValue) {
    // -----------------------------------
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    // Mouse wheel
    case 27:
    case 91:
    case 65: // Mouse wheel down (away from user).
    case 66: // Mouse wheel up   (toward user).
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 10:
    case 13:
      // CR/LF.
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Song control
    //
    // -----------------------------------
    case 0xFF10EF :
    case 0x7E23117B:    // Key REW
    case 'p' :
      Serial.print(F("+ Player, Previous: play previous song, playerCounter="));
      Serial.println();
      break;
    // -----------------------------------
    case 0xFF5AA5:
    case 0x7538143B:    // Key FF
    case 'n':
      Serial.print(F("+ Player, Next: play next song, playerCounter="));
      Serial.println();
      break;
    // -----------------------------------
    case 0xFA2F715F:    // Key STOP
    case 0x2C22119B:    // Key PAUSE/STILL
    case 0xFF9867:      // Small remote, key 0
    case 's':
      Serial.print(F("+ Pause, play current song, playerCounter="));
      break;
    case 0x8AA3C35B:    // Key PLAY
    case 'r':
          Serial.print(F("+ Key PLAY"));
        Serial.println();
        printClockData();
        break;
    // -----------------------------------
    case 0xFF38C7:
    case 0x82D6EC17:
      Serial.print("+ Key OK|Enter - Toggle: pause|start the song, playerCounter=");
        Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Folder, file directory selection.
    //
    // -----------------------------------
    case 0x6D8BBC17:                        // After pressing VCR
    case 0xAD680D1B:                        // After pressing TV
    case 0xFF18E7:                          // Small remote key up.
    case 0xFF629D:                          // Small remote key 2.
    case 'D':
      Serial.print("+ Key up - next directory, directory number: ");
      Serial.println();
      break;
    // -----------------------------------
    case 0xCDFC965B:                        // After pressing VCR
    case 0xDD8E75F:                         // After pressing TV
    case 0xFF4AB5:                          // Small remote key down.
    case 'd':
      Serial.print("+ Key down - previous directory, directory number: ");
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Loop a single song
    //
    case 0xA02E4EBF:
    case 0xFF6897:
    case 'L':
      Serial.println("+ Key *|A.Select - Loop on: loop this single MP3.");
      break;
    case 0xC473DE3A:
    case 0xFFB04F:
    case 'l':
      Serial.println("+ Key #|Eject - Loop off: Single MP3 loop is off.");
      break;
    // -----------------------------------
    // Toshiba VCR remote, not programmed.
    case 0x718E3D1B:                        // Toshiba VCR remote
    case 0xB16A8E1F:                        // After pressing TV
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xF8FB71FB:                        // Toshiba VCR remote
    case 0x38D7C2FF:                        // After pressing TV
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xE9E0AC7F:                        // Toshiba VCR remote
    case 0x29BCFD83:                        // After pressing TV
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    //
    // ----------------------------------------------------------------------
    // Equalizer setting selection.
    //
    case 0x38BF129B:                        // Toshiba VCR remote
    case 0x789B639F:                        // After pressing TV
    case 0xFF22DD:                          // Small remote
    case '4':
      Serial.print("+ Key 4: ");
      Serial.println();
      break;
    case 0x926C6A9F:                        // Toshiba VCR remote
    case 0xD248BBA3:                        // After pressing TV
    case 0xFF02FD:
    case '5':
      Serial.print("+ Key 5: ");
      Serial.println();
      break;
    case 0xE66C5C37:                        // Toshiba VCR remote
    case 0x2648AD3B:                        // After pressing TV
    case 0xFFC23D:
    case '6':
      Serial.print("+ Key 6: ");
      Serial.println();
      break;
    case 0xD75196BB:                        // Toshiba VCR remote
    case 0x172DE7BF:                        // After pressing TV
    case 0xFFE01F:
    case '7':
      Serial.print("+ Key 7: ");
      Serial.println();
      break;
    case 0x72FD3AFB:                        // Toshiba VCR remote
    case 0xB2D98BFF:                        // After pressing TV
    case 0xFFA857:
    case '8':
      Serial.print("+ Key 8: ");
      Serial.println();
      break;
    case 0xCCAA92FF:                        // Toshiba VCR remote
    case 0xC86E403:                         // After pressing TV
    case 0xFF906F:
    case '9':
      Serial.print("+ Key 9: ");
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Volume
    //
    case 0x2B8BE5F:
    case 0xFFE21D:              // Small remote, Key 3
    case 'V':
      Serial.print("+ Key Volume ^");
      Serial.println();
      break;
    case 0x1CF3ACDB:
    case 0xFFA25D:              // Small remote, Key 1
    case 'v':
      Serial.print("+ Key Volume v");
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ MP3 Player Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ s, STOP         Pause, stop playing."));
      Serial.println(F("+ r, RUN          Start, playing the current song."));
      Serial.println(F("+ R, RESET/CLEAR  Reset and set to play first song."));
      Serial.println(F("+ n/p, Play song  Play next/previous song."));
      Serial.println(F("+ d/D, Directory  Play previous directory."));
      Serial.println(F("+ l/L, Loop       Disable/Enable loop the current song."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("--- Equalizer options:"));
      Serial.println(F("+ 4 POP   5 CLASSIC  6 NORMAL"));
      Serial.println(F("+ 7 ROCK  8 JAZZ     9 BASS"));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("------------------"));
      Serial.println(F("+ i, Information  Program variables and hardward values."));
      // Serial.println(F("+ x, EXAMINE      Not implemented. Play specified song number."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 0x953EEEBC:                              // Key CLEAR
    case 'R':
      Serial.println(F("+ Player CLEAR/RESET, play first song."));
      break;
    case 0xC4CC6436:                              // Key DISPLAY After pressing VCR
    case 0x6F46633F:                              // Key DISPLAY After pressing TV
    case 'i':
      Serial.println(F("+ Information"));
      break;
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    case 0x85CF699F:                              // Key TV/VCR
    case 0xDA529B37:                              // Key POWER After pressing VCR
    case 0x1A2EEC3B:                              // Key POWER After pressing TV
    case 'X':
      Serial.println(F("+ Power or Key TV/VCR"));
      programState = PROGRAM_WAIT;
      break;
    // -----------------------------------
    default:
      // Serial.print(F("+ Result value: "));
      // Serial.print(resultsValue);
      // Serial.print(F(", HEX: "));
      // Serial.println(resultsValue, HEX);
      printPrompt = false;
      break;
      // ----------------------------------------------------------------------
  } // end switch
  if (printPrompt && (programState == CLOCK_RUN)) {
    Serial.print(clockPrompt);
  }
}

// -----------------------------------------------------------------------------
// Handle continuous playing, and play errors such as: SD card not inserted.
//
void clockContinuous() {
  //
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    clockSwitch(results.value);
    irrecv.resume();
  }
  processClockNow();
}

// -----------------------------------------------------------------------------
// Calls from other programs.

// -----------------------------------------------------------------------------
// MP3 Player controls.

void rtClockRun() {
  Serial.println(F("+ rtClockRun();"));
  Serial.print(clockPrompt);
  while (programState == CLOCK_RUN) {
    //
    // Process serial input key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      clockSwitch(readByte);
    }
    //
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
