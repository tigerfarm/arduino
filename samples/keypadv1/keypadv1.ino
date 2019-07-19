/********
  A basic implementation of a keypad.

  Project info which includes the connection diagram.
  http://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  Connect the keypad to digital pins: 2 to 9 for 4x4 keypad. 3 to 9 for 3x4 keypad.

  Keypad pins are: first the rows, then the columns.
    For a 4x4 Keypad, the first (left most) 4 pins are for the rows, then the columns:
    Row 1, 2, 3, 4. Column 1, 2, 3, 4.
  
  To compile, install the keypad library by Mark Stanley and Alexander Brevig.
  I installed version 3.1.1.

+++ NodeMCU GPIO pins
Label   Pin:GPIO
D0      16          Button
D1      05          Keypad
D2      04          Keypad
D3      00          Keypad
D5      14          Keypad
D6      12          Out to an LED(+), LED(-) to a resister, then to ground.
D7(RX)  13          Infra red input.
D8(TX)  15
D9 (RX) 03          Keypad
D10(TX) 01
------------------
S3      10          Keypad: actual pin      (requires both)
S2      09          Keypad: pin declaration (requires both)

*/
// -------------------------------------------------------------------------------
#include <Keypad.h>

// For a 4x4 keypad. Match the number of rows and columns to keypad.
const byte ROWS = 4;
const byte COLS = 4;

// -------------------------------------------
// This matches the keypad characters to pins.

// If only for the number 1 key: 1x1.
/*
char hexaKeys[ROWS][COLS] = {
  {'1'}
};
byte rowPins[ROWS] = { 5};    // D1
byte colPins[COLS] = {14};    // D5
*/

// For keys: 1, 2, 4, 5: 2x2.
/* 
char hexaKeys[ROWS][COLS] = {
  {'1', '2'},
  {'4', '5'},
};
byte rowPins[ROWS] = { 5, 4};    // D1 D2
byte colPins[COLS] = {14, 3};    // D5 D9(RX)
*/

// For keys: 1 ... 9: 3x3.
/*
*/
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};
byte rowPins[ROWS] = { 5, 4, 0};    // D1 D2     D3
byte colPins[COLS] = {14, 3, 9};    // D5 D9(RX) S3+S2: S3 to keypad, use S2 as the pin

// For keys: 1 ... 9: 4x4.
/*
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// For Arduino:
// byte rowPins[ROWS] = {9, 8, 7, 6};
// byte colPins[COLS] = {5, 4, 3, 2};  // If using a 3x4 keypad, remove: ", 2".
// For NodeMCU:
// byte rowPins[ROWS] = { 5, 4, 0, ?}; // D1 D2     D3 ?
// byte colPins[COLS] = {14, 3, 9, ?}; // D5 D9(RX) S3 ?
*/

// -------------------------------------------------------------------------------
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void checkKeypad() {
  char customKey = customKeypad.getKey();
  if (customKey) {
    Serial.print("+ Key pressed: ");
    Serial.println(customKey);
  }
}

// -------------------------------------------------------------------------------
void setup() {
  // Serial.begin(9600);
  Serial.begin(115200);
  delay(100);
  Serial.println("--------------------------------");
  Serial.println("+++ Setup completed.");
  delay(3000);
}

void loop() {
  delay(30);
  checkKeypad();
}

// -----------------------------------------------------------------------
// eof
