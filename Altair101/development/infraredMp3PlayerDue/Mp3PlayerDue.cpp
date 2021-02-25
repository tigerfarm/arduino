/*
  -------------------------------------------------------------------------
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.

  First play, "read Current FileNumber: -1"
  ?- + Play, play current song, playerCounter=1, read Current FileNumber: -1

  Every time pause,  "read Current FileNumber: -1"
  + Pause, play current song, playerCounter=3, read Current FileNumber: -1

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

  -------------------------------------------------------------------------
  To compile this version, use the library manager to load the
    DFRobotDFPlayerMini by DFRobot mini player library.
    For my implementation, I loaded version 1.0.5.
    https://github.com/DFRobot/DFRobotDFPlayerMini
  Sample program demoing many options:
    https://github.com/DFRobot/DFRobotDFPlayerMini/issues/20
  -------------------------------------------------------------------------
  DFPlayer specifications with sample programs.
    https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299
    supports FAT16 , FAT32 file system, maximum support 32G
    supported sampling rates (kHz): 8/11.025/12/16/22.05/24/32/44.1/48
    supports up to 100 folders, every folder can hold up to 255 songs
    30 level adjustable volume, 6 -level EQ adjustable
    Serial sending of commands: 9600 bps, Data bits:1, Checkout:none, Flow Control:none

  Sample library commands:
    mp3playerDevice.play(1);           // Play the first mp3
    mp3playerDevice.pause();           // pause the mp3
    mp3playerDevice.start();           // start the mp3 from the pause
    ------------------------------
    mp3playerDevice.next();            // Play next mp3
    mp3playerDevice.previous();        // Play previous mp3
    ------------------------------
    mp3playerDevice.playMp3Folder(4);  // play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
    mp3playerDevice.playFolder(15, 4); // play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
    mp3playerDevice.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
    ------------------------------
    mp3playerDevice.loop(1);           // Loop the first mp3
    mp3playerDevice.enableLoop();      // enable loop.
    mp3playerDevice.disableLoop();     // disable loop.
    mp3playerDevice.loopFolder(5);     // loop all mp3 files in folder SD:/05.
    mp3playerDevice.enableLoopAll();   // loop all mp3 files.
    mp3playerDevice.disableLoopAll();  // stop loop all mp3 files.
    ------------------------------
    mp3playerDevice.volume(10);        // Set volume value. From 0 to 30
    mp3playerDevice.volumeUp();        // Volume Up
    mp3playerDevice.volumeDown();      // Volume Down
    ------------------------------
    mp3playerDevice.setTimeOut(500);   //Set serial communictaion time out 500ms
    mp3playerDevice.reset();           //Reset the module
    ------------------------------
    Serial.println(mp3playerDevice.readState());               //read mp3 state
    Serial.println(mp3playerDevice.readVolume());              //read current volume
    Serial.println(mp3playerDevice.readEQ());                  //read EQ setting
    Serial.println(mp3playerDevice.readFileCounts());          //read all file counts in SD card
    Serial.println(mp3playerDevice.readCurrentFileNumber());   //read current play file number
    Serial.println(mp3playerDevice.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
    ------------------------------
    mp3playerDevice.available()
    printDetail(mp3playerDevice.readType(), mp3playerDevice.read()); //Print the detail message from DFPlayer to handle different errors and states.
    ------------------------------

  Filenames and folder directory names:
    The default folder name needs to be mp3, placed under the SD card root directory: SD:/MP3.
    Other Folder Names: 01 ... 99.
    The mp3 file name needs to be 4 digits, for example, "0001.mp3", placed under the mp3 folder.
    File Names: 0001.mp3 to 0255.mp3. Or, 001.mp3 to 255.mp3?
    Can add characters after the number, for example, "0001hello.mp3".

  Prepare an SD card for use.

  On Mac, use the disk utility to format the disk:
    Applications > Utilities > open Disk Utility.
    Click on the SD card, example: APPLE SD Card Reader Media/MUSICSD.
    Click menu item, Erase.
    Set name, example: MUSICSD.
    Select: MS-DOS (Fat).
    Click Erase. The disk is cleaned and formated.

  List to find the card.
    $ diskutil list
    /dev/disk3 (internal, physical):
      #:                       TYPE NAME                    SIZE       IDENTIFIER
      0:     FDisk_partition_scheme                        *4.0 GB     disk3
      1:                 DOS_FAT_32 MUSICSD                 4.0 GB     disk3s1
    $ ls /Volumes/MUSICSD
  Copy files in order onto the SD card.
    The DFPlayer seems to use some sort of creation timestamp when the files are index.
    So don’t copy 0003.mp3 and then 0001.mp3, otherwise wacky things will happen.
    $ ls /Volumes/MUSICSD
    01  02
  Clean hidden files which can cause issues: https://ss64.com/osx/dot_clean.html
    $ dot_clean /Volumes/MUSICSD
  Format,
    $ sudo diskutil eraseDisk FAT32 MUSICSD MBRFormat /dev/disk3

  ------------------------------------------------------------------------------
  DFPlayer Mini pins
         --------------
    VCC |01   |  |   16| BUSY, low:playing, high:not playing
     RX |02    __    15| USB port - (DM, clock)
     TX |03 DFPlayer 14| USB port + (DP, data)
  DAC_R |04          13| ADKEY_2 Play fifth segment.
  DAC_L |05  ------  12| ADKEY_1 Play first segment.
  SPK - |06 |      | 11| IO_2 short press, play next. Long press, increase volume.
    GND |07 |      | 10| GND
  SPK + |08 Micro SD 09| IO_1 short press, play previous. Long press, decrease volume.
         --------------

  Can try:
    "SPK -" to speaker #1 +
    "SPK +" to speaker #2 +
    GND to ground of speaker #1 and speaker #2.

   3.5mm headphone jack pin out:
   + Tip: left channel
   + Middle: right channel
   + Closest to the cable: ground.

  ---------------------------------
  Connections used with an Arduino,

  1. UART serial,
    RX for receiving control instructions the DFPlayer.
    RX: input connects to TX on Mega/Nano/Uno.
    TX for sending state information.
    TX: output connects to RX on Mega/Nano/Uno.
  Connections for Nano or Uno:
    RX(2) to resister (1K-5K) to serial software pin 11(TX).
    TX(3) to serial software pin 10(RX).
  Connections for Mega:
    RX(2) to resister (1K-5K) to Serial1 pin 18(TX).
    TX(3) to Serial1 pin 19(RX).

  2. Power options.
  Connect from the Arduino directly to the DFPlayer:
    VCC to +5V. Note, also works with +3.3V in the case of an NodeMCU.
    GND to ground(-).
  Or, to eliminate a static noise that I had, I did the following.
  2.1 I use a completely different power source, a separate USB wall plug:
    VCC to +5V of the other power source.
    Connected GND to ground(-), between the Arduino power and the USB wall plug.
  2.2 I connect a short wire between the DFPlayer ground pins: pins 7 to 10.

  I seen another power option:
    From the Arduino +5V, use a 7805 with capacitors and diode to the DFPlayer VCC pin.
    GND to ground(-).

  3. Speaker output.
  For a single speaker, less than 3W:
    SPK - to the speaker pin.
    SPK + to the other speaker pin.
  For output to a stearo amp or ear phones:
    DAC_R to output right (+)
    DAC_L to output left  (+)
    GND   to output ground.

  ------------------------------------------------------------------------------
  Infrared receiver pins

   A1 + -   - Arduino pin connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------

*/
// -----------------------------------------------------------------------

#include "Altair101a.h"

extern int programState;

const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte HLTA_OFF =   ~HLTA_ON;

void playerLights() {}
void ledFlashError() {}
void ledFlashSuccess() {}

// -----------------------------------------------------------------------
// Infrared Receiver

#include <IRremote2.h>      // For Arduino Due, change from IRremote to IRremote2.

// Infrared receiver

// Digital and analog pins work. Also tested with A0 and 9.
int IR_PIN = 24;            // For Arduino Due, change from A1 to 24.

IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------
// DFPlayer Mini MP3 play

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3playerDevice;

#define PLAYER_VOLUME_SETUP 10

byte hwStatus = B11111111;            // Initial state.

// ------------------------------------
// The following is not needed for Mega because it has it's own hardware RX and TX pins.
//
// For communicating a Nano or Uno with the DFPlayer, use SoftwareSerial:
// #include "SoftwareSerial.h"
// DFPlayer pins 3(TX) and 2(RX), connected to Arduino pins: 10(RX) and 11(TX).
// SoftwareSerial playerSerial(10, 11); // Software serial, playerSerial(RX, TX)
// ------------------------------------

// Old values, remove after implemtation.
boolean playPause = false;  // For toggling pause.

// Front panel display light values:
uint16_t playerCounter = 1;                     // First song played when player starts up. Then incremented when next is played.
uint8_t playerStatus = 0;
uint8_t playerVolume = 0;

uint16_t thePlayerCounter;

//
uint16_t currentPlayerCounter = playerCounter;  // Current song playing.
uint16_t processorPlayerCounter = 0;            // Indicator for the processor to play an MP3, if not zero.
boolean processorPlayerLoop = true;             // Indicator for the processor to start playing an MP3 when flipping START.
//
uint16_t playerCounterTop = 0;
uint16_t playerDirectoryTop = 0;
uint8_t playerDirectory = 1;      // File directory name on the SD card. Example 1 is directory name: /01.
boolean loopSingle = false;       // For toggling single song.

boolean NOT_PLAY_SOUND = false;  // Sometimes, nice not to hear sounds over and again when testing.

// -----------------------------------------------------------------------------
// Initialize the player module.
// This allows it to be reset after the computer is restarted.
//
void setupMp3Player() {
  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized the infrared receiver."));

  // Set player front panel values.
  playerCounter = 1;                  // For now, default to song/file 1.
  playerVolume = PLAYER_VOLUME_SETUP;
  playerStatus = OUT_ON | HLTA_ON;    // ,  LED status light to indicate the Player.
  playerDirectory = 1;
  //
  // Until changed to playerStatus, manage playPause.
  playPause = true;
  //
  // -------------------------
  Serial1.begin(9600);
  if (hwStatus > 0) {
    hwStatus = 0;
    if (!mp3playerDevice.begin(Serial1)) {
      delay(500);
      if (!mp3playerDevice.begin(Serial1)) {
        ledFlashError();
        NOT_PLAY_SOUND = true;  // Set to not play sound effects.
        Serial.println(F("MP3 Player, unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        hwStatus = 4;
      }
    }
  }
  if (hwStatus == 0) {
    ledFlashSuccess();
    NOT_PLAY_SOUND = false;                 // Set to play sound effects.
    mp3playerDevice.volume(PLAYER_VOLUME_SETUP);  // Set speaker volume from 0 to 30.
    mp3playerDevice.setTimeOut(60);               // Set serial communications time out.
    //
    // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
    mp3playerDevice.outputDevice(DFPLAYER_DEVICE_SD);
    //
    // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
    mp3playerDevice.EQ(DFPLAYER_EQ_CLASSIC);
    //
    playerCounterTop = mp3playerDevice.readFileCounts();
    if (playerCounterTop == 65535) {
      delay(300);
      playerCounterTop = mp3playerDevice.readFileCounts();
    }
    Serial.print(F("+ DFPlayer is initialized. Number of MP3 files = "));
    Serial.println(playerCounterTop);
  }
}

// -----------------------------------------------------------------------
void printPlayerInfo() {
  Serial.println("+ --------------------------------------");
  Serial.print("++ playerCounterTop:   ");
  Serial.println(playerCounterTop);
  Serial.print("++ playerCounter:      ");
  Serial.println(playerCounter);
  Serial.print("++ playerVolume:       ");
  Serial.println(playerVolume);
  Serial.print("++ playPause:          ");
  if (playPause) {
    Serial.println("True");
  } else {
    Serial.println("False");
  }
  Serial.print("++ playerStatus:       ");
  Serial.println(playerStatus);

  Serial.println("+ --------------------------------------");
  if (!playPause) {
    Serial.println("+ Pause the song while getting information values...");
    mp3playerDevice.pause();
  }
  Serial.print("++ readCurrentFileNumber: ");
  Serial.println(mp3playerDevice.readCurrentFileNumber());   // current play file number
  // Serial.print("++ readState: ");
  // Serial.println(mp3playerDevice.readState());               // mp3 state
  Serial.print("++ readEQ: ");
  Serial.println(mp3playerDevice.readEQ());                  // EQ setting
  Serial.print("++ readFileCounts: ");
  Serial.println(mp3playerDevice.readFileCounts());          // all file counts in SD card
  // Serial.print("++ readFileCountsInFolder 01: ");
  // Serial.println(mp3playerDevice.readFileCountsInFolder(1)); // fill counts in folder SD:/01
  Serial.print("++ readVolume: ");
  Serial.println(mp3playerDevice.readVolume());              // current sound volume
  //
  // Since the above resets to the first song, reset the song that was playing.
  delay(100);
  mp3playerDevice.play(playerCounter);
  // delay(100);
  mp3playerDevice.pause();
  if (!playPause) {
    Serial.println("+ Restarting the song...");
    // Note mp3playerDevice.start(); doesn't work, and the above resets the device back to song 1.
    mp3playerDevice.play(playerCounter);
  }
  Serial.println("+ --------------------------------------");
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

void mp3playerPlay(int theCounter) {
  if (NOT_PLAY_SOUND) {
    return;
  }
  currentPlayerCounter = theCounter;
  mp3playerDevice.play(theCounter);
}

void playCounterHlta() {
  if (!(playerStatus & HLTA_ON)) {
    mp3playerDevice.play(playerCounter);
    currentPlayerCounter = playerCounter;
  }
  playerLights();
}

// -----------------------------------------------------------------------
// DFPlayer configuration and error messages.

void printDFPlayerMessage(uint8_t type, int value);
void printDFPlayerMessage(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      // Serial.println(F("Time Out!"));
      mp3playerPlay(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      playerLights();
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
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case FileIndexOut:
          // Serial.println(F("File Index Out of Bound"));
          mp3playerDevice.start();
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          Serial.print("+ Assume the directory number was incremented too high, play first directory: ");
          playerDirectory = 1;
          Serial.println(playerDirectory);
          mp3playerDevice.loopFolder(playerDirectory);
          break;
        default:
          Serial.print(F("Unknown DFPlayer error message value:"));
          Serial.println(value);
          break;
      }
      break;
    default:
      if (type == 11) {
        // This happens when: read Current FileNumber: -1.
        Serial.print(F("++ read Current FileNumber: "));
        Serial.println(value);  // mp3playerDevice.readCurrentFileNumber()
      } else {
        Serial.print(F("- Unknown DFPlayer message type: "));
        Serial.print(type);
        Serial.print(F(", value:"));
        Serial.println(value);
      }
      break;
  }
}

// -----------------------------------------------------------------------
// Infrared DFPlayer controls

void playerSwitch(int resultsValue) {
  switch (resultsValue) {
    // -----------------------------------
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    case 10:
    case 13:
      // CR/LF.
      Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Song control
    //
    // -----------------------------------
    case 0xFF10EF:
    case 0x7E23117B:
    case 'p':
      // Serial.println("+ Key < - previous");
      // mp3playerDevice.previous();
      // delay(300);
      if (playerCounter > 1) {
        playerCounter--;
      } else {
        playerCounter = playerCounterTop;
      }
      playCounterHlta();
      Serial.print(F("+ Player, Previous: play previous song, playerCounter="));
      Serial.print(playerCounter);
      // Serial.print(F(", read Current FileNumber: "));
      // Serial.println(mp3playerDevice.readCurrentFileNumber());
      Serial.println();
      break;
    // -----------------------------------
    case 0xFF5AA5:
    case 0x7538143B:
    case 'n':
      // Serial.println("+ Key > - next");
      // mp3playerDevice.next();
      // delay(300);
      if (playerCounter < playerCounterTop) {
        playerCounter++;
      } else {
        playerCounter = 1;
      }
      playCounterHlta();
      Serial.print(F("+ Player, Next: play next song, playerCounter="));
      Serial.print(playerCounter);
      // Serial.print(F(", read Current FileNumber: "));
      // Serial.println(mp3playerDevice.readCurrentFileNumber());
      Serial.println();
      break;
    // -----------------------------------
    case 0x2C22119B:
    case 's':
      mp3playerDevice.pause();
      playPause = true;
      Serial.print(F("+ Pause, play current song, playerCounter="));
      Serial.println(playerCounter);
      break;
    case 'r':
      mp3playerDevice.start();
      playPause = false;
      Serial.print(F("+ Play, play current song, playerCounter="));
      Serial.print(playerCounter);
      Serial.print(F(", read Current FileNumber: "));
      Serial.print(mp3playerDevice.readCurrentFileNumber());
      Serial.println();
      break;
    // -----------------------------------
    case 0xFF38C7:
    case 0x82D6EC17:
      Serial.print("+ Key OK|Enter - Toggle: pause|start the song, playerCounter=");
      Serial.print(playerCounter);
      Serial.print(F(", read Current FileNumber: "));
      Serial.println(mp3playerDevice.readCurrentFileNumber());
      if (playPause) {
        mp3playerDevice.start();
        playPause = false;
      } else {
        mp3playerDevice.pause();
        playPause = true;
      }
      break;
    // ----------------------------------------------------------------------
    // Folder, file directory selection.
    //
    // -----------------------------------
    case 0x6D8BBC17:
    case 0x8AA3C35B:
    case 'D':
      Serial.print("+ Key up - next directory, directory number: ");
      playerDirectory ++;
      Serial.print(playerDirectory);
      // ------------------
      mp3playerDevice.pause();
      delay(200);
      mp3playerDevice.loopFolder(playerDirectory);
      mp3playerDevice.pause();
      delay(200);
      // ------------------
      // Set the playerCounter to this new directory file number.
      thePlayerCounter = playerCounter;
      playerCounter = mp3playerDevice.readCurrentFileNumber();
      if (playerCounter > 0 && playerCounter < 256) {
        Serial.print(F(" play song"));
        mp3playerDevice.stop();
        delay(100);
        mp3playerPlay(playerCounter);
      } else {
        Serial.print(F(" playerCounter < 0 or > 256"));
        playerCounter = thePlayerCounter;
        playerDirectory--;
      }
      Serial.print(", playerCounter=");
      Serial.print(playerCounter);
      // Serial.print(F(", read Current FileNumber: "));
      // Serial.println(mp3playerDevice.readCurrentFileNumber());
      Serial.println();
      // ------------------
      break;
    // -----------------------------------
    case 0xCDFC965B:
    case 0xFA2F715F:
    case 'd':
    case 0xFF4AB5:
      Serial.print("+ Key down - previous directory, directory number: ");
      if (playerDirectory > 1) {
        playerDirectory --;
      }
      Serial.print(playerDirectory);
      mp3playerDevice.pause();
      delay(200);
      mp3playerDevice.loopFolder(playerDirectory);
      // ------------------
      mp3playerDevice.pause();
      delay(200);
      // Set the playerCounter to this new directory file number.
      thePlayerCounter = playerCounter;
      playerCounter = mp3playerDevice.readCurrentFileNumber();
      if (playerCounter > 0 && playerCounter < 256) {
        Serial.print(F(" play song"));
        mp3playerDevice.stop();
        delay(100);
        mp3playerPlay(playerCounter);
      } else {
        Serial.print(F(" playerCounter < 0 or > 256"));
        playerCounter = thePlayerCounter;
      }
      Serial.print(", playerCounter=");
      Serial.print(playerCounter);
      // Serial.print(F(", read Current FileNumber: "));
      // Serial.print(mp3playerDevice.readCurrentFileNumber());
      Serial.println();
      // ------------------
      break;
    // ----------------------------------------------------------------------
    // Loop a single song
    //
    case 0xA02E4EBF:
    case 0xFF6897:
    case 'L':
      Serial.println("+ Key *|A.Select - Loop on: loop this single MP3.");
      if (!loopSingle) {
        mp3playerDevice.pause();   // Pause identifies that loop is on. Else I need a LED to indicate loop is on.
        delay(200);
        mp3playerDevice.start();
        loopSingle = true;
      }
      break;
    case 0xC473DE3A:
    case 0xFFB04F:
    case 'l':
      Serial.println("+ Key #|Eject - Loop off: Single MP3 loop is off.");
      if (loopSingle) {
        mp3playerDevice.pause(); // Pause identifies that loop is now off. Else I need a LED to indicate loop is on.
        delay(1000);
        mp3playerDevice.start();
        loopSingle = false;
      }
      break;
    // ----------------------------------------------------------------------
    // Small remote, only.
    case 0xFF9867:
      Serial.println("+ Key 0 - Pause");
      mp3playerDevice.pause();
      playPause = true;
      break;
    case 0xFFA25D:
      Serial.print("+ Key 1: ");
      Serial.println("Volume Down");
      mp3playerDevice.volumeDown();
      break;
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("File directory 2");
      playerDirectory = 2;
      mp3playerDevice.loopFolder(playerDirectory);
      break;
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("Volume UP");
      mp3playerDevice.volumeUp();
      break;
    // -----------------------------------
    // Toshiba VCR remote
    case 0x1163EEDF:
      Serial.println("+ Key 0");
      break;
    case 0x718E3D1B:
      Serial.println("+ Key 1");
      break;
    case 0xF8FB71FB:
      Serial.println("+ Key 2");
      break;
    case 0xE9E0AC7F:
      Serial.println("+ Key 3");
      break;
    // ----------------------------------------------------------------------
    // Equalizer setting selection.
    case 0x38BF129B:
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("DFPLAYER_EQ_POP");
      mp3playerDevice.EQ(DFPLAYER_EQ_POP);
      break;
    case 0x926C6A9F:
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("DFPLAYER_EQ_CLASSIC");
      mp3playerDevice.EQ(DFPLAYER_EQ_CLASSIC);
      break;
    case 0xE66C5C37:
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("DFPLAYER_EQ_NORMAL");
      mp3playerDevice.EQ(DFPLAYER_EQ_NORMAL);
      break;
    case 0xD75196BB:
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("DFPLAYER_EQ_JAZZ");
      mp3playerDevice.EQ(DFPLAYER_EQ_JAZZ);
      break;
    case 0x72FD3AFB:
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("DFPLAYER_EQ_ROCK");
      mp3playerDevice.EQ(DFPLAYER_EQ_ROCK);
      break;
    case 0xCCAA92FF:
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      Serial.println("DFPLAYER_EQ_BASS");
      mp3playerDevice.EQ(DFPLAYER_EQ_BASS);
      break;
    // ----------------------------------------------------------------------
    case 0x2B8BE5F:
    case 'V':
      Serial.println("+ Key Volume ^");
      // mp3playerDevice.volumeUp();
      if (playerVolume < 30) {
        playerVolume++;
      }
      mp3playerDevice.volume(playerVolume);
      Serial.print(F("+ Player, increase volume to "));
      Serial.println(playerVolume);
      // Set: playerLights();
      break;
    case 0x1CF3ACDB:
    case 'v':
      Serial.println("+ Key Volume v");
      // mp3playerDevice.volumeDown();
      if (playerVolume > 1) {
        playerVolume--;
      }
      mp3playerDevice.volume(playerVolume);
      Serial.print(F("+ Player, decrease volume to "));
      Serial.println(playerVolume);
      // Set: playerLights();
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ MP3 Player Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ s, STOP         Pause, stop playing."));
      Serial.println(F("+ r, RUN          Start, playing the current song."));
      Serial.println(F("+ R, RESET        Play first song."));
      Serial.println(F("+ n/p, Play song  Play next/previous song."));
      Serial.println(F("+ d/D, Directory  Play previous directory."));
      Serial.println(F("+ l/L, Loop       Disable/Enable loop the current song."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("------------------"));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("------------------"));
      Serial.println(F("+ i, Information  Not working."));
      Serial.println(F("+ x, EXAMINE      Not implemented. Play specified song number."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 'R':
      Serial.println("+ Player RESET, play first song.");
      mp3playerDevice.stop();   // Required.
      setupMp3Player();   // Sets playerCounter = 1;
      mp3playerPlay(playerCounter);
      mp3playerDevice.stop();
      playerLights();
      break;
    case 'i':
      Serial.println("+ Information");
      printPlayerInfo();
      break;
    case 0xDA529B37:
    case 'X':
      Serial.println("+ Power");
      programState = PROGRAM_WAIT;
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      Serial.print(resultsValue);
      Serial.print(", HEX: ");
      Serial.println(resultsValue, HEX);
      // ----------------------------------------------------------------------
  } // end switch
}

// -----------------------------------------------------------------------------
// Handle continuous playing, and play errors such as: SD card not inserted.
//
void playMp3() {
  if (mp3playerDevice.available()) {
    int theType = mp3playerDevice.readType();
    // ------------------------------
    /*
      + Play Finished, Play next MP3.11
      playerCounter=11, read Current FileNumber: -1
      - Unknown DFPlayer message type: 11, value:11
    */
    if (theType == DFPlayerPlayFinished) {
      Serial.print(F("+ Play Finished, "));
      if (loopSingle) {
        Serial.print("Loop/play the same MP3.");
        mp3playerDevice.start();
      } else {
        Serial.print("Play next MP3.");
        if (playerCounter < playerCounterTop) {
          playerCounter++;
        } else {
          playerCounter = 1;
        }
        mp3playerPlay(playerCounter);
        playerStatus = playerStatus & HLTA_OFF;
        if (programState == PLAYER_RUN) {
          // This "if", allows continuous playing
          //   in other modes (clock) without effecting their dislay lights.
          playerLights();
        }
        Serial.println(playerCounter);
      }
      Serial.print(F(" playerCounter="));
      Serial.print(playerCounter);
      Serial.print(F(", read Current FileNumber: "));
      Serial.println(mp3playerDevice.readCurrentFileNumber());
      // ------------------------------
    } else if (theType == DFPlayerCardInserted ) {
      Serial.println(F("+ SD mini card inserted. Start playing"));
      mp3playerDevice.start();
    } else {
      // Print the detail message from DFPlayer to handle different errors and states,
      //   such as memory card not inserted.
      printDFPlayerMessage(theType, mp3playerDevice.read());
    }
  }
}

// -----------------------------------------------------------------------------
// MP3 Player controlls.

void mp3PlayerPause() {
  // Twice because sometimes, once doesn't work.
  mp3playerDevice.pause();
  delay(100);
  mp3playerDevice.pause();
  playPause = true;
  playerStatus = playerStatus | HLTA_ON;
}

void mp3PlayerRun() {
  Serial.println(F("+ runMp3Player();"));
  Serial.print(F("?- "));
  while (programState == PLAYER_RUN) {
    //
    // Process infrared key presses.
    if (irrecv.decode(&results)) {
      playerSwitch(results.value);
      irrecv.resume();
    }
    // Process serial input, such as key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      playerSwitch(readByte);
      Serial.print(F("?- "));
    }
    playMp3();
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
