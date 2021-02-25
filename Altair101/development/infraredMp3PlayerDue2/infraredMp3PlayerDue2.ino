// -----------------------------------------------------------------------
/*
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.
*/
// -------------------------------------------------------------------------------
#include "Mp3PlayerDue.h"

// -----------------------------------------------------------------------------
// Altair101a.ino program
// Program states
#define PROGRAM_WAIT 1
#define CLOCK_RUN 3
#define PLAYER_RUN 4

int programState;


word status_wait = 1;     // Default on
#define host_read_status_led_WAIT()   status_wait
#define host_clr_status_led_WAIT()  { digitalWrite(13, LOW);  status_wait = false; }
#define host_set_status_led_WAIT()  { digitalWrite(13, HIGH); status_wait = true; }

// -----------------------------------------------------------------------------
void runProcessorWait() {
  Serial.println(F("+ runProcessorWait()"));
  Serial.print(F("?- "));
  while (programState == PROGRAM_WAIT) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      // processWaitSwitch(readByte);
      if ( readByte == 'g' ) {
        Serial.println(F("++ playMp3Pause();"));
        playMp3Pause();
      } else if ( readByte == 'G' ) {
        Serial.println(F("++ set programState = PLAYER_RUN"));
        programState = PLAYER_RUN;
      }
      Serial.print(F("?- "));
    }
    delay(60);
  }
}

// -----------------------------------------------------------------------------
void functionPrint(String theName) {
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
  functionPrint(__func__);
  Serial.println();

  // ----------------------------------------------------
  setupMp3Player();
  functionPrint(__func__);
  Serial.println(F(" DFPlayer is initialized."));

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
      runMp3Player();
      // digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
