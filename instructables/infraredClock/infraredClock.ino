// -----------------------------------------------------------------------
/*
  This sample program includes:
  + Initializing the real time clock.
  + Making demonstrative calls to rtClock.cpp.
*/
// -------------------------------------------------------------------------------
#include "rtClock.h"

// This is for getting this test application to compile.
#include "Altair101.h"

void lightsStatusAddressData(byte status8bits, unsigned int address16bits, byte data8bits) {}
void playerSoundEffect(byte theFileNumber) {}
int KNIGHT_RIDER_SCANNER = 1;
void playerPlaySoundWait(int theFileNumber) {}
int TIMER_COMPLETE        = 2;
int TIMER_MINUTE        = 3;
uint8_t playerStatus = OUT_ON | HLTA_ON;        // Indicates MP3 player and it's paused (halted).
uint16_t playerCounter;
void mp3playerPlay(int theCounter) {}
void playerContinuous() {}

void clockLights(byte theMinute, byte theHour) {}
void initVirtualFrontPanel() {}
boolean VIRTUAL_FRONT_PANEL = false;

// Use OR to turn ON. Example:
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte INT_ON =     B00000001;  // INT    Interrupt

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
