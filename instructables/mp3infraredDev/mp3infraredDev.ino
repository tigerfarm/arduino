/*
  -------------------------------------------------------------------------
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.

  This version controls the currently played MP3 rather than using various library functions.

  -------------------------------------------------------------------------
  Program functionality:
  + Key: Function
  +  01: Volume down
  +  02: Set to directory #2.
  +  03: Volume up
  + 4...9: Select the following equalizer settings:
  ++ (4)DFPLAYER_EQ_POP (5)DFPLAYER_EQ_CLASSIC (6)DFPLAYER_EQ_NORMAL
  ++ (7)DFPLAYER_EQ_ROCK (8)DFPLAYER_EQ_JAZZ (9)DFPLAYER_EQ_BASS
  +  OK: Pause
  +  OK: Play
  +  >>: Play next
  +  <<: Play previous
  +  Up: Play next directory songs
  +  Dn: Play previous directory songs
  + *|Return: Loop single song: on
  + #|Exit: Loop single song: off
*/
// -----------------------------------------------------------------------

#define SWITCH_MESSAGES 1

// -----------------------------------------------------------------------
// Infrared Receiver

#include <IRremote.h>

// Infrared receiver
int IR_PIN = A1;      // Digital and analog pins work. Also tested with A0 and 9.
IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------
// DFPlayer Mini MP3 play

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3player;

// ------------------------------------
// The following is not needed for Mega because it has it's own hardware RX and TX pins.
//
// For communicating a Nano or Uno with the DFPlayer, use SoftwareSerial:
// #include "SoftwareSerial.h"
// DFPlayer pins 3(TX) and 2(RX), connected to Arduino pins: 10(RX) and 11(TX).
// SoftwareSerial playerSerial(10, 11); // Software serial, playerSerial(RX, TX)
// ------------------------------------

int currentSingle = 1;      // First song played when player starts up. Then incremented when next is played.
int currentDirectory = 1;   // File directory name on the SD card. Example 1 is directory name: /01.
boolean playPause = false;  // For toggling pause.
boolean loopSingle = false; // For toggling single song.

// -----------------------------------------------------------------------
// DFPlayer configuration and error messages.

void printDFPlayerMessage(uint8_t type, int value);
void printDFPlayerMessage(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          Serial.print("+ Assume the directory number was incremented too high, play previous directory: ");
          if (currentDirectory > 1) {
            currentDirectory --;
          } else {
            currentDirectory = 1;
          }
          Serial.println(currentDirectory);
          mp3player.loopFolder(currentDirectory);
          break;
        default:
          Serial.println(F("Unknown DFPlayer error message value:"));
          Serial.print(value);
          break;
      }
      break;
    default:
      Serial.println(F("Unknown DFPlayer message type: "));
      Serial.print(type);
      Serial.print(F(", value:"));
      Serial.print(value);
      break;
  }
}

void echoCurrentInfo() {
  Serial.println("+ --------------------------------------");
  Serial.print("+ readCurrentFileNumber: ");
  Serial.println(mp3player.readCurrentFileNumber());   // current play file number
  Serial.print("+ readState: ");
  Serial.println(mp3player.readState());               // mp3 state
  Serial.print("+ readEQ: ");
  Serial.println(mp3player.readEQ());                  // EQ setting
  Serial.print("+ readFileCounts: ");
  Serial.println(mp3player.readFileCounts());          // all file counts in SD card
  Serial.print("+ readFileCountsInFolder 01: ");
  Serial.println(mp3player.readFileCountsInFolder(1)); // fill counts in folder SD:/01
  Serial.print("+ readVolume: ");
  Serial.println(mp3player.readVolume());              // current sound volume
  /*
     Sample output:
    + readCurrentFileNumber: 1
    + readState: 514
    + readEQ: 4
    + readFileCounts: 486
    + readFileCountsInFolder 01: 8
    + readVolume: 12

  */
}

// -----------------------------------------------------------------------------
// Handle continuous playing, and play errors such as: SD card not inserted.

void playMp3() {
  if (mp3player.available()) {
    int theType = mp3player.readType();
    // ------------------------------
    if (theType == DFPlayerPlayFinished) {
      if (loopSingle) {
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Loop/play the same MP3: "));
        Serial.println(playerCounter);
#endif
      } else {
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Play the next MP3: "));
#endif
        if (playerCounter < playerCounterTop) {
          playerCounter++;
        } else {
          playerCounter = 1;
        }
      }
      mp3player.play(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      lightsStatusAddressData(playerStatus, playerCounter, playerVolume);
#ifdef SWITCH_MESSAGES
      Serial.println(playerCounter);
#endif
      // ------------------------------
    } else if (theType == DFPlayerCardInserted ) {
      Serial.println(F("+ SD mini card inserted. Start playing"));
      mp3player.start();
    } else {
      // Print the detail message from DFPlayer to handle different errors and states,
      //   such as memory card not inserted.
      printDFPlayerMessage(theType, mp3player.read());
    }
  }
}

// -----------------------------------------------------------------------
// Infrared DFPlayer controls

void playerInfraredSwitch() {
  // Serial.println("+ playerInfraredSwitch");
  //
  // Consider controls based on state.
  //  For example, if pause, then only allow unpause (OK key).
  //
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    // Song control
    case 0xFF10EF:
    case 0xE0E0A659:
    // Switch logic
    if (playerCounter > 1) {
      playerCounter--;
    } else {
      playerCounter = playerCounterTop;
    }
    mp3player.play(playerCounter);
    lightsStatusAddressData(playerStatus, playerCounter, playerVolume);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Examine: play previous song, playerCounter="));
    Serial.println(playerCounter);
#endif
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
    // Switch logic
    if (playerCounter < playerCounterTop) {
      playerCounter++;
    } else {
      playerCounter = 1;
    }
    mp3player.play(playerCounter);
    lightsStatusAddressData(playerStatus, playerCounter, playerVolume);
#ifdef SWITCH_MESSAGES
    Serial.println("+ Key > - next");
    Serial.print(F("+ Player, Examine Next: play next song, playerCounter="));
    Serial.println(playerCounter);
#endif
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println("+ Key OK - Toggle: pause and start the song.");
      // echoCurrentInfo();
      Serial.print(F("+ Current FileNumber: "));
      Serial.println(mp3player.readCurrentFileNumber());
      if (playPause) {
        mp3player.start();
        playPause = false;
      } else {
        mp3player.pause();
        playPause = true;
      }
      break;
    case 0xFF9867:
    case 0xE0E08877:
      // Serial.println("+ Key 0 - Pause");
      mp3player.pause();
      playPause = true;
      break;
    // -----------------------------------
    // Loop a single song: on/off
    case 0xFF6897:
    case 0xE0E01AE5:
      // Serial.println("+ Key * (Return) - Loop on: loop this single MP3.");
      mp3player.pause();   // Pause identifies that loop is on. Else I need a LED to indicate loop is on.
      delay(200);
      mp3player.start();
      loopSingle = true;
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      // Serial.println("+ Key # (Exit) - Loop off: Single MP3 loop is off.");
      if (loopSingle) {
        mp3player.pause(); // Pause identifies that loop is now off. Else I need a LED to indicate loop is on.
        delay(1000);
        mp3player.start();
        loopSingle = false;
      }
      break;
    // -----------------------------------
    // Folder, file directory selection.
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.print("+ Key up - next directory, directory number: ");
      currentDirectory ++;
      // Serial.println(currentDirectory);
      mp3player.loopFolder(currentDirectory);
      // If no directory, get the error message: DFPlayerError:Cannot Find File
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Serial.print("+ Key down - previous directory, directory number: ");
      if (currentDirectory > 1) {
        currentDirectory --;
      }
      // Serial.println(currentDirectory);
      mp3player.loopFolder(currentDirectory);
      break;
    // -----------------------------------
    case 0xFFA25D:
      // Serial.print("+ Key 1: ");
      // Serial.println("Volume Down");
      mp3player.volumeDown();
      break;
    case 0xFF629D:
      // Serial.print("+ Key 2: ");
      // Serial.println("File directory 2");
      currentDirectory = 2;
      mp3player.loopFolder(currentDirectory);
      break;
    case 0xFFE21D:
      // Serial.print("+ Key 3: ");
      // Serial.println("Volume UP");
      mp3player.volumeUp();
      break;
    // -----------------------------------
    // Equalizer setting selection.
    case 0xFF22DD:
      // Serial.print("+ Key 4: ");
      // Serial.println("DFPLAYER_EQ_POP");
      mp3player.EQ(DFPLAYER_EQ_POP);
      break;
    case 0xFF02FD:
      // Serial.print("+ Key 5: ");
      // Serial.println("DFPLAYER_EQ_CLASSIC");
      mp3player.EQ(DFPLAYER_EQ_CLASSIC);
      break;
    case 0xFFC23D:
      // Serial.print("+ Key 6: ");
      // Serial.println("DFPLAYER_EQ_NORMAL");
      mp3player.EQ(DFPLAYER_EQ_NORMAL);
      break;
    case 0xFFE01F:
      // Serial.print("+ Key 7: ");
      // Serial.println("DFPLAYER_EQ_JAZZ");
      mp3player.EQ(DFPLAYER_EQ_JAZZ);
      break;
    case 0xFFA857:
      // Serial.print("+ Key 8: ");
      // Serial.println("DFPLAYER_EQ_ROCK");
      mp3player.EQ(DFPLAYER_EQ_ROCK);
      break;
    case 0xFF906F:
      // Serial.print("+ Key 9: ");
      // Serial.println("DFPLAYER_EQ_BASS");
      mp3player.EQ(DFPLAYER_EQ_BASS);
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      Serial.println(results.value, HEX);
      // -----------------------------------
  } // end switch
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); // 9600 or 115200
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ----------------------------------------------------
  // DFPlayer serial connection.
  //
  // ------------------------------------
  // Since Mega has its own hardware RX and TX pins, 
  //    use pins 18 and 19, which has the label: Serial1.
  // Pin 18(TX) to resister to pin 2(RX).
  // Pin 19(RX) to pin 3(TX).
  Serial1.begin(9600);
  if (!mp3player.begin(Serial1)) {
  // --------
  // For communicating from a Nano or Uno with the DFPlayer, use a software serial port.
  // playerSerial.begin(9600);
  // if (!mp3player.begin(playerSerial)) {
    // --------
    // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  // ----------------------------------------------------
  // Initial player settings.
  //
  // Set player front panel values.
  playerCounter = 1;                // For now, default to song/file 1.
  playerVolume = 16;
  playerStatus = OUT_ON | HLTA_ON;  // OUT_ON  LED status light to indicate the Player.
  playPause = true;                 // HLTA_ON implies that the player is Paused.
  //
  mp3player.setTimeOut(60);        // Set serial communications time out
  delay(300);
  mp3player.volume(playerVolume);   // Set speaker volume from 0 to 30. Doesn't effect DAC output.
  //
  // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
  mp3player.outputDevice(DFPLAYER_DEVICE_SD);
  //
  // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
  mp3player.EQ(DFPLAYER_EQ_CLASSIC);
  //
  delay(300);
  playerCounterTop = mp3player.readFileCounts();
  mp3player.pause();
  Serial.print(F("+ DFPlayer is initialized. Number of MP3 files = "));
  Serial.println(playerCounterTop);

  // ---------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized the infrared receiver."));

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
void loop() {
    delay(60);
    if (!(playerStatus & HLTA_ON)) {
      playMp3();
    }
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    playerInfraredSwitch();
    irrecv.resume();
  }
}
// -----------------------------------------------------------------------------
