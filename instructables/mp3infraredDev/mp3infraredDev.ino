/*
  -------------------------------------------------------------------------
  DFPlayer - A Mini MP3 Player For Arduino
  MP3 player with: play next, previous, loop single, and pause.

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

  -------------------------------------------------------------------------
  DFPlayer specifications with sample programs.
    https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299
    supports FAT16 , FAT32 file system, maximum support 32G
    supported sampling rates (kHz): 8/11.025/12/16/22.05/24/32/44.1/48
    supports up to 100 folders, every folder can hold up to 255 songs
    30 level adjustable volume, 6 -level EQ adjustable
    Serial sending of commands: 9600 bps, Data bits:1, Checkout:none, Flow Control:none

  Sample library commands:
    myDFPlayer.play(1);           // Play the first mp3
    myDFPlayer.pause();           // pause the mp3
    myDFPlayer.start();           // start the mp3 from the pause
    ------------------------------
    myDFPlayer.next();            // Play next mp3
    myDFPlayer.previous();        // Play previous mp3
    ------------------------------
    myDFPlayer.playMp3Folder(4);  // play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
    myDFPlayer.playFolder(15, 4); // play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
    myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
    ------------------------------
    myDFPlayer.loop(1);           // Loop the first mp3
    myDFPlayer.enableLoop();      // enable loop.
    myDFPlayer.disableLoop();     // disable loop.
    myDFPlayer.loopFolder(5);     // loop all mp3 files in folder SD:/05.
    myDFPlayer.enableLoopAll();   // loop all mp3 files.
    myDFPlayer.disableLoopAll();  // stop loop all mp3 files.
    ------------------------------
    myDFPlayer.volume(10);        // Set volume value. From 0 to 30
    myDFPlayer.volumeUp();        // Volume Up
    myDFPlayer.volumeDown();      // Volume Down
    ------------------------------
    myDFPlayer.setTimeOut(500);   //Set serial communictaion time out 500ms
    myDFPlayer.reset();           //Reset the module
    ------------------------------
    Serial.println(myDFPlayer.readState());               //read mp3 state
    Serial.println(myDFPlayer.readVolume());              //read current volume
    Serial.println(myDFPlayer.readEQ());                  //read EQ setting
    Serial.println(myDFPlayer.readFileCounts());          //read all file counts in SD card
    Serial.println(myDFPlayer.readCurrentFileNumber());   //read current play file number
    Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
    ------------------------------
    myDFPlayer.available()
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
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

  ------------------------------------------------------------------------------
  DFPlayer Mini pins
         ----------
    VCC |   |  |   | BUSY, low:playing, high:not playing
     RX |    __    | USB port - (DM, clock)
     TX | DFPlayer | USB port + (DP, data)
  DAC_R |          | ADKEY_2 Play fifth segment.
  DAC_L |  ------  | ADKEY_1 Play first segment.
  SPK - | |      | | IO_2 short press, play next. Long press, increase volume.
    GND | |      | | GND
  SPK + | Micro SD | IO_1 short press, play previous. Long press, decrease volume.
         ----------

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
  Use a completely different power source:
    VCC to +5V of the other power source.
    GND to ground(-) of the other power source.
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

   A1 + -   - Nano connections
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
      Serial.print(F("+ Play Finished, Current FileNumber: "));
      Serial.println(mp3player.readCurrentFileNumber());
      if (loopSingle) {
        Serial.println("Loop/play the same MP3.");
        mp3player.start();
        // Serial.println("+ mp3player.read() " + mp3player.read());
      } else {
        Serial.println("Play next MP3.");
        delay(300);
        mp3player.next();
      }
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

// Play track#, with retry.
//    This may fix my issue where it skips to the next track until it finds a file that plays.
//    From: https://reprage.com/post/dfplayer-mini-cheat-sheet
void playTrack(uint8_t track) {
  mp3player.stop();
  delay(200);
  mp3player.play(track);
  delay(200);
  int file = mp3player.readCurrentFileNumber();
  Serial.print("Track:");
  Serial.println(track);
  Serial.print("File:");
  Serial.println(file);
  while (file != track) {
    mp3player.play(track);
    delay(200);
    file = mp3player.readCurrentFileNumber();
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
      // Serial.println("+ Key < - previous");
      // Stacy, don't previous before the first song.
      mp3player.previous();
      delay(300);
      Serial.print(F("+ Previous, Current FileNumber: "));
      Serial.println(mp3player.readCurrentFileNumber());
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      // Serial.println("+ Key > - next");
      mp3player.next();
      delay(300);
      Serial.print(F("+ Next, Current FileNumber: "));
      Serial.println(mp3player.readCurrentFileNumber());
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
  // If I add an SD card, I could save the state.
  //  When starting, set to the previous state.
  //
  mp3player.setTimeOut(500);   // Set serial communictaion time out
  delay(300);
  mp3player.volume(12);        // Set speaker volume from 0 to 30. Doesn't effect DAC output.
  //
  // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
  mp3player.outputDevice(DFPLAYER_DEVICE_SD);
  //
  // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
  mp3player.EQ(DFPLAYER_EQ_CLASSIC);
  //
  mp3player.loopFolder(currentDirectory);
  // mp3player.play(currentSingle);
  //
  delay(300); // need a delay after the previous mp3player function call, before the next call.
  mp3player.pause();
  playPause = true;
  Serial.println(F("+ DFPlayer is initialized."));

  // ---------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized the infrared receiver."));

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
void loop() {

  delay(60);

  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    playerInfraredSwitch();
    irrecv.resume();
  }
  // playMp3(); Use hard continuance.

}
// -----------------------------------------------------------------------------
