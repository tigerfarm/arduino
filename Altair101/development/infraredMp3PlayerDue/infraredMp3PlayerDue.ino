// -----------------------------------------------------------------------
/*
  This is a sample program to initate andcall the MP3 player program: Mp3PlayerDue.cpp.
  The MP3 player program controls the DFPlayer MP3 player:
      play or pause a song, play next or previous song,
      play next or previous directory of songs, 
      volume up or down, and loop a single song.

  DFPlayer hardware module is a programable Mini MP3 Player.

  Steps to add the MP3 player program (Mp3PlayerDue.cpp) to an Arduino program (*.ino):
  + Include the header file: Mp3PlayerDue.h
  + Run the setup process in setup(): setupMp3Player();
  + In this programs loop() process, call mp3PlayerRun();
*/
// -------------------------------------------------------------------------------
#include "Mp3PlayerDue.h"

// This is for running the test program.
// The goal is to include the MP3 player program with Altair101a.
#include "Altair101a.h"

// -----------------------------------------------------------------------------
int programState;

// -----------------------------------------------------------------------------
word status_wait = 1;     // Default on
#define host_read_status_led_WAIT()   status_wait
#define host_clr_status_led_WAIT()  { digitalWrite(13, LOW);  status_wait = false; }
#define host_set_status_led_WAIT()  { digitalWrite(13, HIGH); status_wait = true; }

byte hwStatus = B11111111;            // Initial state.

void ledFlashError() {}
void ledFlashSuccess() {}

void processWaitSwitch(int readByte) {
  switch (readByte) {
    // -------------------------------
    case 10:
    case 13:
      // CR/LF.
      Serial.println();
      Serial.print(F("> "));
      break;
    // -------------
    // Mouse wheel
    case 27:
    case 91:
    case 65: // Mouse wheel down (away from user).
    case 66: // Mouse wheel up   (toward user).
      // Ignore.
      break;
    // ------------------------------
    case 'p':
      Serial.println(F("++ mp3PlayerPause();"));
      mp3PlayerPause();
      Serial.print(F("> "));
      break;
    // -------------
    case 'r':
      playerSwitch('r');
      Serial.print(F("> "));
      break;
    case 's':
      playerSwitch('s');
      Serial.print(F("> "));
      break;
    case 'v':
      playerSwitch('v');
      Serial.print(F("> "));
      break;
    case 'V':
      playerSwitch('V');
      Serial.print(F("> "));
      break;
    // ------------------------------
    case '1':
      mp3playerPlaywait(1);
      Serial.print(F("> "));
      break;
    case '2':
      mp3playerPlaywait(2);
      Serial.print(F("> "));
      break;
    case '3':
      mp3playerPlaywait(69);
      Serial.print(F("> "));
      break;
    case '4':
      mp3playerPlay(1);
      Serial.print(F("> "));
      break;
    case '5':
      mp3playerPlay(2);
      Serial.print(F("> "));
      break;
    case '6':
      mp3playerPlay(69);
      Serial.print(F("> "));
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ MP3 Player Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ p, Pause        Pause, pause playing."));
      Serial.println(F("+ s, STOP         Pause, stop playing."));
      Serial.println(F("+ r, RUN          Start, playing the current song."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("------------------"));
      Serial.println(F("+ 1 ...3          Play a file and wait until completed."));
      Serial.println(F("+ 4 ...6          Play a file and return to processing here."));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    case 'X':
      Serial.println(F("++ set programState = PLAYER_RUN"));
      programState = PLAYER_RUN;
      break;
    default:
      Serial.println();
      Serial.print(F("> "));
      break;
  } // end switch
}
void runProcessorWait() {
  Serial.println(F("+ Processor WAIT mode."));
  Serial.print(F("> "));
  while (programState == PROGRAM_WAIT) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      processWaitSwitch(readByte);
    }
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
  Serial.println(", version 0.92");

  // ----------------------------------------------------
  setupMp3Player();
  // functionNamePrint(__func__);
  // Serial.println(F(" DFPlayer is initialized."));
  // While testing, don't play: mp3playerPlaywait(1);
  // mp3PlayerPause();

  // ----------------------------------------------------
  programState = PLAYER_RUN;
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
    case PLAYER_RUN:
      host_clr_status_led_WAIT()
      // digitalWrite(HLDA_PIN, HIGH);
      mp3PlayerRun();
      // digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
