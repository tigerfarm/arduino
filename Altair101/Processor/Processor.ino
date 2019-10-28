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
    // "/sizeof(myArray[0]" not needed when array items are byte size.
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, myArray[i]);
    digitalWrite(latchPin, HIGH);
    delay(100);
  }
  }

*/

// -----------------------------------------------------------------------------
// Memory definitions

int memoryBytes = 1024;
byte arrayBytes[1024];

// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
  0303, 0006, 0000,
  0000, 0000, 0000,
  0303, 0000, 0000
};

// -----------------------------------------------------------------------------
// Byte processing

byte zeroByte = B00000000;
char charBuffer[16];

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
}

void printData(byte theByte) {
  printByte(theByte);
  // Serial.print(arrayBytes[i], BIN);
  Serial.print(" :  ");
  printOctal(theByte);
  // Serial.print(arrayBytes[i], OCT);
  Serial.print(" : ");
  // Serial.println(arrayBytes[i], DEC);
  sprintf(charBuffer, "%3d", theByte);
  Serial.print(charBuffer);
}

// -----------------------------------------------------------------------------
// Memory Functions

void initMemoryToZero() {
  Serial.println("+ Initialize all memory bytes to zero.");
  for (int i = 0; i < memoryBytes; i++) {
    arrayBytes[i] = zeroByte;
  }
}

void copyByteArrayToMemory() {
  Serial.println("+ Copy the jump loop program into the computer's memory array.");
  for (int i = 0; i < sizeof(jumpLoopProgram); i++) {
    arrayBytes[i] = jumpLoopProgram[i];
  }
  Serial.println("+ Copied.");
}

void listByteArray() {
  Serial.println("+ List the jump loop program.");
  for (int i = 0; i < sizeof(jumpLoopProgram); i++) {
    Serial.print("++ ");
    printData(jumpLoopProgram[i]);
    Serial.println("");
  }
  Serial.println("+ End of listing.");
}

// -----------------------------------------------------------------------------
// Operational Instruction

const byte jmpCode = 0303;    // Octal stored as a byte.

void processByte(byte theByte) {
  switch (theByte) {
    case jmpCode:
      Serial.print(" > Jump Instruction");
      break;
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  listByteArray();
  copyByteArrayToMemory();

  Serial.println("+++ Go to process loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int programCounter = 0;
void loop() {

  Serial.print("+ ");
  sprintf(charBuffer, "%3d", programCounter);
  Serial.print(charBuffer);
  Serial.print(" > ");
  printData(arrayBytes[programCounter]);
  processByte(arrayBytes[programCounter]);
  Serial.println("");
  //
  programCounter++;

  delay(3000);
}
// -----------------------------------------------------------------------------
