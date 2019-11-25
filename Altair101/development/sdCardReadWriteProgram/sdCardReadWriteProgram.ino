// -----------------------------------------------------------------------------
/*
  Use a Micro SD Card Module to save and load binary 8080 machine language programs.
*/

// -----------------------------------------------------------------------------
// Test program to use, which tests the 16 bit add opcode, dad.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                //            ; Test opcode dad.
  //                //            ; --------------------------------------
  //                // Start:     ; Intialize register values.
  B00111110, 6,     // mvi a,6    ; Move numbers to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,0
  B00101110, 5,     // mvi l,0
  0343, 38,         // out 38     ; Print the Intialized register values.
  //                //            ; --------------------------------------
  //0RP1001
  B00001001,        // dad b      ; Add register pair B:C to H:L.
  0343, 36,         // out 36     ; Print register pair, H:L.
  B00011001,        // dad d      ; Add register pair D:E to H:L.
  0343, 36,         // out 36     ; Print register pair, H:L.
  //                //            ; --------------------------------------
  0166,             // hlt
  0303, 0000, 0000, // jmp Start  ; Jump back to beginning.
  0000              //            ; End.
};

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 256;
byte memoryData[memoryBytes];
unsigned int programCounter = 0;     // Program address value

// -----------------------------------------------------------------------------
// Memory Functions

char charBuffer[17];
byte zeroByte = B00000000;

void initMemoryToZero() {
  Serial.println(F("+ Initialize all memory bytes to zero."));
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = zeroByte;
  }
}

void copyByteArrayToMemory(byte btyeArray[], int arraySize) {
  Serial.println(F("+ Copy the program into the computer's memory array."));
  for (int i = 0; i < arraySize; i++) {
    memoryData[i] = btyeArray[i];
  }
  Serial.println(F("+ Copied."));
}

void listByteArray(byte btyeArray[], int arraySize) {
  Serial.println(F("+ List the program."));
  Serial.println(F("++   Address:       Data value"));
  for (int i = 0; i < arraySize; i++) {
    Serial.print(F("+ Addr: "));
    sprintf(charBuffer, "%4d:", i);
    Serial.print(charBuffer);
    Serial.print(F(" Data: "));
    printData(btyeArray[i]);
    Serial.println("");
  }
  Serial.println(F("+ End of listing."));
}

// -----------------------------------------------------------------------------
// Output: log messages and Front Panel LED data lights.

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}
void printWord(int theValue) {
  String sValue = String(theValue, BIN);
  for (int i = 1; i <= 16 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
}

void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

// -----------------------------------------------------------------------------
// Used with the SD Card module.
// SPI reference: https://www.arduino.cc/en/Reference/SPI
// SD card library reference: https://www.arduino.cc/en/reference/SD

#include <SPI.h>
#include <SD.h>

String theFilename = "f1.txt";  // Files are created using uppercase: F1.TXT.

// Set to match your SD module to the Nano pin.
// The CS pin is the only one that is not really fixed as any of the Arduino digital pin.
const int csPin = 10;  // SD Card module is connected to Nano pin 10.
File myFile;

// -----------------------------------------------------------------------------
// Open and write a file.

void openWriteFile() {
  Serial.println("+ Open and write to the file.");
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print("- Error opening file: ");
    Serial.println(theFilename);
    return; // When used in setup(), causes jump to loop().
  }
  Serial.print("++ File open: ");
  Serial.print(theFilename);
  Serial.println(", write text to file.");
  //
  // Starts writing from the end of file, i.e. appends to the file.
  myFile.println("Hello there,");
  myFile.println("Line 2: 1, 2, 3.");
  myFile.println("Last line.");
  myFile.close();
  Serial.println("+ File closed.");
}

// -----------------------------------------------------------------------------
// Open and read a file.

void openReadFile() {
  Serial.println("+ Open read from the file.");
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print("- Error opening file: ");
    Serial.println(theFilename);
    return; // When used in setup(), causes jump to loop().
  }
  while (myFile.available()) {
    // Reads one character at a time.
    // Serial.print("+ :");
    // Serial.print(myFile.read());  // Prints ascii character number, one per-line.
    // Serial.println(":");
    Serial.write(myFile.read());  // Prints as it was written.
  }
  myFile.close();
  Serial.println("+ File closed.");
}

// -----------------------------------------------------------------------------
// Delete the file and confirm it was deleted.

void deleteAndCheckFile() {
  Serial.print("+ Delete the file: ");
  Serial.println(theFilename);
  SD.remove(theFilename);
  //
  Serial.println("+ Check if file exists or not.");
  if (SD.exists(theFilename)) {
    Serial.print("++ Exists: ");
  } else {
    Serial.print("++ Doesn't exist: ");
  }
  Serial.println(theFilename);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ----------------------------------------------------
  int programSize = sizeof(theProgram);
  // List a program.
  listByteArray(theProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(theProgram, programSize);

  // ----------------------------------------------------
  // Note, csPin is optional. The default is the hardware SS line (pin 10) of the SPI bus.
  // If using pin, other than 10, add: pinMode(otherPin, OUTPUT);
  // The pin connected to the chip select pin (CS) of the SD card.
  if (!SD.begin(csPin)) {
    Serial.println("- Error initializing SD card.");
    return; // When used in setup(), causes jump to loop().
  }
  Serial.println("+ SD card initialized.");
  
  // Start by ensuring that the files does not exist.
  deleteAndCheckFile();

  // ----------------------------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  Serial.println("");
  Serial.println("---------------------------------------------");
  Serial.println("+ Loop: do a number of reads and writes.");
  Serial.println("---------------------------------------------");
  Serial.println("");
  openWriteFile();
  delay(3000);
  openReadFile();
  delay(3000);
  Serial.println("");
  Serial.println("---------------------------------------------");
  openWriteFile();
  delay(3000);
  openReadFile();
  delay(3000);
  //
  Serial.println("---------------------------------------------");
  deleteAndCheckFile();
  delay(10000);
}
// -----------------------------------------------------------------------------
