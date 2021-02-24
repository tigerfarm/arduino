// -----------------------------------------------------------------------
/*
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.
*/
// -------------------------------------------------------------------------------
#include "Mp3PlayerDue.h"

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
  Serial.println("+ Start the loop().");
}

// -----------------------------------------------------------------------------
void loop() {
  mp3player();
}
// -----------------------------------------------------------------------------
