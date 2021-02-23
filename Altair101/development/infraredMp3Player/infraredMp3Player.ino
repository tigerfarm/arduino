// -------------------------------------------------------------------------------
/*
  Test infrared receiver.

  + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  Connect infrared receiver, pins from top left to right:
    Left most (next to the S) - Arduino pin A1 (or using another analog or digital pin)
    Center - VCC
    Right  - GND (ground)

   A1 + -   - Arduino pin connections
    | | |   - Infrared receiver pins
  ---------
  |S     -|
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------
    Front

  ------------------------------------------------------------------------------
  Install infrared library for Due.
  Project:
    https://github.com/enternoescape/Arduino-IRremote-Due/
    Code dropdown has as Download Zip option.
  On my Mac,
    Move the unzipped files directory, into the directory: Users/.../Documents/Arduino/libraries.
    Unzip the file.

  -----------------------------------------------------------------------------
  DFPlayer Mini pins

  DFPlayer pin 3 (TX) to Arduino RX19.
  DFPlayer pin 2 (RX) to a 5K resister. 5K resister to Arduino TX18.
  DFPlayer pin GND    to Arduino GND. Arduino GND to USB hub GND.
  DFPlayer pin VCC    to USB hub VCC.

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

  Due pins                    TX1:18 RX1:19
   ---------------------------------------
  |                                x x   -| GND
  |                                       |
  |                                       | ...
  |                                       |
  |                                       |
   ---------------------------------------

  USB, from top:
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
  + Purple VCC, +5 VCC.

*/
// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// DFPlayer Mini MP3 play

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3player;

// MP3 player has a sub-state for managing sound effect byte values in files.
#define PLAYER_FILE 0
#define PLAYER_MP3 1
#define PLAYER_VOLUME_SETUP 16
int playerState = PLAYER_MP3;     // Intial, default.

// ------------------------------
// DFPlayer Mini MP3 player

// Front panel display light values:
uint16_t playerCounter = 1;                     // First song played when player starts up. Then incremented when next is played.
uint8_t playerStatus = 0;
uint8_t playerVolume = 0;
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
//
void mp3playerPlay(int theCounter) {
  if (NOT_PLAY_SOUND) {
    return;
  }
  currentPlayerCounter = theCounter;
  mp3player.play(theCounter);
}
//
void playerSetup() {
  // Initialize the player module.
  // This allows it to be reset after the computer is restarted.
  //
  // Set player front panel values.
  playerCounter = 1;                  // For now, default to song/file 1.
  playerVolume = PLAYER_VOLUME_SETUP;
  playerStatus = OUT_ON | HLTA_ON;    // ,  LED status light to indicate the Player.
  playerDirectory = 1;
  //
  // -------------------------
  // Test:
  //    mp3player.reset(); //Reset the module
  //    Serial.println(mp3player.readState()); // State: can tell if a song is playing?
  if (hwStatus > 0) {
    hwStatus = 0;
    if (!mp3player.begin(Serial1)) {
      delay(500);
      if (!mp3player.begin(Serial1)) {
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
    NOT_PLAY_SOUND = false;  // Set to play sound effects.
    mp3player.volume(PLAYER_VOLUME_SETUP);   // Set speaker volume from 0 to 30.
    // delay(300);
    mp3player.setTimeOut(60);         // Set serial communications time out.
    // delay(300);
    //
    // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
    mp3player.outputDevice(DFPLAYER_DEVICE_SD);
    //
    // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
    mp3player.EQ(DFPLAYER_EQ_CLASSIC);
    //
    // delay(300);
    playerCounterTop = mp3player.readFileCounts();
    if (playerCounterTop == 65535) {
      delay(300);
      playerCounterTop = mp3player.readFileCounts();
    }
    Serial.print(F("+ DFPlayer is initialized. Number of MP3 files = "));
    Serial.println(playerCounterTop);
  }
}

void pausePlayer() {
  // Twice because sometimes, once doesn't work.
  mp3player.pause();
  delay(100);
  mp3player.pause();
  playerStatus = playerStatus | HLTA_ON;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// Check Front Panel Control Switches, when in Player mode.

void playCounterHlta() {
  if (!(playerStatus & HLTA_ON)) {
    mp3player.play(playerCounter);
    currentPlayerCounter = playerCounter;
  }
  playerLights();
}

void checkProtectSetVolume() {

  // -------------------
  if (playerVolume > 1) {
    playerVolume--;
  }
  mp3player.volume(playerVolume);
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, decrease volume to "));
  Serial.println(playerVolume);
#endif

  // -------------------
  if (playerVolume < 30) {
    playerVolume++;
  }
  mp3player.volume(playerVolume);
  // playerLights();
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, increase volume to "));
  Serial.println(playerVolume);
#endif

  // -------------------
#ifdef SWITCH_MESSAGES
  Serial.println(F("+ Player, STOP: stop playing."));
#endif
  mp3player.pause();
  playerStatus = playerStatus | HLTA_ON;
  playerLights();

  // -------------------
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, RUN: start playing, playerCounter="));
  Serial.println(playerCounter);
#endif
  // Logic to handle playerCounter changes when HLTA_ON.
  if (playerCounter == currentPlayerCounter) {
    mp3player.start();
  } else {
    mp3player.play(playerCounter);
    currentPlayerCounter = playerCounter;
  }
  // This fixes an issue of skipping 2 songs when first run
  //  because, else, playMp3() is called before player status changes to busy,
  //  and it increments playerCounter.
  while (mp3player.available()) {
    mp3player.readType();
  }
  Serial.println("");
  //
  playerStatus = playerStatus & HLTA_OFF;
  playerLights();

  // -------------------
  byte dataToggles = toggleData();   // Use data toggles instead of: unsigned int addressToggles = toggleAddress();
  if (dataToggles > 0 && dataToggles <= playerCounterTop) {
    playerCounter = dataToggles;
    playCounterHlta();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, EXAMINE: play a specific song number, playerCounter="));
    Serial.println(playerCounter);
#endif
  } else {
    ledFlashError();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, EXAMINE: data toggles out of player file count range: "));
    Serial.print(dataToggles);
    Serial.print(F(", player file counts: "));
    Serial.println(playerCounterTop);
#endif
  }
  // -------------------
  if (playerCounter < playerCounterTop) {
    playerCounter++;
  } else {
    playerCounter = 1;
  }
  playCounterHlta();
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, Examine Next: play next song, playerCounter="));
  Serial.println(playerCounter);
#endif

  // -------------------
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, Single Step, "));
#endif
  if (loopSingle) {
#ifdef SWITCH_MESSAGES
    Serial.print(F("Toggle loop a single song: off, song# "));
#endif
    loopSingle = false;
    // mp3player.disableLoop();
    playerStatus = playerStatus & M1_OFF;
  } else {
#ifdef SWITCH_MESSAGES
    Serial.print(F("Toggle loop a single song: on, song# "));
#endif
    loopSingle = true;
    playerStatus = playerStatus | M1_ON;
  }
  playerLights();
#ifdef SWITCH_MESSAGES
  Serial.println(playerCounter);
#endif

  // -------------------
  if (playerCounter > 1) {
    playerCounter--;
  } else {
    playerCounter = playerCounterTop;
  }
  playCounterHlta();
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, STEP down: play previous song, playerCounter="));
  Serial.println(playerCounter);
#endif

  // -------------------
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, RESET: set to play first song."));
#endif
  mp3player.stop();   // Required.
  playerSetup();
  playerPlaySoundEffectWait(RESET_COMPLETE);  // Doesn't change the current MP3 file.
  mp3player.play(playerCounter);
  mp3player.stop();
  playerLights();

  // -------------------
  if (playerVolume > 1) {
    playerVolume--;
  }
  mp3player.volume(playerVolume);
  playerLights();
  // delay(300);
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, decrease volume to "));
  Serial.println(playerVolume);
#endif

  // -------------------
  if (playerVolume < 30) {
    playerVolume++;
  }
  mp3player.volume(playerVolume);
  playerLights();
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, increase volume to "));
  Serial.println(playerVolume);
#endif

  // -------------------
  playerStatus = playerStatus & HLTA_OFF;
  if (playerDirectory > 1) {
    playerDirectory --;
  } else if (playerDirectoryTop > 0) {
    // If playerDirectoryTop is set, then loop to the last directory.
    playerDirectory = playerDirectoryTop;
  }
  int depositPlayerCounter = playerCounter;
  mp3player.loopFolder(playerDirectory);
  delay(300);
  playerCounter = mp3player.readCurrentFileNumber();
  int i = 0;
  while ((i++ < 3) && ((playerCounter >= playerCounterTop) || playerCounter == depositPlayerCounter)) {
    delay(300);
    playerCounter = mp3player.readCurrentFileNumber();
  }
  playerLights();
  //
  // Disable the loop function so that playerCounter will be used for next MP3.
  // mp3player.disableLoop();
  //
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, Deposit: play previous folder# "));
  Serial.print(playerDirectory);
  Serial.print(F(", playerCounter="));
  Serial.println(playerCounter);
#endif

  // -------------------
#ifdef SWITCH_MESSAGES
  Serial.print(F("+ Player, Deposit Next"));
#endif
  playerStatus = playerStatus & HLTA_OFF;
  int depositPlayerCounter = playerCounter;
  //
  playerDirectory ++;
  mp3player.loopFolder(playerDirectory);
  delay(300);
  playerCounter = mp3player.readCurrentFileNumber();
  int i = 0;
  while ((i++ < 3) && ((playerCounter >= playerCounterTop) || playerCounter == depositPlayerCounter)) {
    delay(300);
    playerCounter = mp3player.readCurrentFileNumber();
    if ((depositPlayerCounter + 1) >= playerCounter) {
      // + Player, Deposit Next play next folder# 8, playerCounter,pre=59,post=84
      // + Player, Deposit Next play next folder# 9, playerCounter,pre=84,post=86
      playerDirectory--;
      playerDirectoryTop = playerDirectory;
      playerDirectory = 1;
      playerCounter = 1;
      mp3player.loopFolder(playerDirectory);
      delay(300);
    }
  }
  playerLights();
  //
  // Disable the loop function so that playerCounter will be used for next MP3.
  // mp3player.disableLoop();
  //
#ifdef SWITCH_MESSAGES
  Serial.print(F(" play next folder# "));
  Serial.print(playerDirectory);
  Serial.print(F(", playerCounter,pre="));
  Serial.print(depositPlayerCounter);
  Serial.print(F(",post="));
  Serial.println(playerCounter);
#endif

  // -------------------
  // -------------------
  // For debugging player issues.
  /*
    Serial.print(F("+ "));
    Serial.print(F(" playerStatus="));
    printByte(playerStatus);
    Serial.print(F(" playerVolume="));
    Serial.print(playerVolume);
    Serial.print(F(" playerCounterTop="));
    Serial.print(playerCounterTop);
    Serial.print(F(" playerDirectory="));
    Serial.print(playerDirectory);
    Serial.print(F(" playerCounter="));
    Serial.print(playerCounter);
    Serial.print(F(", mp3player.read()="));
    Serial.print(mp3player.read());
    if (mp3player.available()) {
      Serial.print(F(", mp3player available"));
      int theType = mp3player.readType();
      if (theType == DFPlayerPlayFinished) {
        Serial.print(F(", Play Finished."));
      } else {
        // printDFPlayerMessage(theType, mp3player.read());
        Serial.print(F(", theType="));
        Serial.print(theType);
      }
    } else {
      // Song is playing or paused.
      Serial.print(F(", mp3player not available."));
    }
    Serial.println();
  */
}

// -------------------------------------------------------------------------------
// DFPlayer configuration and error messages.

void printDFPlayerMessage(uint8_t type, int value);
void printDFPlayerMessage(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
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
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case FileIndexOut:
          if (loopSingle) {
            // For some reason, turning on single looping (mp3player.loop(playerCounter);)
            //    on the last song causes this error.
            //    This is ignore the error and get the song player.
            mp3player.start();
          } else {
            Serial.println(F("File Index Out of Bound"));
          }
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          Serial.print("+ Assume the directory number was incremented too high, play previous directory: ");
          if (playerDirectory > 1) {
            playerDirectory = 1;
          } else {
            playerDirectory = 1;
          }
          Serial.println(playerDirectory);
          mp3player.loopFolder(playerDirectory);
          break;
        default:
          Serial.print(F("Unknown DFPlayer error message value:"));
          Serial.println (value);
          break;
      }
      break;
    default:
      Serial.print(F("Unknown DFPlayer message type: "));
      Serial.print(type);
      Serial.print(F(", value:"));
      Serial.println(value);
      playerCounter = value;  // The song to play.
      playerLights();
      break;
  }
}

void playMp3() {
  if (mp3player.available()) {
    int theType = mp3player.readType();
    // ------------------------------
    if (theType == DFPlayerPlayFinished) {
      if (loopSingle) {
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Loop/play the same MP3: "));
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
      mp3playerPlay(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      if (programState == PLAYER_RUN) {
        // This "if", allows continuous playing in other modes (clock) without effecting their dislay lights.
        playerLights();
      }
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

void runPlayer() {
#ifdef SWITCH_MESSAGES
  Serial.println(F("+ runPlayer(), programState: PLAYER_RUN."));
#endif
  playerPlaySound(PLAYER_ON);
  playerLights();
  Serial.println("");
  while (programState == PLAYER_RUN) {
    delay(60);
    switch (playerState) {
      // ----------------------------
      case PLAYER_MP3:
        // Play MP3 files
        if (!(playerStatus & HLTA_ON)) {
          playMp3();
        }
        checkPlayerControls();      // Player control functions from STOP to UNPROTECT.
        break;
      case PLAYER_FILE:
        checkPlayerFileControls();  // Player control functions from STOP to UNPROTECT.
        checkProtectSetVolume();
        break;
    }
    checkAux1();          // Toggle between processor, clock, and player modes.
  }
  playerPlaySound(PLAYER_OFF);
  restoreLcdScreenData();
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// For the infrared receiver.

// This is the only difference when using the non-Due infrared library.
// The non-Due infrared library is: #include <IRremote1.h>
#include <IRremote2.h>

// Other pins can be used. I've use both analog and digital pins.
// int IR_PIN = A1;
int IR_PIN = 24;

IRrecv irrecv(IR_PIN);
decode_results results;

void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0x7E23117B:
    case 0xFF10EF:
      Serial.println("+ Key < - previous");
      break;
    case 0x7538143B:
    case 0xFF5AA5:
      Serial.println("+ Key > - next");
      break;
    case 0x8AA3C35B:
    case 0xFF18E7:
      Serial.println("+ Key up");
      break;
    case 0xFA2F715F:
    case 0xFF4AB5:
      Serial.println("+ Key down");
      break;
    case 0x82D6EC17:
    case 0xFF38C7:
      pausePlayer();
      Serial.println("+ Key OK|Enter - Toggle");
      break;
    case 0x2C22119B:
      Serial.println("+ Pause");
      break;
    // -----------------------------------
    case 0x1163EEDF:
    case 0xFF9867:
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0x718E3D1B:
    case 0xFFA25D:
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xF8FB71FB:
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xE9E0AC7F:
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    case 0x38BF129B:
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("");
      break;
    case 0x926C6A9F:
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("");
      break;
    case 0xE66C5C37:
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("");
      break;
    case 0xD75196BB:
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("");
      break;
    case 0x72FD3AFB:
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("");
      break;
    case 0xCCAA92FF:
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.println("+ Key * (Return)");
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.println("+ Key # (Exit)");
      break;
    // -----------------------------------
    case 0x6D8BBC17:
      Serial.println("+ Key Channel ^");
      break;
    case 0xCDFC965B:
      Serial.println("+ Key Channel v");
      break;
    // -----------------------------------
    case 0x1CF3ACDB:
      Serial.println("+ Key Volume ^");
      break;
    case 0x2B8BE5F:
      Serial.println("+ Key Volume v");
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      Serial.println(results.value, HEX);
      // -----------------------------------
  } // end switch

}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  irrecv.enableIRIn();
  Serial.println("+ Initialized the infrared receiver.");

  Serial.println("++ Go to loop.");
}

// -------------------------------------------------------------------------------
void loop() {

  // -----------------------------
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }
  switch (programState) {
    // ----------------------------
    case PLAYER_RUN:
      digitalWrite(HLDA_PIN, HIGH);
      runPlayer();
      digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
  }

  delay(60);
}

// -------------------------------------------------------------------------------
