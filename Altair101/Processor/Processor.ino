// -----------------------------------------------------------------------------
/*
  Altair 101 Memory Definitions and Management.

  Reference > Language > Variables > Data types > Array 
    https://www.arduino.cc/reference/en/language/variables/data-types/array/
  A byte stores an 8-bit unsigned number, from 0 to 255.
    https://www.arduino.cc/reference/en/language/variables/data-types/byte/
  Array size, sizeof(): Reference > Language > Variables > Utilities > Sizeof 
    https://www.arduino.cc/reference/en/language/variables/utilities/sizeof/
  Octal,  Reference > Language > Variables > Constants > Integerconstants 
    https://www.arduino.cc/reference/en/language/variables/constants/integerconstants/
    leading "0" characters 0-7 valid, for example: 0303 is octal 303.

int memoryBytes = 1024;
byte arrayBytes[memoryBytes];
byte zeroByte = B00000000;
arrayBytes[0] = aByte;
for (int i = 0; i < memoryBytes; i++) {
  arrayBytes[i] = zeroByte;
}
byte jmpCode = 0303;    // Octal to byte.

// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
    0303, 0006, 0000,
    0000, 0000, 0000,
    0303, 0000, 0000
}
// Copy the jump loop program into the computer's memory array.
for (int i = 0; i < sizeof(jumpLoopProgram) - 1; i++) {
    arrayBytes[i] = jumpLoopProgram[i];
    Serial.printl(arrayBytes[i], DEC);
    Serial.printl(arrayBytes[i], BIN);
    Serial.println(arrayBytes[i], DEC);
}

-------------
void setup()
{
  Serial.begin(9600);
  byte myByte = B11000011;
  //byte myByte = 0xC3;
  //byte myByte = 195;
  Serial.println(myByte, DEC);
  Serial.println(myByte, BIN);
  Serial.println(myByte, HEX);
  shiftOut(1,1,LSBFIRST, myByte); // <<<< will shift out the same no matter which of the three assignments you choose above.
}

--------------------

int latchPin = 8;   //Pin connected to ST_CP of 74HC595
int clockPin = 12;  //Pin connected to SH_CP of 74HC595
int dataPin = 11;   //Pin connected to DS of 74HC595
byte myArray[] = {
    B00000000, B10000000, B11100000, 
    B01100000, B01000000, B01110000, 
    B00110000, B00100000, B00111000, 
    B00011000, B00010000, B00011100, 
    B00001100, B00001000, B00001110, 
    B00000110, B00000100, B00000111, 
    B00000011, B00000010, B1000011, 
    B10000001, B00000001, B11000001, 
    B11000000
};
void setup() {
  Serial.begin(9600);
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}
void loop() {
  for (int i = 0; i < sizeof(myArray)/sizeof(myArray[0]); i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, myArray[i]);
    digitalWrite(latchPin, HIGH);
    delay(100);
  }
}

*/
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // Setup code.

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  Serial.println("+ Looping.");
  delay(10000);
}
// -----------------------------------------------------------------------------
