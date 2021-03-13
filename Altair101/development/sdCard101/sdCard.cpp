// -------------------------------------------------------------------------------
/*
  This is an SD card control program.
  Functionality:
  + Setup SD card for processing.
  + List directory.
  + Select/enter a directory name for processing.
  + Print the selected file to the screen.
  + Delete the selected file.

  Sample directory listings:

  CARD ?- + n, SD card directory lising.
  + Initialized: SD card module.
  ++ Directory: FSEVEN~1
  ++ Directory: SPOTLI~1
  ++ File:      00000011.BIN  2048
  ++ File:      00000101.BIN  5376
  ++ File:      00000100.BIN  5120
  ++ File:      F1.TXT        44
  ++ File:      00000010.BIN  2048
  ++ File:      00000001.BIN  5376
  ++ File:      00010001.BIN  5120
  ++ File:      01000000.BIN  5376
  ++ File:      00001001.BIN  5376
  ++ File:      00000000.BIN  4608
  ++ File:      01000001.BIN  4066
  ++ File:      DIR.TXT       158
  ++ File:      00001011.BIN  4608
  ++ File:      00100000.BIN  5120
  ++ File:      00010000.BIN  5376
  ++ File:      11000000.BIN  5120
  ++ File:      10000000.BIN  5376

*/
// -------------------------------------------------------------------------------
#include "Altair101b.h"

void ledFlashError() {}
void ledFlashSuccess() {}

void host_set_status_led_HLDA() {};
void host_clr_status_led_HLDA() {};
void playerPlaySoundWait(int abc) {};
void controlResetLogic() {};
uint16_t fpAddressToggleWord = 0;
String getSenseSwitchValue() {
  return ("10000000");
};

String sdCardPrompt = "CARD ?- ";
String sdCardGetFilenamePrompt = "CARD FN ?- ";
String thePrompt = sdCardPrompt;           // Default.
extern int programState;

// Clock internal status, internatl to this program.
#define THIS_RUN 1
#define THIS_GET 2
int thisState = THIS_RUN;

char theBuffer[13]; // 12345678.123 + 1 for terminator.
int theBufferMaxLength = 12;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD Card setup and functions.

#define SETUP_SDCARD 1
String thisFilename;

#ifdef SETUP_SDCARD

// SD Card module is an SPI bus slave device.
#include <SPI.h>
#include <SD.h>

//     Mega Nano - SPI module pins
// Pin 53   10   - CS   : chip/slave select (SS pin). Can be any master(Nano) digital pin to enable/disable this device on the SPI bus.
// Pin 52   13   - SCK  : serial clock, SPI: accepts clock pulses which synchronize data transmission generated by Arduino.
// Pin 51   11   - MOSI : master out slave in, SPI: input to the Micro SD Card Module.
// Pin 50   12   - MISO : master in slave Out, SPI: output from the Micro SD Card Module.
// Pin 5V+  - VCC  : can use 3.3V or 5V
// Pin GND  - GND  : ground
// Note, Nano pins are declared in the SPI library for SCK, MOSI, and MISO.

// The CS pin is the only one that is not really fixed as any of the Arduino digital pin.
// const int csPin = 10;  // SD Card module is connected to Nano pin 10.
const int csPin = 53;     // SD Card module is connected to Mega pin 53.

File myFile;

int iFileBytes;

// -----------------------------------------------------------------------------
// SD card module functions
// The csPin pin is connected to the SD card module select pin (CS).

// Handle the case if the card is not inserted. Once inserted, the module will be re-initialized.
boolean sdcardInitiated = false;

void initSdCard() {
  if (SD.begin(csPin)) {
    sdcardInitiated = true;
  } else {
    Serial.println(F("- Error initializing SD card module."));
    ledFlashError();
    hwStatus = 1;
    sdcardInitiated = false;
    Serial.println(F("- Error initializing SD card."));
    Serial.println(F("-- Check that SD card is inserted"));
    Serial.println(F("-- Check that SD card adapter is wired properly."));
    // Optionally, retry for a period of time.
  }
}

void setupSdCard() {
  initSdCard();
  if (sdcardInitiated) {
    Serial.println(F("+ Initialized: SD card module."));
    ledFlashSuccess();
  }
}

// -----------------------------------------------------------------------------
// Read file into memory.

boolean readFileToMemory(String theFilename) {
  if (!sdcardInitiated) {
    initSdCard();
  }
  Serial.print(F("+ Read into memory, from file: "));
  Serial.print(theFilename);
  if (!SD.exists(theFilename)) {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return (false);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print(F("- Read ERROR, cannot open file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return (false);
  }
  Serial.print(F(", file size: "));
  Serial.print(myFile.size(), DEC);
  Serial.println();
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
  Serial.print(F("+ Read completed, number of bytes: "));
  Serial.print(iFileBytes);
  Serial.println();
  return (true);
}

// -----------------------------------------------------------------------------
// Print memory to a screen.

void printMemoryToScreen() {
  Serial.print(F("+ Print memory to a screen, number of bytes: "));
  Serial.print(iFileBytes);
  Serial.println();
  for (int i = 0; i < iFileBytes; i++) {
    byte memoryData = MREAD(i);
    Serial.write(memoryData);
  }
  Serial.println(F("<--"));
  Serial.println(F("+ Print completed."));
}

// -----------------------------------------------------------------------------
// Write memory to a file.

void writeMemoryToFile(String theFilename) {
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
    ledFlashError();
    sdcardInitiated = false;
    return;
  }
  for (int i = 0; i < iFileBytes; i++) {
    byte memoryData = MREAD(i);
    myFile.write(memoryData);
  }
  myFile.close();
  Serial.println(F("+ Write completed, file closed. Number of bytes: "));
  Serial.print(iFileBytes);
  Serial.println(".");
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
#endif  // SETUP_SDCARD

// -----------------------------------------------------------------------------
void getFilename() {
  thePrompt = sdCardGetFilenamePrompt;
  Serial.print(thePrompt);
  //
  // Initialize the clock set values.
  for (int i = 0; i < theBufferMaxLength; i++ ) {
    theBuffer[i] = ' ';                  // Set to empty.
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
    thisFilename = String(theBuffer);
    thisFilename.trim();  // Needs to be on its own line.
    /*
      Serial.println();
      Serial.print(F("++ Buffer :"));
      for (int i = 0; i < iBuffer; i++ ) {
      Serial.write(theBuffer[i]);
      }
      Serial.print(F(":"));
      Serial.print(thisFilename);
      Serial.print(F(":"));
    */
  }
  thePrompt = sdCardPrompt;
  Serial.println();
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// SD Card Controls

byte readConfirmByte;

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
    case 65: // Mouse wheel down (away from user).
    case 66: // Mouse wheel up   (toward user).
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
      readFileToMemory(thisFilename);
      break;
    case 'p':
      printMemoryToScreen();
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
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ SD Card Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ m, Read           Read an SD card file into program memory."));
      Serial.println(F("+ M, Write          Write program memory to an SD card file."));
      Serial.println(F("+ n, Directory      Directory file listing of the SD card."));
      Serial.println(F("------------------"));
      Serial.println(F("+ f, Filename       Enter a filename for processing."));
      Serial.println(F("+ i, Information    Program variables and hardward values."));
      Serial.println(F("+ r, Read file      Read the file into memory and write to screen."));
      Serial.println(F("+ p, Print memory   Print memory to screen."));
      Serial.println(F("+ w, Write file     Write memory array to file."));
      Serial.println(F("+ d, Delete         Delete the file from the SD card."));
      Serial.println(F("------------------"));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    case 'X':
      Serial.println(F("+ Power or Key TV/VCR"));
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
    Serial.print(thePrompt);
  }
}

// -----------------------------------------------------------------------------
// SD Card Control Loop.

void sdCardRun() {
  Serial.println(F("+ sdCardRun();"));
  Serial.print(thePrompt);
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