/********
  A basic implementation of a keypad.

  Project info which includes the connection diagram.
  http://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  Connect the keypad to digital pins: 2 to 9 for 4x4 keypad. 3 to 9 for 3x4 keypad.
  
  To compile, install the keypad library by Mark Stanley and Alexander Brevig.
  I installed version 3.1.1.
*/
#include <Keypad.h>

// For a 4x4 keypad:
const byte ROWS = 4;
const byte COLS = 4;                // Use 3 if a 3x4 keypad & remove the 4 column:
// This matches the keypad characters:
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};  // Remove ", 2" if using a 3x4 keypad.

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  Serial.println("--------------------------------");
  Serial.println("+++ Setup.");
  delay(3000);
}

void loop() {

  delay(30);
  char customKey = customKeypad.getKey();
  if (customKey) {
    Serial.print("+ Key pressed: ");
    Serial.println(customKey);
  }

}

// -----------------------------------------------------------------------
// eof
