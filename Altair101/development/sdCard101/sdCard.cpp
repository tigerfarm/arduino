// -------------------------------------------------------------------------------
/*
  This is an SD card file management program.

  Functionality:
  + Setup SD card for processing.
  + List the root directory.
  + Select/enter a file name for processing.
  + Read the selected file into a memory array.
  + Print the memory array to the screen.
  + Write the memory array to a file.
  + Delete the selected file.

  -----------------------------------------------------------------------------
  +++ Next

  Basic line editor functions:
  + Delete a line.
    CARD ED ?- d 9
  + Add/insert a line.
    CARD ED ?- a Hello there.
    CARD ED ?- i 9 Hello there.

  + To fix, print if only 1 byte.
  CARD ?- + File: ABYTE.BIN, number of bytes: 1, number of bytes read: 1
  CARD ?- + Nothing to print. Memory bytes are all zero.

  Handle this in Altair101.
  + Allow manual update of single byte files.
  + Upload a file from the laptop to the Altair101 program memory.
  ++ Then the memory can be save to the SD card by this program.

  -----------------------------------------------------------------------------
  Wire the SD Card reader/writer module:

  From the back of the SD Card module, the pins:
  1    2    3    4    5    6
  CS   SCK  MOSI MISO VCC  GND
  53   Blck Brwn Whte red

  From the Front of the SD Card module, the pins:
  GND  VCC  MISO MOSI SCK  CS
       Red  Whte Brwn Blck To pin 53.

  Due SPI center pins (tested). Should work for Mega and Uno as well (not tested).
   ---------------------------------------
  |                                      -| GND
  |                 MISO x x + VCC        |
  |                  SCK x x MOSI         |
  |                RESET x x - GND        |
  |                                      x| pin 53
  |                                      +| VCC
   ---------------------------------------

  Or use digital-SPI pins:
  //     Mega Nano - SPI module pins
  // Pin 53   10   - CS   : chip/slave select (SS pin). Can be any master(Nano) digital pin to enable/disable this device on the SPI bus.
  // Pin 52   13   - SCK  : serial clock, SPI: accepts clock pulses which synchronize data transmission generated by Arduino.
  // Pin 51   11   - MOSI : master out slave in, SPI: input to the Micro SD Card Module.
  // Pin 50   12   - MISO : master in slave Out, SPI: output from the Micro SD Card Module.
  // Pin 5V+  - VCC  : can use 3.3V or 5V
  // Pin GND  - GND  : ground
  // Note, Nano pins are declared in the SPI library for SCK, MOSI, and MISO.
  // The CS pin is the only one that is not really fixed as any of the Arduino digital pinsåç.

*/
// -------------------------------------------------------------------------------
#include "Altair101b.h"
#include "cpuIntel8080.h"

String sdCardPrompt = "CARD ?- ";
String sdCardGetFilenamePrompt = "CARD FN ?- ";
String sdCardEditFilePrompt = "CARD ED ?- ";
String theCardPrompt = sdCardPrompt;           // Default.
extern int programState;

// Status internal to this program.
#define THIS_RUN 1
#define THIS_GET 2
#define THIS_EDIT 3
int thisState = THIS_RUN;

byte readConfirmByte;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD Card setup and functions.

// SD Card module is an SPI bus slave device.
#include <SPI.h>
#include <SD.h>

// const int csPin = 10;  // SD Card module is connected to Nano pin 10.
const int csPin = 53;     // SD Card module is connected to Mega pin 53.

File myFile;
String thisFilename;
int iFileBytes;

// -----------------------------------------------------------------------------
// SD card module functions

// Handle the case if the card is not inserted. Once inserted, the module will be re-initialized.
boolean sdcardInitiated = false;

void initSdCard() {
  if (SD.begin(csPin)) {
    sdcardInitiated = true;
  } else {
    Serial.println(F("- Error initializing SD card module."));
    hwStatus = 1;
    sdcardInitiated = false;
    Serial.println(F("- Error initializing SD card."));
    Serial.println(F("-- Check that SD card is inserted"));
    Serial.println(F("-- Check that SD card adapter is wired properly."));
    // Optionally, retry for a period of time.
  }
}

boolean setupSdCard() {
  initSdCard();
  if (sdcardInitiated) {
    Serial.println(F("+ Initialized: SD card module."));
    return true;
  } else {
    return false;
  }
}

// -----------------------------------------------------------------------------
// Read file into memory.

boolean readFileToMemory(String theFilename) {
  if (theFilename == "") {
    Serial.println(F("- Read file, missing file name."));
    return (false);
  }
  if (!sdcardInitiated) {
    initSdCard();
  }
  if (!SD.exists(theFilename)) {
    // Serial.print(F("- Read ERROR, file doesn't exist: "));
    // Serial.println(theFilename);
    sdcardInitiated = false;
    return (false);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    // Serial.print(F("- Read ERROR, cannot open file: "));
    // Serial.println(theFilename);
    sdcardInitiated = false;
    return (false);
  }
  int numFileBytes = myFile.size();
  iFileBytes = 0;
  while (myFile.available()) {
    // Read and process one character at a time.
    byte memoryData = myFile.read();
    // Serial.write(memoryData);
    MWRITE(iFileBytes, memoryData);
    iFileBytes++;
  }
  myFile.close();
  // Serial.println(F("<--"));
  // Serial.print(F("<-- Read completed, number of bytes: "));
  Serial.print(F("+ File: "));
  Serial.print(theFilename);
  Serial.print(F(", number of bytes: "));
  Serial.print(numFileBytes);
  Serial.print(F(", number of bytes read: "));
  Serial.print(iFileBytes);
  Serial.println();
  return (true);
}

// -----------------------------------------------------------------------------
int getMemoryByteCount() {
  // Calculate the number of bytes by removing memory trailing zeros.
  for (iFileBytes = MEMSIZE - 1; iFileBytes > 0; iFileBytes--) {
    if (MREAD(iFileBytes) != 0) {
      break;
    }
  }
  if (iFileBytes > 0) {
    iFileBytes++;       // To correct the last character being removed.
  }
  return iFileBytes;
}

// Print memory to a screen.

void listMemoryToScreen() {
  iFileBytes = getMemoryByteCount();
  if (iFileBytes == 0) {
    Serial.println(F("+ Nothing to list. Memory bytes are all zero."));
    return;
  }
  Serial.print(F("+ List memory bytes to the screen, number of bytes: "));
  Serial.print(iFileBytes);
  Serial.println();
  // First line.
  int lineCounter = 1;
  Serial.print(F("00"));
  Serial.print(lineCounter);
  Serial.print(F(": "));
  for (int i = 0; i < iFileBytes; i++) {
    byte memoryData = MREAD(i);
    if (memoryData != 10 && memoryData != 13) {
      Serial.write(memoryData);
    }
    if (memoryData == 10) {
      Serial.println();
      lineCounter++;
      if (lineCounter < 10) {
        Serial.print(F("00"));
      } else if (lineCounter < 100) {
        Serial.print(F("0"));
      }
      Serial.print(lineCounter);
      Serial.print(F(": "));
    }
  }
  Serial.println(F("<--(End of array)"));
  Serial.println();
}

// -----------------------------------------------------------------------------
// Write memory to a file.

boolean writeMemoryToFile(String theFilename) {
  if (getMemoryByteCount() == 0) {
    Serial.println(F("+ Nothing to write. Memory bytes are all zero."));
    return false;
  }
  if (!sdcardInitiated) {
    initSdCard();
  }
  Serial.print(F("+ Write memory to file: "));
  Serial.println(theFilename);
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print(F("- Error opening file: "));
    Serial.println(theFilename);
    sdcardInitiated = false;
    return false;
  }
  for (int i = 0; i < iFileBytes; i++) {
    byte memoryData = MREAD(i);
    myFile.write(memoryData);
  }
  myFile.close();
  Serial.print(F("+ Write completed, file closed. Number of bytes: "));
  Serial.print(iFileBytes);
  Serial.println(".");
  return true;
}

// -----------------------------------------------------------------------------
// Delete the file and confirm it was deleted.

void deleteFile(String theFilename) {
  Serial.print(F("+ Delete the file: "));
  Serial.print(theFilename);
  Serial.print(F(" ... "));
  SD.remove(theFilename);
  if (SD.exists(theFilename)) {
    Serial.print("- Delete failed.");
  } else {
    Serial.print("+ Deleted.");
  }
  Serial.println();
}

// -------------------------------------

boolean writeFileByte(String theFilename, byte theByte) {
  if (!sdcardInitiated) {
    initSdCard();
  }
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print(F("- Error opening file: "));
    Serial.println(theFilename);
    sdcardInitiated = false;
    return (false);
  }
  myFile.write(theByte);
  myFile.close();
  Serial.print(F("+ Byte written: "));
  Serial.print(theByte, DEC);
  Serial.print(F(" to: "));
  Serial.print(theFilename);
  Serial.println();
  return (true);
}

// Read a byte from a file.
int readFileByte(String theFilename) {
  if (!sdcardInitiated) {
    initSdCard();
  }
  if (!SD.exists(theFilename)) {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    sdcardInitiated = false;
    return (0);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print(F("- Read ERROR, cannot open file: "));
    Serial.println(theFilename);
    sdcardInitiated = false;
    return (0);
  }
  int returnByte = 0;
  if (myFile.available()) {
    returnByte = myFile.read();
  }
  myFile.close();
  Serial.print(F("+ Read byte: "));
  Serial.print(returnByte, DEC);
  Serial.print(F(" from: "));
  Serial.print(theFilename);
  Serial.println();
  return (returnByte);
}

// -----------------------------------------------------------------------------
// Directory listing

void printSpacing(String theString) {
  for (int i = theString.length(); i < 14; i++) {
    Serial.print(F(" "));
  }
}

void sdListDirectory() {
  if (!sdcardInitiated) {
    initSdCard();
  }
  // listDirectory(root);
  // List files for a single directory.
  File dir = SD.open("/");
  String tabString = "   ";
  File entry = dir.openNextFile();
  while (entry) {
    if (entry.isDirectory()) {
      Serial.print(F("++ Directory: "));
      Serial.print(entry.name());
    } else {
      Serial.print(F("++ File:      "));
      Serial.print(entry.name());
      printSpacing(entry.name());
      Serial.print(entry.size(), DEC);
    }
    Serial.println();
    entry.close();
    entry =  dir.openNextFile();
  }
  dir.close();
}

// -----------------------------------------------------------------------------
void getFilename() {
  theCardPrompt = sdCardGetFilenamePrompt;
  Serial.print(theCardPrompt);
  //
  // Initialize the input buffer to empty.
  char theBuffer[13]; // 12345678.123 + 1 for terminator.
  int theBufferMaxLength = 12;
  for (int i = 0; i <= theBufferMaxLength; i++ ) {
    theBuffer[i] = ' ';                     // Set to empty, all blanks.
  }
  thisState = THIS_GET;
  int iBuffer = 0;
  while (thisState == THIS_GET) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();         // Read and process an incoming byte.
      switch (readByte) {
        // -----------------------------------
        case 10:                            // CR or LF
        case 13:
          thisState = THIS_RUN;
          break;
        case 127:                           // Backspace
          if (iBuffer > 0) {
            Serial.print(F("\033[1D"));     // Esc[ValueD : Move the cursor left Value number of spaces.
            Serial.print(F(" "));           // Print a space to remove the previous character.
            Serial.print(F("\033[1D"));     // Move the cursor back the the removed character space.
            iBuffer--;
            theBuffer[iBuffer] = ' ';
          }
          break;
        default:
          if (readByte >= 'a' && readByte <= 'z') {
            readByte = readByte - 32;     // Convert from lowercase to uppercase, as all filenames are in uppercase.
          }
          if ((iBuffer < theBufferMaxLength) && (
                (readByte >= '0' && readByte <= '9')
                || (readByte >= 'A' && readByte <= 'Z')
                || readByte == '.' || readByte == '_' || readByte == '-' || readByte == '~')
             ) {
            // If a valid filename character.
            // Filename reference: https://en.wikipedia.org/wiki/8.3_filename
            //    Number of characters: 8.3
            Serial.write(readByte);
            theBuffer[iBuffer] = readByte;
            iBuffer++;
          } // else ignore the character.
      }
    }
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
  if (iBuffer > 0) {
    thisFilename = "";
    for (int i = 0; i < iBuffer; i++ ) {
      // Serial.write(theBuffer[i]);
      thisFilename = thisFilename + String(theBuffer[i]);
    }
  }
  theCardPrompt = sdCardPrompt;
  Serial.println();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void editDeleteLine(int deleteLineNumber) {
  iFileBytes = getMemoryByteCount();
  if (iFileBytes == 0) {
    Serial.println(F("+ Nothing to edit. Memory bytes are all zero."));
    return;
  }
  Serial.print(F("+ Delete line from memory #"));
  Serial.print(deleteLineNumber);
  Serial.println(".");
  int startDelete = 0;
  int endDelete = 0;
  int totalDelete = 0;
  // First line.
  int lineCounter = 1;
  Serial.print(F("00"));
  Serial.print(deleteLineNumber);
  Serial.print(F(": "));
  if (deleteLineNumber == lineCounter) {
    Serial.print(F("(Start delete of this line)"));
    startDelete = 0;
  }
  for (int i = 0; i < iFileBytes; i++) {
    byte memoryData = MREAD(i);
    if (memoryData != 10 && memoryData != 13) {
      Serial.write(memoryData);
    }
    if (endDelete > 0) {
      MWRITE(startDelete, memoryData);
      startDelete++;
    }
    if (memoryData == 10) {
      if (deleteLineNumber == lineCounter) {
        Serial.print(F("(end delete this line)"));
        endDelete = i;
        totalDelete = endDelete - startDelete;
      }
      Serial.println();
      lineCounter++;
      if (deleteLineNumber == lineCounter) {
        Serial.print(F("(Start delete of this line)"));
        startDelete = i + 1;
      }
      if (lineCounter < 10) {
        Serial.print(F("00"));
      } else if (i < 100) {
        Serial.print(F("0"));
      }
      Serial.print(lineCounter);
      Serial.print(F(": "));
    }
  }
  if (deleteLineNumber == lineCounter) {
    // Delete last line.
    for (int i = startDelete - 1; i < iFileBytes; i++) {
      MWRITE(i, 0);
    }
  } else {
    // The delete line is over written with higher lines.
    // Now, set the trailing bytes to zeros.
    for (int i = iFileBytes - totalDelete - 1; i < iFileBytes; i++) {
      MWRITE(i, 0);
    }
  }
  Serial.println(F("<--(End of array)"));
  Serial.println();
}

// -----------------------------------------------------------------------------
void editAddString(String theLine) {
  iFileBytes = getMemoryByteCount();
  int numOfBytes = theLine.length();
  if (numOfBytes < 1) {
    Serial.println(F("+ Nothing to add."));
    return;
  }
  // -----------------------------------
  // Need to check the
  boolean backslash = false;
  iFileBytes++;
  for (int i = 0; i < numOfBytes; i++ ) {
    // Read and process one character at a time.
    byte memoryData = theLine[i];
    if (memoryData == '\\') {
      backslash = true;
      // backslash is never printed.
    } else if (backslash && memoryData == 'n') {
      backslash = false;
      // Handle newline character.
      iFileBytes++;
      MWRITE(iFileBytes + i, 10);   // Add a newline character.
    } else {
      backslash = false;
      MWRITE(iFileBytes + i, memoryData);
      iFileBytes++;
    }
  }
  Serial.print(F("+ Added string to memory :"));
  Serial.print(theLine);
  Serial.println(":");
}

// -----------------------------------------------------------------------------
String getEditCommandline() {
  // Initialize the input string and character buffer to empty.
  String thisLine = "";
  int theBufferMaxLength = 80;            // Max line input length.
  char theBuffer[81];
  for (int i = 0; i < theBufferMaxLength; i++ ) {
    theBuffer[i] = ' ';                     // Set to empty, all blanks.
  }
  thisState = THIS_GET;
  int iBuffer = 0;
  Serial.print(theCardPrompt);
  while (thisState == THIS_GET) {
    if (Serial.available() > 0) {
      int readByte = Serial.read();         // Read and process an incoming byte.
      if (iBuffer == 0
          && readByte == 'h'
          || readByte == 'l'
          || readByte == 'w'
          || readByte == 'X' ) {
        theBuffer[0] = readByte;
        iBuffer++;
        thisState = THIS_EDIT;
      } else {
        switch (readByte) {
          // -----------------------------------
          case 10:                            // CR or LF
          case 13:
            thisState = THIS_EDIT;
            break;
          case 127:                           // Backspace
            if (iBuffer > 0) {
              Serial.print(F("\033[1D"));     // Esc[ValueD : Move the cursor left Value number of spaces.
              Serial.print(F(" "));           // Print a space to remove the previous character.
              Serial.print(F("\033[1D"));     // Move the cursor back the the removed character space.
              iBuffer--;
              theBuffer[iBuffer] = ' ';
            }
            break;
          case 12:
            // Ctrl+L, clear screen.
            Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
            thisState = THIS_EDIT;
          default:
            if ( (iBuffer < theBufferMaxLength) && (readByte >= 32 && readByte <= 126) ) {
              // Printable characters: 32(' ') to 126('~').
              Serial.write(readByte);
              theBuffer[iBuffer] = readByte;
              iBuffer++;
            } // else ignore the character.
        }
      }
    }
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
  if (iBuffer > 0) {
    thisLine = "";
    for (int i = 0; i < iBuffer; i++ ) {
      // Serial.write(theBuffer[i]);
      thisLine = thisLine + String(theBuffer[i]);
    }
  }
  Serial.println();
  return thisLine;
}

// -----------------------------------------------------------------------------
void editFile() {
  theCardPrompt = sdCardEditFilePrompt;
  thisState = THIS_EDIT;
  String theCommand = "Start";
  String aString;
  boolean editChange = false;
  while (theCommand != "X") {
    theCommand = getEditCommandline();
    if (theCommand == "w") {
      Serial.print(F("+ Confirm write memory to file, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ Write cancelled."));
      } else {
        Serial.println(F("+ Write confirmed."));
        writeMemoryToFile(thisFilename);
      }
    } else if (theCommand.startsWith("a ")) {
      // Add string.
      editAddString(theCommand.substring(2));
    } else if (theCommand.startsWith("d ")) {
      // Delete line #.
      if (theCommand.length() > 1) {
        aString = theCommand.substring(2);
        aString.trim();
      }
      editDeleteLine(aString.toInt());
    } else if (theCommand == "i") {
      Serial.println(F("+ Insert, not implemented."));
    } else if (theCommand == "l") {
      listMemoryToScreen();
    } else if (theCommand == "h") {
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Memory Editor Commands"));
      Serial.println(F("------------------"));
      Serial.println(F("+ a <string>          Add the string to the end of the memory."));
      Serial.println(F("+ d <line#>           Delete line number."));
      Serial.println(F("+ i <line#> <string>  Not implemented: Insert string at line number location."));
      Serial.println(F("+ l, List memory      List memory to screen."));
      Serial.println(F("+ w, Write            Write edited memory to file."));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L              Clear screen."));
      Serial.println(F("+ X, Exit             Exit the Editor."));
      Serial.println(F("----------------------------------------------------"));
    }
  }
  theCardPrompt = sdCardPrompt;
  Serial.println();
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// SD Card Controls

void sdCardSwitch(int resultsValue) {
  boolean printPrompt = true;
  switch (resultsValue) {
    // -----------------------------------
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    // Mouse wheel
    case 27:
    case 91:
    case 65: // Character "A", Mouse wheel down (away from user).
    case 66: // Character "B", Mouse wheel up   (toward user).
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 10:
      // LF, IDE character.
      // Ignore.
      return;     // To avoid printing the prompt.
      break;
    // -------------
    case 13:
      // CR, Mac terminal window character. ignore
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Controls
    //
    case 'n':
      Serial.println(F("+ n, SD card directory lising."));
      sdListDirectory();
      break;
    // -----------------------------------
    case 'f':
      Serial.println(F("+ Enter a filename for processing."));
      getFilename();
      break;
    case 'i':
      Serial.print(F("+ File information for: "));
      Serial.print(thisFilename);
      if (SD.exists(thisFilename)) {
        Serial.print(F(", file exists"));
        myFile = SD.open(thisFilename);
        if (!myFile) {
          Serial.print(F(" - Read ERROR, cannot open file: "));
          Serial.print(thisFilename);
        } else {
          Serial.print(F(", file size: "));
          Serial.print(myFile.size(), DEC);
          myFile.close();
        }
      } else {
        Serial.print(F(", file does not exist."));
      }
      Serial.println();
      break;
    case 'r':
      if (!readFileToMemory(thisFilename)) {
        Serial.println(F("- Read ERROR."));
      }
      break;
    case 'l':
      listMemoryToScreen();
      break;
    case 'd':
      Serial.print(F("+ Delete file: "));
      Serial.print(thisFilename);
      Serial.println();
      Serial.print(F("+ Confirm delete, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ Delete cancelled."));
        break;
      }
      Serial.println(F("+ Delete confirmed."));
      deleteFile(thisFilename);
      break;
    case 'w':
      if (getMemoryByteCount() == 0) {
        Serial.println(F("+ Nothing to write. Memory bytes are all zero."));
        return;
      }
      Serial.print(F("+ Write memory to file: "));
      Serial.print(thisFilename);
      Serial.println();
      Serial.print(F("+ Confirm write, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ Write cancelled."));
        break;
      }
      Serial.println(F("+ Write confirmed."));
      writeMemoryToFile(thisFilename);
      break;
    case 'R':
      readFileByte("aByte.bin");
      break;
    case 'W':
      writeFileByte("aByte.bin", 'b');
      break;
    case 'e':
      Serial.println(F("+ Edit file."));
      editFile();
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ SD Card Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ n, Directory      Root directory file listing of the SD card."));
      Serial.println(F("------------------"));
      Serial.println(F("+ f, Filename       Enter a filename for processing."));
      Serial.println(F("+ i, Information    File information for the entered filename."));
      Serial.println(F("+ r, Read file      Read the file into memory."));
      Serial.println(F("+ l, List memory    List memory to screen."));
      Serial.println(F("+ w, Write file     Write memory array to file."));
      Serial.println(F("+ e, Edit           Edit file memory."));
      Serial.println(F("+ d, Delete         Delete the file from the SD card."));
      Serial.println(F("+ R/W, Read/Write   A byte from/to a file."));
      Serial.println(F("------------------"));
      Serial.println(F("+ C, Clear memory   Set memory array to zero, and file byte counter to zero."));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit         Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 'C':
      iFileBytes = 0;
      if (getMemoryByteCount() == 0) {
        Serial.println(F("+ Memory is already clear, all memory bytes are all zero."));
        return;
      }
      Serial.println(F("+ C, CLR: Clear memory, set registers to zero, set data and address to zero."));
      Serial.print(F("+ Confirm CLR, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ CLR cancelled."));
        break;
      }
      Serial.println(F("+ CLR confirmed."));
      Serial.print(F("+ Clear memory: "));
      Serial.println(MEMSIZE);
      for (int i = 0; i < MEMSIZE; i++) {
        MWRITE(i, 0);
      }
      break;
    // ---------------------------------
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    case 'X':
      Serial.println(F("+ Exit SD card mode."));
      programState = PROGRAM_WAIT;
      break;
    // -----------------------------------
    default:
      // Serial.print(F("+ Result value: "));
      // Serial.print(resultsValue);
      // Serial.print(F(", HEX: "));
      // Serial.println(resultsValue, HEX);
      printPrompt = false;
      break;
      // ----------------------------------------------------------------------
  } // end switch
  if (printPrompt && (programState == SDCARD_RUN)) {
    Serial.print(theCardPrompt);
  }
}

// -----------------------------------------------------------------------------
// SD Card Control Loop.

void sdCardRun() {
  Serial.println(F("+ sdCardRun();"));
  Serial.print(theCardPrompt);
  while (programState == SDCARD_RUN) {
    // Process serial input key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      sdCardSwitch(readByte);
    }
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
