// -----------------------------------------------------------------------
/*
  This sample program includes:
  + Initializing the MP3 player.
  + Making demonstrative calls to Mp3PlayerDue.cpp.
  
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
// This is for running the test program.
// The goal is to include the MP3 player program with Altair101a.
#include "Altair101b.h"

#include "Mp3Player.h"
boolean VIRTUAL_FRONT_PANEL;
void initVirtualFrontPanel() {}
extern void printVirtualFrontPanel() {}
void playerLights(uint8_t statusByte, uint8_t playerVolume, uint8_t songNumberByte) {}
uint16_t fpAddressToggleWord = 5; // MP3, "Transfer complete."

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
  boolean printPrompt = true;
  switch (readByte) {
    // -------------------------------
    case 10:
      // LF, ignore
      printPrompt = false;
      break;
    case 13:
      // CR
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
    case 'r':
      playerSwitch('r');
      break;
    case 's':
      playerSwitch('s');
      break;
    case 'v':
      playerSwitch('v');
      break;
    case 'V':
      playerSwitch('V');
      break;
    case 'i':
      playerSwitch('i');
      break;
    // ------------------------------
    case 'g':
      Serial.print(F("++ mp3PlayerStart();"));
      mp3PlayerStart();
      break;
    case 'p':
      Serial.print(F("++ mp3PlayerPause();"));
      mp3PlayerPause();
      break;
    // ------------------------------
    case '1':
      mp3playerSinglePlay(1);
      break;
    case '2':
      mp3playerSinglePlay(2);
      break;
    case '3':
      mp3playerSinglePlay(69);
      break;
    case '4':
      mp3PlayerSingleLoop(1);
      break;
    case '5':
      mp3PlayerSingleLoop(2);
      break;
    case '6':
      mp3PlayerSingleLoop(69);
      break;
    case '7':
      mp3playerPlaywait(1);
      break;
    case '8':
      mp3playerPlaywait(2);
      break;
    case '9':
      mp3playerPlaywait(69);
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ MP3 Player controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ s, STOP         Pause, stop playing."));
      Serial.println(F("+ r, RUN          Start, playing the current MP3."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("+ i, Information  Program variables and hardward values."));
      Serial.println(F("------------------"));
      Serial.println(F("+ p, Pause        mp3PlayerPause();       Pause playing."));
      Serial.println(F("+ g, Go, Start    mp3PlayerStart();       Start playing the MP3."));
      Serial.println(F("+ 1 ...3          mp3playerSinglePlay(_); Play a single MP3. Stop when finished."));
      Serial.println(F("+ 4 ...6          mp3PlayerSingleLoop(_); Loop play a single MP3."));
      Serial.println(F("+ 7 ...9          mp3playerPlaywait(_);   Play an MP3 and wait until completed."));
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
      Serial.print(F("++ set programState = PLAYER_RUN"));
      programState = PLAYER_RUN;
      break;
    default:
      printPrompt = false;
      break;
  } // end switch
  if (printPrompt) {
    Serial.println();
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
    playerContinuous();              // Check for player update or infrared key pressed.
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

  setupMp3Player();

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
