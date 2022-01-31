// -----------------------------------------------------------------------
/*
  This is a MP3 Player program program.
  Functionality:
    start or stop the playing of an MP3 file (a song),
    play next or previous MP3 file,
    play next or previous directory of files,
    loop an MP3,
    volume up or down,
    and select an equilizer option.

  This program offers external calls for Arduino INO programs to control the MP3 player:
  + setupMp3Player()    Setup process to initialize the hardware and program settings.
  + mp3PlayerRun()      Interactive: A process while loop for command line control of the player.
  + playerContinuous()  The process to have the player continuously play MP3 files.

  Functional calls:
  + mp3PlayerStart()
  + mp3PlayerPause()
  + mp3playerSinglePlay(byte theFileNumber)
  + mp3PlayerSingleLoop(byte theFileNumber)
  + mp3playerPlaywait(byte theFileNumber)

  DFPlayer - A Mini MP3 Player hardware module
              that is controlled by this program.

  Steps to add the MP3 player program (Mp3Player.cpp) to an Arduino program (*.ino):
  + Include the header file: Mp3Player.h

  -------------------------------------------------------------------------
  Next to implement,

  3 modes:
  + Play all songs
  + Loop play a single song
  + Play a single song and stop
  ++ Change loopSingle to playMode, and handle all 3 play modes.
  + Update Altair101b to use playMode for OUT 10 and OUT 11.

  Need to better handle the following:
  Busy, Card not found
  Card Online!
  + When does it happen? Can it be prevented.
  + If it cannot be prevented, handle it better.

  --------------------
  Have LED playerLights()
  + When ready, display the song number that's playing to LEDs or to an LCD.
  + Consider adding 8 LED lights (0...255) to indicate current file selected for playing.

  --------------------
  Consider Implementing setupMp3Player() within this program,
  + When first playing a song, run: setupMp3Player();

  I added maxDirectory. Consider loop around with directories.

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
  Mega and Due

  Wire connections:
  DFPlayer pin 3 (TX) to a 1K resister. 1K resister to Arduino RX19.
  DFPlayer pin 2 (RX) to a 5K resister. 5K resister to Arduino TX18.
  DFPlayer pin GND    to Arduino GND. Arduino GND to USB hub GND.
  DFPlayer pin VCC    to USB hub VCC.

  Due pins                    TX1:18 RX1:19
   ---------------------------------------
  |                                x x   -| +VCC
  |                                       |
  |                                       | ...
  |                                       |
  |                                      -| -GND
   ---------------------------------------

  USB cable connector to power the DFPlayer.
  Pins from the top:
     ---------------
    | VCC D- D+ GND |
    |   -       _   |
    |  | |     | |  |
    |   _       _   |
    |               |
     ---------------
          |  |
          |  |
          |  |
  My USB cable has:
  + A ground wire wrap.
  + Purple VCC, +5 VCC.s

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
  Connections for Mega and Due:
    RX(2) to resister (1K-5K) to SerialSw pin 18(TX).
    TX(3) to resister (1K-5K) to SerialSw pin 19(RX).

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

  A24 + -   - Mega or Due pin connections
   A1 + -   - Uno or Nano pin connections
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

#include "Altair101.h"
#include "Mp3Player.h"

#ifdef Altair101f
#include "frontPanel.h"
#endif

String playerPrompt = "MP3 ?- ";
extern int programState;

const byte HLTA_OFF =   ~HLTA_ON;
const byte M1_OFF =     ~M1_ON;

// -----------------------------------------------------------------------
// Motherboard Specific setup for DFPlayer communications

// -----------------------------------------------
#if defined(__AVR_ATmega2560__)
// -----------------------------------------------
#elif defined(__SAM3X8E__)
// -----------------------------------------------
#else
// -----------------------------------------------
// Nano or Uno use a software serial port for communications with the DFPlayer.
#include "SoftwareSerial.h"
// DFPlayer pins 3(TX) and 2(RX), connected to Arduino pins: 10(RX) and 11(TX).
// SoftwareSerial playerSerial(10, 11); // Software serial, playerSerial(RX, TX)
SoftwareSerial SerialSw(10, 11); // Software serial, playerSerial(RX, TX)
// ------------------
#endif

// -----------------------------------------------------------------------
// Infrared Receiver

IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------
// DFPlayer Mini MP3 play

// #include "Arduino.h"     // Included in Altair101.h
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3playerDevice;

// ----------------------------

#define PLAYER_VOLUME_SETUP 6

// ------------------------------------
// The following is not needed for Mega because it has it's own hardware RX and TX pins.
//
// For communicating a Nano or Uno with the DFPlayer, use SoftwareSerial:
// #include "SoftwareSerial.h"
// DFPlayer pins 3(TX) and 2(RX), connected to Arduino pins: 10(RX) and 11(TX).
// SoftwareSerial playerSerial(10, 11); // Software serial, playerSerial(RX, TX)
// ------------------------------------


// Front panel display light values:
uint16_t playerCounter = 1;                     // First song played when player starts up. Then incremented when next is played.
uint8_t playerVolume = 0;
//
uint8_t playerStatus = OUT_ON | HLTA_ON;        // Indicates MP3 player and it's paused (halted).
uint16_t thePlayerCounter;

//
uint16_t currentPlayerCounter = playerCounter;  // Current song playing.
boolean processorPlayerLoop = true;             // Indicator for the processor to start playing an MP3 when flipping START.
//
uint16_t playerCounterTop = 0;
uint16_t playerDirectoryTop = 0;
uint8_t playerDirectory = 1;                    // File directory name on the SD card. Example 1 is directory name: /01.
uint8_t playerEq = DFPLAYER_EQ_CLASSIC;         // Default equalizer setting.

uint8_t playMode = PLAY_ALL;                    // Default play all songs and then loop back and start over.
void setPlayMode(uint8_t setTo) {
  playMode = setTo;                             // For external programs to set the value.
}
uint8_t getPlayMode() {
  return playMode;                              // For external programs to get the value.
}

// Sometimes, nice not to hear sounds over and again when testing.
//      NOT_PLAY_SOUND = false >> Do play sounds.
boolean NOT_PLAY_SOUND = false;
//      NOT_PLAY_SOUND = true  >> Don't play sounds.

// ---------------------------
// Sound bites for sound effects
/*
   soundEffects is an array that matches index values to an MP3 file number.
   Example: READ_FILE=1
    where
      The value of soundEffects[1], is stored in file: 0001.sbf
      The value of soundEffects[2], is stored in file: 0002.sbf
      ...
    If the byte stored in 0001.sbf is 5, then,
      soundEffects[READ_FILE]=5 or soundEffects[1]=5.
    Then,
      playerPlaySound(READ_FILE) plays file: 0005.mp3.
*/
int READ_FILE             = 1;
int TIMER_COMPLETE        = 2;
int RESET_COMPLETE        = 3;
int CLOCK_ON              = 4;
int CLOCK_OFF             = 5;
int PLAYER_ON             = 6;
int PLAYER_OFF            = 7;
int KR5                   = 8;
int CLOCK_CUCKOO          = 9;
int TIMER_MINUTE          = 10;     // MP3# 7 or 21
int DOWNLOAD_COMPLETE     = 11;
int WRITE_FILE            = 12;
int KNIGHT_RIDER_SCANNER  = 13;

// const int maxSoundEffects = 16;
//
//                     {0  1   2  3  4  5  6  7  8  9   0  1  2  3  4  5};
int soundEffects[16] = {1, 1, 31, 1, 2, 3, 9, 3, 1, 6, 21, 1, 1, 6, 1, 1};

// -----------------------------------------------------------------------------
// Initialize the player module.
// This allows it to be reset after the computer is restarted.
//
boolean setupMp3Player() {
  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized: MP3 player infrared receiver."));

  // Set player front panel values.
  playerCounter = 1;                  // For now, default to song/file 1.
  playerDirectory = 1;
  playerVolume = PLAYER_VOLUME_SETUP;
  playerEq = DFPLAYER_EQ_CLASSIC;
  playerStatus = OUT_ON | HLTA_ON;    // ,  LED status light to indicate the Player.
  //
  // ---------------------------------------------------------------------------
  // Serial connection depends on the motherboard.
  //
#if defined(__AVR_ATmega2560__) || defined(__SAM3X8E__)
  Serial1.begin(9600);
  if (!mp3playerDevice.begin(Serial1)) {
    delay(500);
    if (!mp3playerDevice.begin(Serial1)) {
      NOT_PLAY_SOUND = true;  // Set to not play sound effects.
      Serial.println(F("MP3 Player, unable to begin:"));
      Serial.println(F("1.Recheck the connection."));
      Serial.println(F("2.Insert the SD card."));
      return false;
    }
  }
#else
  SerialSw.begin(9600);
  if (!mp3playerDevice.begin(SerialSw)) {
    delay(500);
    if (!mp3playerDevice.begin(SerialSw)) {
      NOT_PLAY_SOUND = true;  // Set to not play sound effects.
      Serial.println(F("MP3 Player, unable to begin:"));
      Serial.println(F("1.Recheck the connection."));
      Serial.println(F("2.Insert the SD card."));
      return false;
    }
  }
#endif
  // ---------------------------------------------------------------------------
  mp3playerDevice.setTimeOut(60);               // Set serial communications time out.
  delay(300);
  NOT_PLAY_SOUND = false;                       // Set to play sound effects.
  mp3playerDevice.volume(PLAYER_VOLUME_SETUP);  // Set speaker volume from 0 to 30.
  delay(100);
  //
  // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
  mp3playerDevice.outputDevice(DFPLAYER_DEVICE_SD);
  //
  mp3playerDevice.EQ(playerEq);
  //
  playerCounterTop = mp3playerDevice.readFileCounts();
  if (playerCounterTop > 256) {
    // Re-try getting the count.
    delay(300);
    playerCounterTop = mp3playerDevice.readFileCounts();
  }
  if (playerCounterTop > 256) {
    Serial.print(F("+ Initialized: MP3 player, number of MP3 files is unknown."));
  } else {
    Serial.print(F("+ Initialized: MP3 player, number of MP3 files = "));
    Serial.print(playerCounterTop);
  }
  Serial.println();
  return true;
}

// -----------------------------------------------------------------------
void playerEqSwitch(byte thePlayerEq) {
  Serial.print(thePlayerEq);
  Serial.print(F(", "));
  switch (thePlayerEq) {
    case DFPLAYER_EQ_POP:
      Serial.print(F("+ Key 4: "));
      Serial.println(F("DFPLAYER_EQ_POP"));
      break;
    case DFPLAYER_EQ_CLASSIC:
      Serial.print(F("+ Key 5: "));
      Serial.println(F("DFPLAYER_EQ_CLASSIC"));
      break;
    case DFPLAYER_EQ_NORMAL:
      Serial.print(F("+ Key 6: "));
      Serial.println(F("DFPLAYER_EQ_NORMAL"));
      break;
    case DFPLAYER_EQ_JAZZ:
      Serial.print(F("+ Key 7: "));
      Serial.println(F("DFPLAYER_EQ_JAZZ"));
      break;
    case DFPLAYER_EQ_ROCK:
      Serial.print(F("+ Key 8: "));
      Serial.println(F("DFPLAYER_EQ_ROCK"));
      break;
    case DFPLAYER_EQ_BASS:
      Serial.print(F("+ Key 9: "));
      Serial.println(F("DFPLAYER_EQ_BASS"));
      break;
    default:
      Serial.println(F("+ Other EQ setting"));
  }
}

String stringPlayMode() {
  switch (playMode) {
    case PLAY_ALL:
      return ("Play all songs and then loop back and start over.");
    case PLAY_SINGLE:
      return ("Play a single MP3 and then stop.");
    case LOOP_SINGLE:
      return ("Loop play a single MP3.");
    default:
      return ("- playMode error value: " + playMode);
  }
}

void printPlayerInfo() {
  Serial.println(F("+ --------------------------------------"));
  Serial.println(F("+++ MP3 Player, version 0.97"));
  Serial.println(F("+ Software variable values:"));
  Serial.print(F("++ playerVolume:          "));
  Serial.println(playerVolume);
  Serial.print(F("++ playerCounterTop:      "));
  Serial.println(playerCounterTop);
  Serial.print(F("++ playerCounter:         "));
  Serial.println(playerCounter);
  Serial.print(F("++ playerEq:              "));
  playerEqSwitch(playerEq);
  Serial.print(F("++ playerDirectory#       "));
  Serial.println(playerDirectory);
  Serial.print(F("++ playerDirectoryTop#    "));
  Serial.println(playerDirectoryTop);
  Serial.print(F("++ playMode:              "));
  Serial.println(stringPlayMode());
  Serial.println(F("+ ----------------"));
  // -----------------
  Serial.print(F("++ playerStatus:          "));
  Serial.print(playerStatus);
  Serial.print(F(" = "));
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(playerStatus, i));  // Print as a binary number.
  }
  Serial.println(F(" HLTA is bit 4, ON example: B00001000."));
  // -----------------
  Serial.print(F("++ playerStatus, HLTA:    "));
  if (playerStatus & HLTA_ON) {
    Serial.print(F("HLTA is on - Play is paused."));
  }
  if (!(playerStatus & HLTA_ON)) {
    Serial.print(F("HLTA is Off - Playing, not paused."));
  }
  Serial.println();
  // -----------------
  Serial.println(F("+ -------------------------"));
  Serial.println(F("+ Hardware data values:"));
  if (!(playerStatus & HLTA_ON)) {
    // Serial.println(F("+ Pause the song while getting information values..."));
    mp3playerDevice.pause();
    delay(200);
  }
  Serial.print(F("++ readVolume:            "));
  Serial.println(mp3playerDevice.readVolume());             // current sound volume
  delay(300); // For some reason, readVolume() requires time to get the value, or maybe it's just that the first one requires time.
  Serial.print(F("++ readFileCounts:        "));
  Serial.println(mp3playerDevice.readFileCounts());         // all file counts in SD card
  Serial.print(F("++ readCurrentFileNumber: "));
  Serial.println(mp3playerDevice.readCurrentFileNumber());  // current play file number
  Serial.print(F("++ readEQ:                "));
  playerEqSwitch(mp3playerDevice.readEQ());                 // EQ setting
  // Serial.println();
  //
  // Serial.print(F("++ readFileCountsInFolder 01: "));
  // Serial.println(mp3playerDevice.readFileCountsInFolder(1)); // fill counts in folder SD:/01
  //
  // Since the above resets to the first song, set back to the song that was playing.
  delay(100);
  mp3playerDevice.play(playerCounter);
  delay(100);
  mp3playerDevice.pause();
  if (!(playerStatus & HLTA_ON)) {
    Serial.println(F("+ Restarting the song..."));
    // Note mp3playerDevice.start(); doesn't work, and the above resets the device back to song 1.
    mp3playerDevice.play(playerCounter);
  }
  Serial.println(F("+ --------------------------------------"));
}

void mp3playerPlayCounter(int theCounter) {
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
  playerLights(playerStatus, playerVolume, playerCounter);
}

// -----------------------------------------------------------------------
// Play a specific directory. (Doesn't work)
//
int playDirectory(int theDirectory) {
  int returnDirectory = theDirectory;
  Serial.print(returnDirectory);
  mp3playerDevice.pause();
  delay(200);
  mp3playerDevice.loopFolder(returnDirectory);
  delay(200);
  mp3playerDevice.pause();
  delay(300);
  // ------------------
  // Set the playerCounter to this new directory file number.
  thePlayerCounter = playerCounter;
  playerCounter = mp3playerDevice.readCurrentFileNumber();
  if (playerCounter > 0 && playerCounter < 256) {
    Serial.print(F(" play song"));
    mp3playerDevice.stop();
    delay(100);
    mp3playerPlayCounter(playerCounter);
  } else {
    // Error getting the hardware file number value.
    //  Means that the directory is incremented beyond the last song.
    // Serial.print(F(" playerCounter < 0 or > 256"));
    playerCounter = thePlayerCounter;
    if (returnDirectory > 2) {
      returnDirectory = theDirectory - 2;
      Serial.print(F(", playerCounter="));
      playDirectory(returnDirectory);
    }
  }
  Serial.print(F(", playerCounter="));
  Serial.print(playerCounter);
  Serial.print(F(", returnDirectory="));
  Serial.print(returnDirectory);
  playerLights(playerStatus, playerVolume, playerCounter);
  return ( returnDirectory );
}

// -----------------------------------------------------------------------
// DFPlayer configuration and error messages.

void printDFPlayerMessage(uint8_t type, int value);
void printDFPlayerMessage(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      // Serial.println(F("Time Out!"));
      mp3playerPlayCounter(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      playerLights(playerStatus, playerVolume, playerCounter);
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      // When it comes back online, the volume needs to be reset.
      mp3playerDevice.volume(playerVolume);
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      switch (value) {
        case Busy:
          Serial.println(F("Busy, Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case FileIndexOut:
          // Serial.println(F("File Index Out of Bound"));
          mp3playerDevice.start();
          break;
        case FileMismatch:
          // This happens when setting to a directory that is past the end.
          // Serial.print(F("--- Cannot Find File ---"));
          break;
        default:
          // Serial.print(F("Unknown DFPlayer error message value: "));
          // Serial.println(value);
          break;
      }
      break;
    default:
      Serial.print(F("- Unknown DFPlayer message type: "));
      Serial.print(type);
      Serial.print(F(", value:"));
      Serial.println(value);
      break;
  }
}

// -----------------------------------------------------------------------
// Infrared DFPlayer controls

void playerSwitch(int resultsValue) {
  if ( resultsValue >= '0' && resultsValue <= '7' ) {
    // Data (address toggles)
    // Serial input, not hardware input.
    fpAddressToggleWord = fpAddressToggleWord ^ (1 << (resultsValue - '0'));
    if (VIRTUAL_FRONT_PANEL) {
      printVirtualFrontPanel();
    }
    return;
  }
  if (VIRTUAL_FRONT_PANEL) {
    if (programState == PLAYER_RUN) {
      Serial.print(F("\033[9;1H"));  // Move cursor to below the prompt: line 9, column 1.
      Serial.print(F("\033[J"));     // From cursor down, clear the screen.
    }
  }
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
    case 13:
      // CR/LF.
      // Serial.println();
      break;
    // ----------------------------------------------------------------------
    // Song control
    //
    // -----------------------------------
    case 0xFF10EF :
    case 0x7E23117B:    // Key REW
    case 'p' :
      if (playerCounter > 1) {
        playerCounter--;
      } else {
        playerCounter = playerCounterTop;
      }
      playCounterHlta();
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Player, Previous: play previous MP3, playerCounter="));
        Serial.print(playerCounter);
      }
      break;
    // -----------------------------------
    case 0xFF5AA5:
    case 0x7538143B:    // Key FF
    case 'n':
      if (playerCounter < playerCounterTop) {
        playerCounter++;
      } else {
        playerCounter = 1;
      }
      playCounterHlta();
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Player, Next: play next MP3, playerCounter="));
        Serial.print(playerCounter);
      }
      break;
    // -----------------------------------
    case 0xFA2F715F:
    case 's':
    case 'g':
      mp3playerDevice.pause();
      playerStatus = playerStatus | HLTA_ON;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Pause current MP3, playerCounter="));
        Serial.print(playerCounter);
      }
      break;
    case 0x8AA3C35B:    // Key PLAY
    case 'r':
    case 'G':
      {
        // Before starting check if the playerCounter has changed.
        //  If yes, then play the new song.
        currentPlayerCounter = mp3playerDevice.readCurrentFileNumber();
        if (currentPlayerCounter == playerCounter) {
          mp3playerDevice.start();        // Continue playing current song.
        } else {
          if (playerCounter > 0) {
            mp3playerPlayCounter(playerCounter); // Start the new song.
          } else {
            // Error getting the hardware song file number, continue playing current song.
            mp3playerDevice.start();
          }
        }
        playerStatus = playerStatus & HLTA_OFF;
        if (programState == PLAYER_RUN) {
          Serial.print(F("+ Play current song, playerCounter="));
          Serial.print(playerCounter);
          if (playerCounter >= 1) {
            Serial.print(F(", read Current FileNumber: "));
            Serial.print(mp3playerDevice.readCurrentFileNumber());
          }
        }
        break;
      }
    // -----------------------------------
    case 0xFF38C7:
    case 0x82D6EC17:
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key OK|Enter - Toggle: pause|start the MP3 , playerCounter="));
        Serial.print(playerCounter);
        Serial.print(F(", read Current FileNumber: "));
        Serial.print(mp3playerDevice.readCurrentFileNumber());
      }
      if (playerStatus & HLTA_ON) {
        mp3playerDevice.start();
        playerStatus = playerStatus & HLTA_OFF;
      } else {
        mp3playerDevice.pause();
        playerStatus = playerStatus | HLTA_ON;
      }
      break;
    // ----------------------------------------------------------------------
    // Folder, file directory selection.
    //
    // -----------------------------------
    case 0x6D8BBC17:                        // After pressing VCR
    case 0xAD680D1B:                        // After pressing TV
    case 0xFF18E7:                          // Small remote key up.
    case 0xFF629D:                          // Small remote key 2.
    case 'D':
      // + Key up - next directory, directory number: 1 play song, playerCounter=1, playerDirectory=1
      // MP3 ?- + Key up - next directory, directory# 2 play song, playerCounter=34, playerDirectory=2
      // MP3 ?- + Key up - next directory, directory# 3 play song, playerCounter=44, playerDirectory=3
      if (playerDirectoryTop == 0 || (playerDirectory < playerDirectoryTop)) {
        playerDirectory ++;
      }
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key up - next directory, directory# "));
        Serial.print(playerDirectory);
      }
      // ------------------
      mp3playerDevice.pause();
      delay(200);
      mp3playerDevice.loopFolder(playerDirectory);
      mp3playerDevice.pause();
      delay(200);
      //
      // ------------------------------------
      // Set the playerCounter to this new directory file number.
      thePlayerCounter = playerCounter;
      playerCounter = mp3playerDevice.readCurrentFileNumber();
      if (!(playerCounter > 0 && playerCounter < 256)) {
        // Try again. Note, this is required when using sound effects.
        delay(300);
        playerCounter = mp3playerDevice.readCurrentFileNumber();
      }
      if (playerCounter > 0 && playerCounter < 256) {
        if (programState == PLAYER_RUN) {
          Serial.print(F(" play song"));
        }
        mp3playerDevice.stop();
        delay(100);
        mp3playerPlayCounter(playerCounter);
      } else {
        // Past the end.
        // Serial.print(F(" playerCounter < 0 or > 256"));
        playerCounter = thePlayerCounter;
        playerDirectory--;
        playerDirectoryTop = playerDirectory;
        // ------
        delay(200);
        mp3playerDevice.loopFolder(playerDirectory);
        delay(200);
        mp3playerDevice.pause();
        if (programState == PLAYER_RUN) {
          Serial.print(F(" play song"));
        }
        delay(200);
        mp3playerDevice.stop();
        delay(200);
        mp3playerPlayCounter(playerCounter);
        // ------
      }
      // ------------------------------------
      if (programState == PLAYER_RUN) {
        Serial.print(F(", playerCounter="));
        Serial.print(playerCounter);
        Serial.print(F(", playerDirectory="));
        Serial.print(playerDirectory);
      }
      // ------------------
      playerStatus = playerStatus & HLTA_OFF;
      break;
    // -----------------------------------
    case 0xCDFC965B:                        // After pressing VCR
    case 0xDD8E75F:                         // After pressing TV
    case 0xFF4AB5:                          // Small remote key down.
    case 'd':
      if (playerDirectory > 1) {
        if (programState == PLAYER_RUN) {
          Serial.print(F("+ Key down - previous directory, directory number: "));
        }
        playerDirectory --;
      } else {
        if (programState == PLAYER_RUN) {
          Serial.print(F("+ Key down - previous directory. Already at the first directory."));
        }
        break;
      }
      if (programState == PLAYER_RUN) {
        Serial.print(playerDirectory);
      }
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
        if (programState == PLAYER_RUN) {
          Serial.print(F(" play song"));
        }
        mp3playerDevice.stop();
        delay(100);
        mp3playerPlayCounter(playerCounter);
      } else {
        if (programState == PLAYER_RUN) {
          Serial.print(F(" playerCounter < 0 or > 256"));
        }
        playerCounter = thePlayerCounter;
      }
      if (programState == PLAYER_RUN) {
        Serial.print(F(", playerCounter="));
        Serial.print(playerCounter);
        Serial.print(F(", playerDirectory="));
        Serial.print(playerDirectory);
      }
      break;
    // ----------------------------------------------------------------------
    // Loop a single song
    //
    case 0xA02E4EBF:
    case 0xFF6897:
    case 'L':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key *|A.Select - Loop on: loop this single MP3."));
      }
      playMode = LOOP_SINGLE;
      playerStatus = playerStatus | M1_ON;
      if (!LED_LIGHTS_IO) {
        // In not setting front panel LED, pause identifies loop status change.
        if (!(playerStatus & HLTA_ON)) {
          mp3playerDevice.pause();
          delay(200);
          mp3playerDevice.start();
        }
      }
      break;
    case 0xC473DE3A:
    case 0xFFB04F:
    case 'l':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key #|Eject - Loop off: Play all MP3 files."));
      }
      playMode = PLAY_ALL;
      playerStatus = playerStatus & M1_OFF;
      if (!LED_LIGHTS_IO) {
        // In not setting front panel LED, pause identifies loop status change.
        if (!(playerStatus & HLTA_ON)) {
          mp3playerDevice.pause();
          delay(1000);
          mp3playerDevice.start();
        }
      }
      break;
    // -----------------------------------
    // Toshiba VCR remote, not programmed.
    case 0x718E3D1B:                        // Toshiba VCR remote
    case 0xB16A8E1F:                        // After pressing TV
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key 1: "));
      }
      break;
    case 0xF8FB71FB:                        // Toshiba VCR remote
    case 0x38D7C2FF:                        // After pressing TV
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key 2: "));
      }
      break;
    case 0xE9E0AC7F:                        // Toshiba VCR remote
    case 0x29BCFD83:                        // After pressing TV
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key 3: "));
      }
      break;
    //
    // ----------------------------------------------------------------------
    // Equalizer setting selection.
    //
    case 0x38BF129B:                        // Toshiba VCR remote
    case 0x789B639F:                        // After pressing TV
    case 0xFF22DD:                          // Small remote
    case 'P':                               // Over wrote above, for usage on the virtualfront panel.
      playerEq = DFPLAYER_EQ_POP;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key P, infared 4: "));
        Serial.print(F("DFPLAYER_EQ_POP"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    case 0x926C6A9F:                        // Toshiba VCR remote
    case 0xD248BBA3:                        // After pressing TV
    case 0xFF02FD:
    case 'C':                               // Over wrote above, for usage on the virtualfront panel.
      playerEq = DFPLAYER_EQ_CLASSIC;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key  C, infared 5: "));
        Serial.print(F("DFPLAYER_EQ_CLASSIC"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    case 0xE66C5C37:                        // Toshiba VCR remote
    case 0x2648AD3B:                        // After pressing TV
    case 0xFFC23D:
    case 'N':                               // Over wrote above, for usage on the virtualfront panel.
      playerEq = DFPLAYER_EQ_NORMAL;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key  N, infared 6: "));
        Serial.print(F("DFPLAYER_EQ_NORMAL"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    case 0xD75196BB:                        // Toshiba VCR remote
    case 0x172DE7BF:                        // After pressing TV
    case 0xFFE01F:
    case 'J':                               // Over wrote above, for usage on the virtualfront panel.
      playerEq = DFPLAYER_EQ_JAZZ;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key  J, infared 7: "));
        Serial.print(F("DFPLAYER_EQ_JAZZ"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    case 0x72FD3AFB:                        // Toshiba VCR remote
    case 0xB2D98BFF:                        // After pressing TV
    case 0xFFA857:
    case 'K':
      playerEq = DFPLAYER_EQ_ROCK;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key P, infared 8: "));
        Serial.print(F("DFPLAYER_EQ_ROCK"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    case 0xCCAA92FF:                        // Toshiba VCR remote
    case 0xC86E403:                         // After pressing TV
    case 0xFF906F:
    case 'S':
      playerEq = DFPLAYER_EQ_BASS;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key S, infared 9: "));
        Serial.print(F("DFPLAYER_EQ_BASS"));
      }
      mp3playerDevice.EQ(playerEq);
      break;
    // ----------------------------------------------------------------------
    // Volume
    //
    case 0x2B8BE5F:
    case 0xFFE21D:              // Small remote, Key 3
    case 'V':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key Volume ^"));
      }
      // mp3playerDevice.volumeUp();
      if (playerVolume < 30) {
        playerVolume++;
      }
      mp3playerDevice.volume(playerVolume);
      if (programState == PLAYER_RUN) {
        Serial.print(F(" increase volume to "));
        Serial.print(playerVolume);
      }
      break;
    case 0x1CF3ACDB:
    case 0xFFA25D:              // Small remote, Key 1
    case 'v':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Key Volume v"));
      }
      // mp3playerDevice.volumeDown();
      if (playerVolume > 1) {
        playerVolume--;
      }
      mp3playerDevice.volume(playerVolume);
      if (programState == PLAYER_RUN) {
        Serial.print(F(" decrease volume to "));
        Serial.print(playerVolume);
      }
      break;
    case 'x':
      playerCounter = fpAddressToggleWord;
      mp3playerPlayCounter(playerCounter);         // Play the song.
      playerStatus = playerStatus & HLTA_OFF;
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ EXAMINE, play MP3 playerCounter="));
        Serial.print(playerCounter);
      }
      break;
    case 'e':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Set mode to play a single MP3 once."));
      }
      playMode = PLAY_SINGLE;
      break;
    case 'E':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Set mode to playing of all MP3 files."));
      }
      playMode = PLAY_ALL;
      break;
    // ----------------------------------------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ MP3 Player Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ g/s, STOP       Pause, stop playing the MP3."));
      Serial.println(F("+ G/r, Play       Start playing the MP3."));
      Serial.println(F("+ R, RESET/CLEAR  Reset player settings to default, and set to play first MP3."));
      Serial.println(F("+ x, EXAMINE      Play specified MP3 number."));
      Serial.println(F("+ n/p, Play song  Play next/previous MP3."));
      Serial.println(F("+ d/D, Directory  Play next/previous directory."));
      Serial.println(F("+ e/E, Single/All Play only a single once/Play all MP3 files in a loop."));
      Serial.println(F("+ l/L, Loop       Disable/Enable looping of the current MP3."));
      Serial.println(F("+ v/V, Volume     Down/Up volume level."));
      Serial.println(F("+ I, Information  Program variables and hardward values. Restarts the song."));
      Serial.println(F("+ i, Info MP3#    Print current MP3 counter. Current MP3 being played."));
      Serial.println(F("--- Equalizer options:"));
      Serial.println(F("+ 4/P POP   5/C CLASSIC  6/N NORMAL"));
      Serial.println(F("+ 7/K ROCK  8/J JAZZ     9/S BASS"));
      Serial.println(F("------------------"));
      Serial.println(F("+ t/T VT100 panel Disable/enable VT100 virtual front panel."));
      Serial.println(F("+ Ctrl+L          Clear screen."));
      Serial.println(F("+ X, Exit player  Return to program WAIT mode."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // ----------------------------------------------------------------------
    case 0x953EEEBC:                              // Key CLEAR
    case 'R':
      if (programState == PLAYER_RUN) {
        Serial.println(F("+ Player CLEAR/RESET, play first song."));
      }
      mp3playerDevice.stop();                     // Required.
      delay(200);
      setupMp3Player();                           // Sets playerCounter = 1, and other default settings;
      delay(200);
      mp3playerPlayCounter(playerCounter);
      delay(200);
      mp3playerDevice.pause();
      // mp3playerDevice.stop();                  // Test removing this "stop".
      break;
    case 'i':
      if (programState == PLAYER_RUN) {
        Serial.print(F("+ Info, MP3 playerCounter = "));
        Serial.print(playerCounter);
      }
      break;
    case 'I':
      Serial.println(F("+ Information"));
      printPlayerInfo();
      break;
    case 12:
      // Ctrl+L, clear screen.
      Serial.print(F("\033[H\033[2J"));           // Cursor home and clear the screen.
      break;
    case 'X':
      Serial.println(F("+ Exit MP3 PLAYER mode. Return to Processor WAIT mode."));
      programState = PROGRAM_WAIT;
      break;
    // -------------------------------------------------------------------
    case 't':
      Serial.print(F("+ VT100 escapes are disabled and block cursor on."));
      if (VIRTUAL_FRONT_PANEL) {
        VIRTUAL_FRONT_PANEL = false;
        Serial.print(F("\033[0m\033[?25h"));       // Insure block cursor display: on.
      }
      // Note, VT100 escape sequences don't work on the Ardino IDE monitor.
      break;
    case 'T':
      // The following requires a VT100 terminal such as a Macbook terminal.
      initVirtualFrontPanel();
      playerLights(playerStatus, playerVolume, playerCounter);
      Serial.print(F("+ VT100 escapes are enabled and block cursor off."));
      break;
    // -------------------------------------------------------------------
    default:
      // Serial.print(F("+ Result value: "));
      // Serial.print(resultsValue);
      // Serial.print(F(", HEX: "));
      // Serial.println(resultsValue, HEX);
      printPrompt = false;
      break;
      // ----------------------------------------------------------------------
  } // end switch
  if (printPrompt && (programState == PLAYER_RUN)) {
    Serial.println();
    Serial.print(playerPrompt);
    playerLights(playerStatus, playerVolume, playerCounter);
  }
}

// -----------------------------------------------------------------------------
// Handle continuous playing, and play errors such as: SD card not inserted.
//
void playerContinuous() {
  //
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    playerSwitch(results.value);
    irrecv.resume();
  }
  if (playerStatus & HLTA_ON) {
    // HLTA means that the player is halted, paused, not playing.
    //  So, don't play any more.
    return;
  }
  if (mp3playerDevice.available()) {
    //
    int theType = mp3playerDevice.readType();
    // ------------------------------
    if (theType == DFPlayerPlayFinished) {
      // Serial.print(F("+ Play Finished, "));
      if (playMode == LOOP_SINGLE) {
        // Serial.print(F("Loop/play the same MP3"));
        mp3playerDevice.start();
      } else if (playMode == PLAY_ALL) {
        // Serial.print(F("Play next MP3"));
        if (playerCounter < playerCounterTop) {
          playerCounter++;
        } else {
          playerCounter = 1;
        }
        mp3playerPlayCounter(playerCounter);
        //
        // This fixes the issue of skipping 2 songs,
        //  because playMp3() calls this process before the player status changes to busy,
        //  and each time it's called, it increments playerCounter.
        while (mp3playerDevice.available()) {
          mp3playerDevice.readType();
          delay(10);
        }
        //
        playerStatus = playerStatus & HLTA_OFF;
        if (programState == PLAYER_RUN) {
          // This "if", allows continuous playing
          //   in other modes (clock) without effecting their display lights.
          playerLights(playerStatus, playerVolume, playerCounter);
        }
      } else {
        // Playing of a single MP3 is completed.
        playerStatus = playerStatus | HLTA_ON;
      }
      // Serial.print(F(", playerCounter="));
      // Serial.print(playerCounter);
      // Serial.println();
      //
      // ------------------------------
    } else if (theType == DFPlayerCardInserted ) {
      Serial.println(F("+ SD mini card inserted. Start playing"));
      mp3playerDevice.start();
    } else if (theType == 11 ) {
      // This happens when mp3playerDevice.readCurrentFileNumber():
      //                    read Current FileNumber: -1.
      // Serial.print(F("++ read Current FileNumber: "));
      // Serial.print(mp3playerDevice.read());  // mp3playerDevice.readCurrentFileNumber()
      // Serial.println();
    } else {
      // Print the detail message from DFPlayer to handle different errors and states,
      //   such as memory card not inserted.
      printDFPlayerMessage(theType, mp3playerDevice.read());
    }
  }
}

// -----------------------------------------------------------------------------
// Calls from other programs.

void mp3PlayerStart() {
  // if (NOT_PLAY_SOUND) {
  //   return;
  // }
  mp3playerDevice.start();
  playerStatus = playerStatus & HLTA_OFF;
}

void mp3PlayerPause() {
  // Twice because sometimes, once doesn't work.
  mp3playerDevice.pause();
  delay(100);
  mp3playerDevice.pause();
  playerStatus = playerStatus | HLTA_ON;
}

void mp3playerSinglePlay(byte theFileNumber) {
  playMode = PLAY_SINGLE;
  playerStatus = playerStatus & HLTA_OFF;
  mp3playerDevice.play(theFileNumber);
}

void mp3PlayerSingleLoop(byte theFileNumber) {
  playMode = LOOP_SINGLE;
  playerStatus = playerStatus & HLTA_OFF;
  mp3playerDevice.play(theFileNumber);    // playerContinuous() will manage the looping.
}

void playerSoundEffect(byte theFileNumber) {
  if (playerStatus & HLTA_ON) {
    mp3playerDevice.play(soundEffects[theFileNumber]);
  }
}
void playerSoundEffectWait(byte theFileNumber) {
  if (playerStatus & HLTA_ON) {
    mp3playerPlaywait(soundEffects[theFileNumber]);
  }
}

// ---------------------------------------
// For playing short sound effects in a game.
void mp3playerPlayShort(byte theFileNumber) {
  // Serial.print(F("+ Play short sound effects in a game."));
  mp3playerDevice.play(theFileNumber);
  delay(300);
}
void mp3playerPlayShort(byte theFileNumber, uint16_t theTime) {
  // Serial.print(F("+ Play short sound effects in a game."));
  mp3playerDevice.play(theFileNumber);
  delay(theTime);
}
// ---------------------------------------
void mp3playerPlaywait(byte theFileNumber) {
  // Serial.print(F("+ Play MP3 until completed."));
  playerCounter = theFileNumber;
  currentPlayerCounter = playerCounter;
  //
  // Start the MP3 and wait for the player to be available(started).
  mp3playerDevice.play(playerCounter);
  playerStatus = playerStatus & HLTA_OFF;
  while (mp3playerDevice.available()) {
    mp3playerDevice.readType();
    delay(10);
  }
  boolean playing = true;
  while (playing) {
    if (mp3playerDevice.available()) {
      int theType = mp3playerDevice.readType();
      if (theType == DFPlayerPlayFinished) {
        playing = false;
        // Serial.print(F(" > Playing is completed."));
      }
    }
    // ------------------------
    // STOP, to end at anytime.
    //
    if (Serial.available() > 0) {
      // Hit any key to exit.
      // Serial.print(F(" > Ended early."));
      playing = false;
      mp3playerDevice.stop();
    }
#ifdef Altair101f
    if (getPcfControlinterrupted()) {
      // Hardware front panel controls.
      // Switch any switch, and exit.
      playing = false;
      setPcfControlinterrupted(false); // Reset for next interrupt.
    }
#endif
    if (irrecv.decode(&results)) {
      // Hit any infrared key to exit.
      // Serial.print(F(" > Ended early."));
      playing = false;
      mp3playerDevice.stop();
      irrecv.resume();
    }
    // ------------------------
  }
  playerStatus = playerStatus | HLTA_ON;
  // Serial.println();
}

// -----------------------------------------------------------------------------
// MP3 Player controls.

void mp3PlayerRun() {
  if (VIRTUAL_FRONT_PANEL || LED_LIGHTS_IO) {
    playerSwitch('T');
  }
  Serial.println(F("+ runMp3Player();"));
  Serial.print(playerPrompt);
  while (programState == PLAYER_RUN) {
    //
    // Process serial input key presses from a keyboard.
    if (Serial.available() > 0) {
      int readByte = Serial.read();    // Read and process an incoming byte.
      playerSwitch(readByte);
    }
#ifdef Altair101f
    if (getPcfControlinterrupted()) {
      // Hardware front panel controls.
      playerControlSwitches();
      fpCheckAux1();
      if (byte readByte = fpCheckProtectSetVolume()) {
        processWaitSwitch(readByte);
      }
      setPcfControlinterrupted(false); // Reset for next interrupt.
    }
#endif
    playerContinuous();  // For continuous playing. Else, when a song ends, playing would stop.
    //
    delay(60);  // Delay before getting the next key press, in case press and hold too long.
  }
}

// -----------------------------------------------------------------------------
