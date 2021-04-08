// -------------------------------------------------------------------------------
/*
  This is a clock control program.
  Functionality:
  + Use serial keyboard inputs
  + View current time
  + Set clock time
  + Use infrared inputs for MP3 player, if one is connected.

  DS3231 - A real time clock hardware module
              that is controlled by this program.

  -----------------------------------------------------------------------------
  Altair 101 desktop CLOCK mode
  + Front Panel Lights and Toggles
  + Start by showing the time of day hours and minutes.

  Altair 101 desktop CLOCK mode LED light indiators,
  + Status        Display the minutes tens digit,    day tens,   or year tens
  + Address       Display the hour: A1 ... A12,      month,      or century
  + Data          Display the minutes single digit,  day single, or year single
  + Indicator     WAIT : Off.
  + Indicator     HLDA : On to indicate controlled by another process, other than the program emulator.

  Steps to set the time hours and minutes using the front panel.
  + In clock mode, flip RESET. Time flashes once a second.
  + Flip EXAMINE to decrement the minutes.
  + Flip EXAMINE NEXT to increment the minutes.
  + Flip RESET to return to clock mode, time not changed.
  + Flip DEPOSIT to return to clock mode, time is set based on the displayed hours and minutes.
  ++ Seconds are set to zero, so flip DEPOSIT when the second hand hits 12 and the minute changes.

  --------------------------------------------------------------------------------
  Program sections
  ----------------------------------------
  + Print clock data
  void printClockDateTime() {
  void printClockDate();
  void printClockDateTime()
  ----------------------------------------
  + Clock pulse processing
  clockPulse<Year|Month|Day|Hour|Seconds>()
  void processClockNow()
  ----------------------------------------
  void setupClock()
  void printSetClockDateTime()
  int getClockValue(int getValue)
  void getClockValueMinMax(int getValue)
  void clockSetSwitch(int resultsValue)
  ----------------------------------------
  void clockSwitch(int resultsValue)
  void rtClockContinuous()
  void rtClockSet()
  void rtClockRun()

  -----------------------------------------------------------------------------
  Wiring

  DS3231 Clock pins   Arduino pins
  + VCC               +5v on Uno or Mega, or +3.3v on Due or NodeMCU.
  + GND               DND ground.
  + SDA               SDA on Due or Mega, Nano D4 (pin 4), same on Uno.
  + SCL               SCL on Due or Mega, Nano D5 (pin 5), same on Uno.

  ---------------------
  Clock module pins

  -----------------------
  |     Curcuit           |
  |     side              |
  -----------------------
   |   |   |   |   |   |
  GND VCC SDA SCL SQW 32K

  -----------------------
  |     Battery           |
  |     side              |
  -----------------------
   |   |   |   |   |   |
  32K SQW SCL SDA VCC GND

  ------------------------------------------------------------------------------
  DS3231 Clock Library:
    Filter your search by typing ‘rtclib’.
    There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib

*/
// -------------------------------------------------------------------------------

#include "Altair101b.h"

#ifdef Altair101b
#include "Mp3Player.h"
#endif

String clockPrompt = "CLOCK ?- ";
String thePrompt = "CLOCK ?- ";           // Default.
String clockSetPrompt = "Clock SET ?- ";
extern int programState;

// Clock internal status, internal to this program.
#define RTCLOCK_RUN 1
#define RTCLOCK_SET 2
int rtClockState = RTCLOCK_RUN;

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

int theHour;                    // Variable for use anytime.

// -----------------------------------------------------------------------------
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dayOfTheWeek[7][2] = {"S", "M", "T", "W", "T", "F", "S"};

void printClockInt(int theInt) {
  if (theInt < 10) {
    Serial.print(F("0"));
  }
  Serial.print(theInt);
}

// -----------------------------------------------------------------------------
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

void printClockDateTime() {
  // Date: 2018/12/08 (YYYY/MM/DD)  Time: 15:59:56 (HH:MM:SS) Saturday 3pm
  Serial.print(F("Date: "));
  printClockDate();
  Serial.print(F(" Time: "));
  printClockTime();
  clockLights(theCounterMinutes, theCounterHours);  // Display in the front panel.
}

// -----------------------------------------------------------------------------
// Clock pulse actions.

void clockPulseYear() {
  if (programState == CLOCK_RUN) {
    Serial.print(F("+++++ clockPulseYear(), theCounterYear= "));
    Serial.println(theCounterYear);
  }
}
void clockPulseMonth() {
  if (programState == CLOCK_RUN) {
    Serial.print(F("++++ clockPulseMonth(), theCounterMonth= "));
    Serial.println(theCounterMonth);
  }
}
void clockPulseDay() {
  if (programState == CLOCK_RUN) {
    Serial.print(F("+++ clockPulseDay(), theCounterDay= "));
    Serial.println(theCounterDay);
  }
}
void clockPulseHour() {
  clockLights(theCounterMinutes, theCounterHours);  // Display in the front panel.
  if (programState == CLOCK_RUN && !VIRTUAL_FRONT_PANEL) {
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
}
void clockPulseMinute() {
  clockLights(theCounterMinutes, theCounterHours);  // Display in the front panel.
  if (programState == CLOCK_RUN && !VIRTUAL_FRONT_PANEL) {
    Serial.print(F("+ clockPulseMinute(), theCounterMinutes= "));
    printClockInt(theCounterMinutes);
    Serial.println();
  }
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
    // -------------------------
    // Time pulses.
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

int numClockValues = 6;               // Number of clock values: year ... seconds.
int setClockValue = numClockValues;   // Set which clock value is set for changing.
int setValues[6];                     // For storing each type of clock values that are to be changed.
char clockValueName[6][8] = {"year", "month", "day", "hour", "minute", "seconds" };

void setupClock() {
  // Initialize the Real Time Clock (RTC).
  if (!rtc.begin()) {
    Serial.println(F("--- Error: RTC not found."));
    while (1);
  }
  //
  // Set the time for testing. Example, test for testing AM/PM.
  // Serial.println(F("++ Set clock to Dec.8,2018 03:59:56pm."));
  now = rtc.now();
  // If no battery, set the date and time. If there is a battery, don't reset it.
  if (now.year() < 2021) {
    rtc.adjust(DateTime(2021, 4, 8, 15, 36, 58)); // DateTime(year, month, day, hour, minute, second)
  }
  delay(100);
  //
  Serial.print(F("+ Initialized: clock, "));
  printClockDateTime();
  Serial.println();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Menu items to set the clock date and time values.

int theSetMin = 0;
int theSetMax = 59;

void printSetClockDateTime() {
  // Date: 2018/12/08 (YYYY/MM/DD)  Time: 15:59:56 (HH:MM:SS) Saturday 3pm
  Serial.print(F("Date: "));
  int aCounter = 0;
  // -------
  if (setValues[aCounter] < 0) {
    Serial.print(F("----"));
  } else {
    Serial.print(setValues[aCounter]);
  }
  // -------
  aCounter++;
  Serial.print(F("/"));
  if (setValues[aCounter] < 0) {
    Serial.print(F("--"));
  } else {
    printClockInt(setValues[aCounter]);
  }
  // -------
  aCounter++;
  Serial.print(F("/"));
  if (setValues[aCounter] < 0) {
    Serial.print(F("--"));
  } else {
    printClockInt(setValues[aCounter]);
  }
  //Serial.print(F(" (YYYY/MM/DD) "));
  Serial.print(F(  "              "));  // Spacing
  //
  // -------
  aCounter++;
  Serial.print(F(" Time: "));
  if (setValues[aCounter] < 0) {
    Serial.print(F("--"));
  } else {
    printClockInt(setValues[aCounter]);
  }
  // -------
  aCounter++;
  Serial.print(F(":"));
  if (setValues[aCounter] < 0) {
    Serial.print(F("--"));
  } else {
    printClockInt(setValues[aCounter]);
  }
  // -------
  aCounter++;
  Serial.print(F(":"));
  if (setValues[aCounter] < 0) {
    Serial.print(F("--"));
  } else {
    printClockInt(setValues[aCounter]);
  }
}

int getClockValue(int getValue) {
  switch (getValue) {
    case 0:
      return (theCounterYear);
      break;
    case 1:
      return (theCounterMonth);
      break;
    case 2:
      return (theCounterDay);
      break;
    case 3:
      return (theCounterHours);
      break;
    case 4:
      return (theCounterMinutes);
      break;
    case 5:
      return (theCounterSeconds);
      break;
    default:
      return (-1);  // Error
  }
}

void getClockValueMinMax(int getValue) {
  switch (getValue) {
    case 0:
      theSetMax = 2525; // In the year 2525, If man is still alive, If woman can survive...
      theSetMin = 1795; // Year John Keats the poet was born.
      break;
    case 1:
      theSetMax = 12;
      theSetMin = 1;
      break;
    case 2:
      theSetMax = 31;
      theSetMin = 1;
      break;
    case 3:
      theSetMax = 24;
      theSetMin = 0;
      break;
    case 4:
      theSetMax = 59;
      theSetMin = 0;
      break;
    case 5:
      theSetMax = 59;
      theSetMin = 0;
      break;
    default:
      // Error
      theSetMax = 0;
      theSetMin = 0;
  }
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void clockSetSwitch(int resultsValue) {
  boolean printPrompt = true;
  int theValue;
  int setValue;   // temporary
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
      // LF, IDE character.
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 13:
      // CR, Mac terminal window character. ignore
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    case 'S':                   // Single STEP up
      Serial.print(F("+ Clock SET, "));
      Serial.print(clockValueName[setClockValue]);
      if (setValues[setClockValue] < theSetMax) {
        setValues[setClockValue] = setValues[setClockValue] + 1;
        Serial.print(F(", increase to: "));
      } else {
        Serial.print(F(", already at maximum: "));
      }
      Serial.print(setValues[setClockValue]);
      Serial.println();
      break;
    case 's':                   // Single STEP down
      Serial.print(F("+ Clock SET, "));
      Serial.print(clockValueName[setClockValue]);
      if (setValues[setClockValue] > theSetMin) {
        setValues[setClockValue] = setValues[setClockValue] - 1;
        Serial.print(F(", decrease to: "));
      } else {
        Serial.print(F(", already at minimum: "));
      }
      Serial.print(setValues[setClockValue]);
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    case 'r':
      Serial.println();
      Serial.print(F("+ Current "));
      printClockDateTime();
      Serial.println();
      Serial.print(F("+ Set     "));
      printSetClockDateTime();
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // char clockValueName[6][8] = {"year", "month", "day", "hours", "minutes", "seconds" };
    // int numClockValues = 6;
    case 'x' :
      if (setClockValue > 0) {
        setClockValue--;
      } else {
        setClockValue = numClockValues - 1; // Loop around.
      }
      Serial.print(F("+ Clock SET, Previous value: "));
      Serial.print(clockValueName[setClockValue]);
      if (setValues[setClockValue] < 0) {
        // Not set, set to current clock value.
        setValues[setClockValue] = getClockValue(setClockValue);
      }
      Serial.print(F(", current set value = "));
      Serial.print(setValues[setClockValue]);
      Serial.println();
      getClockValueMinMax(setClockValue);       // Used when increasing or decreasing the value.
      break;
    // -----------------------------------
    case 'X':
      if (setClockValue < (numClockValues - 1)) {
        setClockValue++;
      } else {
        setClockValue = 0;      // Loop around.
      }
      Serial.print(F("+ Clock SET, Next value: "));
      Serial.print(clockValueName[setClockValue]);
      if (setValues[setClockValue] < 0) {
        // Not set, set to current clock value.
        setValues[setClockValue] = getClockValue(setClockValue);
      }
      Serial.print(F(", current set value = "));
      Serial.print(setValues[setClockValue]);
      Serial.println();
      getClockValueMinMax(setClockValue);       // Used when increasing or decreasing the value.
      break;
    // ----------------------------------------------------------------------
    case 'p':                   // DEPOSIT
      Serial.print(F("+ DEPOSIT, Key OK|Enter, values="));
      Serial.println();
      int theSetValue[6];
      for (int i = 0; i < numClockValues - 1; i++ ) {
        theSetValue[i] = setValues[i];
        if (setValues[i] < 0) {
          theSetValue[i] = getClockValue(i);
        }
      }
      //                    year            month           day             hours           minutes         seconds(not changed)
      rtc.adjust(DateTime(theSetValue[0], theSetValue[1], theSetValue[2], theSetValue[3], theSetValue[4], setValues[5]));
      Serial.println();
      Serial.print(F("+ Clock set to "));
      printClockDateTime();
      Serial.println();
      Serial.println("");
      if (VIRTUAL_FRONT_PANEL) {
        initVirtualFrontPanel();    // Update VFP.
        clockLights(theCounterMinutes, theCounterHours);
      }
      break;
    // ----------------------------------------------------------------------
    case 'R':
      Serial.println(F("+ RESET, return to clock run mode."));
      rtClockState = RTCLOCK_RUN;
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ SET Clock Time"));
      Serial.println(F("-------------"));
      Serial.println(F("+ x, EXAMINE      Rotate through the clock values that can be set."));
      Serial.println(F("                  Rotate order: seconds to year."));
      Serial.println(F("+ X, EXAMINE NEXT Rotate through the clock values that can be set."));
      Serial.println(F("                  Rotate order: year to seconds."));
      Serial.println(F("+ S, Set UP       Decrement date or time value."));
      Serial.println(F("+ s, Set Down     Increment date or time value."));
      Serial.println(F("-------------"));
      Serial.println(F("+ r, RUN          CLOCK mode: show date and time."));
      Serial.println(F("                  Clock SET mode: show date and time, and the set data and time value."));
      Serial.println(F("+ p, DEPOSIT      Set the time based on the clock SET values."));
      Serial.println(F("+ R, RESET        Toggle between clock SET mode to CLOCK mode."));
      Serial.println(F("                  Reset the clock SET values when taggling into clock SET mode."));
      Serial.println(F("                  Doesn't set/change the time."));
      Serial.println(F("-------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // -----------------------------------------------------------------------
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    // ----------------------------------------------------------------------
    default:
      // Serial.print(F("+ Result value: "));
      // Serial.print(resultsValue);
      // Serial.print(F(", HEX: "));
      // Serial.println(resultsValue, HEX);
      printPrompt = false;
      break;
  }
  if (printPrompt && (rtClockState == RTCLOCK_SET)) {
    Serial.print(thePrompt);
  }
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// Clock Controls

void clockSwitch(int resultsValue) {
  if (VIRTUAL_FRONT_PANEL) {
    if (programState == CLOCK_RUN) {
    Serial.print(F("\033[9;1H"));  // Move cursor to below the prompt: line 9, column 1.
    Serial.print(F("\033[J"));     // From cursor down, clear the screen.
    }
  }
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
      // LF, IDE character.
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 13:
      // CR, Mac terminal window character. ignore
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    case 'r':
      Serial.println();
      Serial.print(F("+ Current "));
      printClockDateTime();
      Serial.println();
      break;
    // -------------
    case 'R':
      Serial.println(F("+ RESET, enter clock set mode."));
      rtClockState = RTCLOCK_SET;
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Real Time Clock Controls"));
      Serial.println(F("--------------------------"));
      Serial.println(F("+ r, RUN time     Show date and time."));
      Serial.println(F("+ R, RESET        Toggle between clock SET mode to CLOCK view mode."));
      Serial.println(F("--------------------------"));
      Serial.println(F("+ t/T VT100 panel Disable/enable VT100 virtual front panel."));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 'i':
      Serial.println(F("+ Information"));
      break;
    // -------------
    case 't':
      Serial.print(F("+ VT100 escapes are disabled and block cursor on."));
      if (VIRTUAL_FRONT_PANEL) {
        VIRTUAL_FRONT_PANEL = false;
        Serial.print(F("\033[0m\033[?25h"));       // Insure block cursor display: on.
      }
      // Note, VT100 escape sequences don't work on the Ardino IDE monitor.
      break;
    case 'T':
      // The following requires a VT100 terminal such as a Macbook terminal.
      initVirtualFrontPanel();
      clockLights(theCounterMinutes, theCounterHours);
      Serial.print(F("+ VT100 escapes are enabled and block cursor off."));
      break;
    // ----------------------------------------------------------------------
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    // -------------
    case 'X':
      if (VIRTUAL_FRONT_PANEL) {
        Serial.print(F("\033[9;1H"));  // Move cursor to below the prompt: line 9, column 1.
        Serial.print(F("\033[J"));     // From cursor down, clear the screen.
      }
      Serial.println(F("+ Exit CLOCK mode. Return to Processor WAIT mode."));
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
  if (printPrompt && (programState == CLOCK_RUN) && (rtClockState == RTCLOCK_RUN)) {
    Serial.print(thePrompt);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Handle continuous playing, and play errors such as: SD card not inserted.
//
void rtClockContinuous() {
  //
  // Process infrared key presses.
  /*
    if (clockIrrecv.decode(&clockIrResults)) {
    clockSwitch(clockIrResults.value);
    clockIrrecv.resume();
    }
  */
  processClockNow();
}

// -----------------------------------------------------------------------------
// MP3 Player controls.

void rtClockSet() {
  thePrompt = clockSetPrompt;
  Serial.print(thePrompt);
  // Initialize the clock set values.
  for (int i = 0; i < numClockValues - 2; i++ ) {
    setValues[i] = -1;                  // -1 means that the value is not set to change.
  }
  setValues[numClockValues - 2] = getClockValue(numClockValues - 2) + 1;       // Set minutes to current minutes + 1, for quick setting.
  setValues[numClockValues - 1];        // Set seconds to zero.
  //
  while (rtClockState == RTCLOCK_SET) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      clockSetSwitch(readByte);
    }
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
  thePrompt = clockPrompt;
  Serial.print(thePrompt);
}

void rtClockRun() {
  if (VIRTUAL_FRONT_PANEL) {
    clockSwitch('T');         // Update VFP.
  }
  setupClock();
  Serial.println();
  Serial.print(thePrompt);
  while (programState == CLOCK_RUN) {
    // Process serial input key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      clockSwitch(readByte);
    }
    processClockNow();    // Process on going time clicks.
    if (rtClockState == RTCLOCK_SET) {
      rtClockSet();
    }
    //
#ifdef Altair101b
    playerContinuous();   // Allow for infrared music control while in clock mode.
#endif
    //
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
