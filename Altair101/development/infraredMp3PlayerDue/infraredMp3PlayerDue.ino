// -----------------------------------------------------------------------
/*
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.
*/
// -------------------------------------------------------------------------------
#include <Mp3PlayerDue.h>

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ----------------------------------------------------
  setupMp3Player();
  Serial.println(F("+ DFPlayer is initialized."));

  // ----------------------------------------------------

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
void loop() {
  playMp3();
}
// -----------------------------------------------------------------------------
