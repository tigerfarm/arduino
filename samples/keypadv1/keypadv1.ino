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

// -------------------------------------------
// This matches the keypad characters to pins.

// If only for the number 1 key: 1x1.
/*
const byte ROWS = 1;
const byte COLS = 1;
  char hexaKeys[ROWS][COLS] = {
  {'1'}
  };
  byte rowPins[ROWS] = { 5};    // D1
  byte colPins[COLS] = {14};    // D5
*/

// For keys: 1, 2, 4, 5: 2x2.
/*
const byte ROWS = 2;
const byte COLS = 2;
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
const byte ROWS = 3;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};
byte rowPins[ROWS] = { 5, 4, 0};    // D1 D2 D3
byte colPins[COLS] = {14,12, 3};    // D5 D6 D9(RX)


 // For keys: 1 ... 9: 4x4.
/*
const byte ROWS = 4;
const byte COLS = 4;
  char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };
  // For Arduino:
  // byte rowPins[ROWS] = {9, 8, 7, 6};
  // byte colPins[COLS] = {5, 4, 3, 2};  // If using a 3x4 keypad, remove: ", 2".
  //
  // For NodeMCU:
  // byte rowPins[ROWS] = { 5, 4, 0, ?}; // D1 D2     D3 ?
  // byte colPins[COLS] = {14, 3, 9, ?}; // D5 D9(RX) D0 ?
  //
  // S3 to keypad, use S2 as the pin
*/

// -------------------------------------------------------------------------------
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char keyPressed;
void keypadProcess() {
  Serial.print("+ Key pressed: ");
  Serial.println(keyPressed);
  int keyInt = keyPressed - '0';
  if (keyInt >= 1 && keyInt <= 6) {
    Serial.println("+ keyPressed >= 1 && keyPressed <= 6");
  }
  switch (keyPressed) {
    case '1':
      Serial.println("+ Keypad switch: 1");
      break;
    case '2':
      Serial.println("+ Keypad switch: 2");
      break;
    case '3':
      Serial.println("+ Keypad switch: 3");
      break;
    default:
      Serial.print("+ Keypad value: ");
      Serial.println(keyPressed);
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
  //
  if (keyPressed = customKeypad.getKey()) {
    keypadProcess();;
  }
}

// -----------------------------------------------------------------------
// eof
