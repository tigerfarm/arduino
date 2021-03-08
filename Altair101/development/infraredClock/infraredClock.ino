// -----------------------------------------------------------------------
/*
  This is a sample program to initiate and call the real time clock program: rtClock.cpp.
*/
// -------------------------------------------------------------------------------
#include "rtClock.h"

// This is for running the test program.
// The goal is to include the MP3 player program with Altair101a.
#include "Altair101b.h"

// -----------------------------------------------------------------------------
int programState;

String waitPrompt = "101 ?- ";

// -----------------------------------------------------------------------------
word status_wait = 1;     // Default on
// On a Due, the WAIT LED is yellow.
#define WAIT_PIN 13
#define host_read_status_led_WAIT()   status_wait
#define host_clr_status_led_WAIT()  { digitalWrite(13, LOW);  status_wait = false; }
#define host_set_status_led_WAIT()  { digitalWrite(13, HIGH); status_wait = true; }

byte hwStatus = B11111111;            // Initial state.

void ledFlashError() {}
void ledFlashSuccess() {}

void processWaitSwitch(int readByte) {
  /*
  Serial.print(F("++ processWaitSwitch("));
  Serial.print(readByte);
  Serial.print(F(")"));
  Serial.println();
  */
  boolean printPrompt = true;
  switch (readByte) {
    // -------------------------------
    case 10:
      // LF, IDE character.
      return;     // To avoid printing the prompt.
      break;
    case 13:
      // CR, Mac terminal window character. ignore
      Serial.println();
      // printPrompt = false;
      break;
    // -------------
    // Mouse wheel
    case 27:
    case 91:
    case 65: // Mouse wheel down (away from user).
    case 66: // Mouse wheel up   (toward user).
      // Ignore.
      printPrompt = false;
      break;
    // ------------------------------
    // -------------
    case 'r':
      clockSwitch('r');
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ WAIT mode controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+++ Clock controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ r, RUN          Show clock's time."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("------------------"));
      Serial.println(F("+ 1 ...3          Test: Play a file and wait until completed."));
      Serial.println(F("+ 4 ...6          Test: Play a file and return to processing here."));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));          // Cursor home and clear the screen.
      break;
    case 0x85CF699F:                              // Key TV/VCR
    case 'X':
      Serial.println(F("++ set programState = CLOCK_RUN"));
      programState = CLOCK_RUN;
      break;
    default:
      printPrompt = false;
      break;
  } // end switch
  if (printPrompt) {
    Serial.print(waitPrompt);
  }
}

void runProcessorWait() {
  /*
+ Start the loop().
+ rtClockRun();
CLOCK ?- + Power or Key TV/VCR
+ Processor WAIT mode.
101 ?- 
   */
  Serial.println(F("+ Processor WAIT mode."));
  Serial.print(waitPrompt);
  while (programState == PROGRAM_WAIT) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      processWaitSwitch(readByte);
    }
    // rtClockContinuous();              // Check for player update or infrared key pressed.
    delay(60);
  }
}

// -----------------------------------------------------------------------------
void functionNamePrint(String theName) {
  Serial.print(F("+ "));
  Serial.print(theName); // prints the function name.
  Serial.print(F("()"));
}
void setup() {
  // ----------------------------------------------------
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);         // Baud rates: 9600 19200 57600 115200
  delay(2000);
  Serial.println(); // Newline after garbage characters.
  Serial.print(F("++"));
  functionNamePrint(__func__);
  Serial.println();

  // ----------------------------------------------------
  pinMode(WAIT_PIN, OUTPUT);        // Indicator: Altair 8800 emulator program WAIT state: LED on or LED off.
  host_set_status_led_WAIT();
  delay(200);
  host_clr_status_led_WAIT();
  delay(200);
  host_set_status_led_WAIT();

  setupClock();

  // ----------------------------------------------------
  programState = CLOCK_RUN;
  Serial.println("+ Start the loop().");
}

// -----------------------------------------------------------------------------
void loop() {
  switch (programState) {
    // ----------------------------
    case PROGRAM_WAIT:
      host_set_status_led_WAIT();
      runProcessorWait();
      break;
    // ----------------------------
    case CLOCK_RUN:
      host_clr_status_led_WAIT()
      // digitalWrite(HLDA_PIN, HIGH);
      rtClockRun();
      // digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
