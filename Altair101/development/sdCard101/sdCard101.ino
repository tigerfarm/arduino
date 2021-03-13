// -----------------------------------------------------------------------
/*
  This sample program includes:
  + Initializing the SD card.
  + Making demonstrative calls to sdCard.cpp.
*/
// -------------------------------------------------------------------------------
#include "sdCard.h"

// This is for running the test program.
// The goal is to include the MP3 player program with Altair101a.
#include "Altair101b.h"
#include "cpuIntel8080.h"

byte Mem[MEMSIZE];

void ledFlashError() {}
void ledFlashSuccess() {}

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
      sdCardSwitch('r');
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ WAIT mode controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+++ SD Card controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ r, RUN          Show..."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
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
      Serial.println(F("++ set programState = SDCARD_RUN"));
      programState = SDCARD_RUN;
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
    delay(60);
  }
}

// -----------------------------------------------------------------------------
/*
// Needs to match the upload program:
//  9600, 19200, 38400, 57600, 115200
unsigned long downloadBaudRate = 115200;

void modeDownloadProgram() {
  Serial.println(F("+ Download mode: ready to receive a program bytes on Serial2. Enter, x, to exit."));
  Serial.print(F("++ Serial 2 baud rate: "));
  Serial.println(downloadBaudRate);
  byte readByte = 0;
  int readByteCount = 0;      // Count the downloaded bytes that are entered into processor memory.
  unsigned long timer;        // Indicator used to identify when download has ended.
  boolean downloadStarted = false;
  Serial2.begin(downloadBaudRate);
  while (programState == SERIAL_DOWNLOAD) {
    if (Serial2.available() > 0) {
      readByte = Serial2.read();            // Read the incoming byte.
      if (!downloadStarted) {
        downloadStarted = true;
        Serial.println(F("+       Address"));
      }
      // Process the incoming byte into the memory buffer.
      Serial.print(F("++ Byte# "));
      Serial.print(readByteCount);
      MWRITE(readByteCount, readByte)
      readByteCount++;
      Serial.println();
      timer = millis();
    }
    if (downloadStarted && ((millis() - timer) > 500)) {
      // Exit download state, if the bytes were downloaded and then stopped for 1/2 second.
      //  This indicates that the download is complete.
      Serial.println(F("+ Download complete."));
      programState = PROGRAM_WAIT;
    }
    // -----------------------------------------------
    // Flip or send serial character RESET(R) or STOP(s), to exit download mode.
    // Check serial input for RESET or STOP.
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      if (readByte == 'R' || readByte == 's' || readByte == 'x') {
        Serial.println(F("+ Exit download mode."));
        programState = PROGRAM_WAIT;
        delay(100); // Required to wait for the LF when in IDE serial monitor.
        while (Serial.available() > 0) {
          // Serial.println(F("+ Exit, Clear other characters in the buffer, example: LF."));
          readByte = Serial.read();
          delay(100); // Required to wait for the LF when in IDE serial monitor.
        }
      }
    }
    // -----------------------------------------------
  }
  Serial2.end();                  // Close and allow this port for output use.
  if (readByteCount > 0) {
    Serial.println(F("+ Downloaded bytes stored into memory."));
  } else {
    Serial.println(F("+ Downloaded cancelled."));
  }
  Serial.println(F("+ Exit serial download mode."));
}
*/
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

  // ----------------------------------------------------
  setupSdCard();

  // ----------------------------------------------------
  programState = SDCARD_RUN;
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
    case SDCARD_RUN:
      host_clr_status_led_WAIT()
      // digitalWrite(HLDA_PIN, HIGH);
      sdCardRun();
      // digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
    // ----------------------------
    case SERIAL_DOWNLOAD:
      host_clr_status_led_WAIT();
      // host_set_status_led_HLDA();
      // modeDownloadProgram();
      // host_clr_status_led_HLDA();
      host_set_status_led_WAIT();
      // printFrontPanel();
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
