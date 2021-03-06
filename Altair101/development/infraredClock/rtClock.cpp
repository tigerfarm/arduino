// -------------------------------------------------------------------------------
/*
  This is a clock control program.
  Functionality:
    ...

  DS3231 - A real time clock hardware module
              that is controlled by this program.

  Clock
  Front Panel Lights and Toggles

    Start by showing the time of day hours and minutes.
    To do: If clock timer mode was set, return to timer mode or reset timer mode values.
    Function: runClock().

  -----------
  Clock mode,
  + Status        Display the minutes tens digit,    day tens,   or year tens
  + Address       Display the hour: A1 ... A12,      month,      or century
  + Data          Display the minutes single digit,  day single, or year single
  + Indicator     WAIT : Off.
  + Indicator     HLDA : On to indicate controlled by another process, other than the program emulator.
  -----------
  + STOP          Not implemented.
  + RUN           Not implemented.
  + SINGLE up     1) First flip, show month and day.
                2) Next flip, show: year.
                3) Next flip, return to show time of day: hour and minutes.
  + SINGLE down   Not implemented.
  + EXAMINE       Set mode: flip EXAMINE to decrement the minutes.
  + EXAMINE NEXT  Set mode: flip EXAMINE NEXT to increment the minutes.
  + DEPOSIT       Toggle from clock set mode to clock mode. Set the time based on the display.
  + DEPOSIT NEXT  Not implemented.
  + RESET         Toggle from clock mode to clock set mode, to change the hours and minutes.
                   In clock set mode, the time flashes.
                Toggle from clock set mode to clock mode. Don't change the time.
  + PROTECT       Decrease MP3 player volume. To do: Decrement value to set.
  + UNPROTECT     Increase MP3 player volume. To do: Increment value to set.
  + AUX1 Up       Toggle clock mode off, return to processor mode.
  + AUX1 down     MP3 player mode
  + AUX2 up       Toggle clock counter mode.
  + AUX2 Down     Toggle clock timer mode on.

  Steps to set the time hours and minutes using the front panel.

  + In clock mode, flip RESET. Time flashes once a second.
  + Flip EXAMINE to decrement the minutes.
  + Flip EXAMINE NEXT to increment the minutes.
  + Flip RESET to return to clock mode, time not changed.
  + Flip DEPOSIT to return to clock mode, time is set based on the displayed hours and minutes.
  ++ Seconds are set to zero, so flip DEPOSIT when the second hand hits 12 and the minute changes.

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

String clockPrompt = "CLOCK ?- ";
String clockSetPrompt = "Clock SET ?- ";
String thePrompt = clockPrompt;           // Default.
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

// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
//
int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

int theHour;                // Variable for use anytime.

// -----------------------------------------------------------------------------
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dayOfTheWeek[7][2] = {"S", "M", "T", "W", "T", "F", "S"};

void printClockInt(int theInt) {
  if (theInt < 10) {
    Serial.print(F("0"));
  }
  Serial.print(theInt);
}

void printClockDate() {
  now = rtc.now();
  theCounterYear = now.year();
  theCounterMonth = now.month();
  theCounterDay = now.day();
  //
  Serial.print(theCounterYear);
  Serial.print(F("/"));
  printClockInt(theCounterMonth);
  Serial.print(F("/"));
  printClockInt(theCounterDay);
  Serial.print(F(" (YYYY/MM/DD) "));
}

void printClockTime() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  printClockInt(theCounterHours);
  Serial.print(F(":"));
  printClockInt(theCounterMinutes);
  Serial.print(F(":"));
  printClockInt(theCounterSeconds);
  Serial.print(F(" (HH:MM:SS)"));
  //
  Serial.print(F(" "));
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(F(" "));
  // Show AM/PM hour.
  String AmPm = "";
  if (theCounterHours > 12) {
    theHour = theCounterHours - 12;
    AmPm = "pm";
  } else if (theCounterHours == 0) {
    theHour = 12; // 12 midnight, 12am
    AmPm = "pm";
  } else {
    theHour = theCounterHours;
    AmPm = "am";
  }
  Serial.print(theHour);
  Serial.print(AmPm);
}

// -----------------------------------------------------------------------------
void printClockDateTime() {
  // Date: 2018/12/08 (YYYY/MM/DD)  Time: 15:59:56 (HH:MM:SS) Saturday 3pm
  Serial.print(F("Date: "));
  printClockDate();
  Serial.print(F(" Time: "));
  printClockTime();
}
void syncCountWithClock() {
  Serial.print(F("+ syncCountWithClock, "));
  printClockDateTime();
  Serial.println();
}

// -----------------------------------------------------------------------------
// Clock pulse actions.

void clockPulseYear() {
  Serial.print(F("+++++ clockPulseYear(), theCounterYear= "));
  Serial.println(theCounterYear);
}
void clockPulseMonth() {
  Serial.print(F("++++ clockPulseMonth(), theCounterMonth= "));
  Serial.println(theCounterMonth);
}
void clockPulseDay() {
  Serial.print(F("+++ clockPulseDay(), theCounterDay= "));
  Serial.println(theCounterDay);
}
void clockPulseHour() {
  Serial.print(F("++ clockPulseHour(), theCounterHours= "));
  Serial.print(theCounterHours);
  // Use AM/PM rather than 24 hours.
  String AmPm = "";
  if (theCounterHours > 12) {
    theHour = theCounterHours - 12;
    AmPm = "pm";
  } else if (theCounterHours == 0) {
    theHour = 12; // 12 midnight, 12am
    AmPm = "pm";
  } else {
    theHour = theCounterHours;
    AmPm = "am";
  }
  Serial.print(F(", "));
  Serial.print(theHour);
  Serial.print(AmPm);
  Serial.println();
}
void clockPulseMinute() {
  // Serial.print(F("+ clockPulseMinute(), theCounterMinutes= "));
  // printClockInt(theCounterMinutes);
  // Serial.println();
}
void clockPulseSecond() {
  Serial.print(F("+ theCounterSeconds = "));
  printClockInt(theCounterSeconds);
  Serial.println();
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

// -----------------------------------------------------------------------------
// Initialize the component module.
//
void setupClock() {
  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized: infrared receiver."));

  // Initialize the Real Time Clock (RTC).
  if (!rtc.begin()) {
    Serial.println(F("--- Error: RTC not found."));
    while (1);
  }
  //
  // Set the time for testing. Example, test for testing AM/PM.
  // Serial.println(F("++ Set clock to Dec.8,2018 03:59:56pm."));
  rtc.adjust(DateTime(2018, 12, 8, 15, 59, 56)); // DateTime(year, month, day, hour, minute, second)
  delay(100);
  //
  Serial.print(F("+ Initialized: clock, "));
  printClockDateTime();
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
      // CR.
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 13:
      // LF.
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
      // Serial.println(F("+ Key PLAY"));
      Serial.print(F("+ 'r', Current "));
      printClockDateTime();
      Serial.println();
      break;
    // -----------------------------------
    case 0xFF38C7:
    case 0x82D6EC17:
      Serial.print(F("+ Key OK|Enter - Toggle: pause|start the song, playerCounter="));
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
      Serial.print(F("+ Key up - next directory, directory number: "));
      Serial.println();
      break;
    // -----------------------------------
    case 0xCDFC965B:                        // After pressing VCR
    case 0xDD8E75F:                         // After pressing TV
    case 0xFF4AB5:                          // Small remote key down.
    case 'd':
      Serial.print(F("+ Key down - previous directory, directory number: "));
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Loop a single song
    //
    case 0xA02E4EBF:
    case 0xFF6897:
    case 'L':
      Serial.println(F("+ Key *|A.Select - Loop on: loop this single MP3."));
      break;
    case 0xC473DE3A:
    case 0xFFB04F:
    case 'l':
      Serial.println(F("+ Key #|Eject - Loop off: Single MP3 loop is off."));
      break;
    // -----------------------------------
    // Toshiba VCR remote, not programmed.
    case 0x718E3D1B:                        // Toshiba VCR remote
    case 0xB16A8E1F:                        // After pressing TV
      Serial.print(F("+ Key 1: "));
      Serial.println(F(""));
      break;
    case 0xF8FB71FB:                        // Toshiba VCR remote
    case 0x38D7C2FF:                        // After pressing TV
      Serial.print(F("+ Key 2: "));
      Serial.println(F(""));
      break;
    case 0xE9E0AC7F:                        // Toshiba VCR remote
    case 0x29BCFD83:                        // After pressing TV
      Serial.print(F("+ Key 3: "));
      Serial.println(F(""));
      break;
    //
    // ----------------------------------------------------------------------
    // Equalizer setting selection.
    //
    case 0x38BF129B:                        // Toshiba VCR remote
    case 0x789B639F:                        // After pressing TV
    case 0xFF22DD:                          // Small remote
    case '4':
      Serial.print(F("+ Key 4: "));
      Serial.println();
      break;
    case 0x926C6A9F:                        // Toshiba VCR remote
    case 0xD248BBA3:                        // After pressing TV
    case 0xFF02FD:
    case '5':
      Serial.print(F("+ Key 5: "));
      Serial.println();
      break;
    case 0xE66C5C37:                        // Toshiba VCR remote
    case 0x2648AD3B:                        // After pressing TV
    case 0xFFC23D:
    case '6':
      Serial.print(F("+ Key 6: "));
      Serial.println();
      break;
    case 0xD75196BB:                        // Toshiba VCR remote
    case 0x172DE7BF:                        // After pressing TV
    case 0xFFE01F:
    case '7':
      Serial.print(F("+ Key 7: "));
      Serial.println();
      break;
    case 0x72FD3AFB:                        // Toshiba VCR remote
    case 0xB2D98BFF:                        // After pressing TV
    case 0xFFA857:
    case '8':
      Serial.print(F("+ Key 8: "));
      Serial.println();
      break;
    case 0xCCAA92FF:                        // Toshiba VCR remote
    case 0xC86E403:                         // After pressing TV
    case 0xFF906F:
    case '9':
      Serial.print(F("+ Key 9: "));
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Volume
    //
    case 0x2B8BE5F:
    case 0xFFE21D:              // Small remote, Key 3
    case 'V':
      Serial.print(F("+ Key Volume ^"));
      Serial.println();
      break;
    case 0x1CF3ACDB:
    case 0xFFA25D:              // Small remote, Key 1
    case 'v':
      Serial.print(F("+ Key Volume v"));
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Virtual Front Panel Switch Controls"));
      Serial.println(F("+++ Real Time Clock Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ _, STOP         Not implemented."));
      Serial.println(F("+ r, RUN mode     CLOCK mode: show date and time."));
      Serial.println(F("+ s, SINGLE STEP  Clock SET mode: flip to decrement date or time value."));
      Serial.println(F("+ S, SINGLE Down  Clock SET mode: flip to increment date or time value."));
      Serial.println(F("+ x, EXAMINE      Enter clock SET mode. Show the current clock SET mode date and time."));
      Serial.println(F("+ X, EXAMINE NEXT Rotate through the clock values that can be set."));
      Serial.println(F("+                 1) First flip, go into clock SET mode."));
      Serial.println(F("++                   Clock SET mode to change the year value."));
      Serial.println(F("++                2) Clock SET mode to change the month value."));
      Serial.println(F("++                3) Clock SET mode to change the day value."));
      Serial.println(F("++                4) Clock SET mode to change the hours value."));
      Serial.println(F("++                5) Clock SET mode to change the minutes. value"));
      Serial.println(F("+ p, DEPOSIT      Set the time based on the set clock values."));
      Serial.println(F("++                Toggle from clock SET mode to CLOCK mode."));
      Serial.println(F("+ P, DEPOSIT NEXT Not implemented."));
      Serial.println(F("+ R, RESET        Toggle from clock SET mode to CLOCK mode. Don't change the time."));
      Serial.println(F("+ _, CLR          Not implemented."));
      Serial.println(F("-------------"));
      Serial.println(F("+ 0...9, a...f    Toggle sense/address/data switches:  A0...A9, A10...A15."));
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Real Time Clock Controls"));
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
    Serial.print(thePrompt);
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
  Serial.print(thePrompt);
  while (programState == CLOCK_RUN) {
    // Process serial input key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      clockSwitch(readByte);
    }
    // Process infrared input key presses from a remote.
    if (irrecv.decode(&results)) {
      clockSwitch(results.value);
      irrecv.resume();
    }
    processClockNow();    // Print on going time clicks.
    //
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
