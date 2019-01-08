/**************************************************************

  MP3 player with: play next, previous, loop single, and pause.
  
  Version 3.0
  This is an updated version of Angelo's development version.
  This version includes infrared controls which is way better than breadboard buttons.
  To compile this version, use the library manager to load the
    DFRobot mini player library. For my implementation, I loaded version 1.05.

  // ----------------------------------------------------------
  DFPlayer - A Mini MP3 Player For Arduino

  Original GitHub project:
  https://github.com/DFRobot/DFRobotDFPlayerMini
  Created 2016-12-07 bBy [Angelo qiao](Angelo.qiao@dfrobot.com)
  GNU Lesser General Public License.
  See <http://www.gnu.org/licenses/> for details.
  https://www.dfrobot.com/product-1121.html
  All above must be included in any redistribution
  // ----------------------------------------------------------

  The following link is to a connection Diagram.
  http://educ8s.tv/arduino-mp3-player/
  Then, use this program instead of the one in the project page.
  Also, add another button for the loop single option.
*/

// -----------------------------------------------------------------------
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <IRremote.h>

// Infrared receiver
int IR_PIN = 9;
IRrecv irrecv(IR_PIN);
decode_results results;

#define BUTTON_NEXT_PIN 2
#define BUTTON_PAUSE_PIN 3
#define BUTTON_PREVIOUS_PIN 4
#define BUTTON_LOOP_PIN 5

boolean playPause = false;  // For toggling pause.
boolean loopSingle = false; // For toggling single song.
int currentSingle = 1;      // First song played when player starts up. Then incremented when next is played.
int currentDirectory = 1;   // File directory name on the SD card. Example 1 is directory name: /01.

// For controling button presses: handle a quick click or a click and hold.
boolean buttonLoop = false;
boolean buttonPause = false;
boolean buttonNext = false;
boolean buttonPrevious = false;
boolean buttonLoopPressed = false;
boolean buttonPausePressed = false;
boolean buttonNextPressed = false;
boolean buttonPreviousPressed = false;

// -----------------------------------------------------------------------
SoftwareSerial mySoftwareSerial(10, 11);      // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
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
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
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
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          //
          Serial.print("+ Assume the directory number was incremented too high, play previous directory: ");
          if (currentDirectory > 1) {
            currentDirectory --;
          }
          Serial.println(currentDirectory);
          myDFPlayer.loopFolder(currentDirectory);
          //
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("+++ DFRobot DFPlayer Mini"));
  //
  Serial.println(F("++ Initializing, may take a few seconds..."));
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    /*
     * If SD card not installed, the will require a restart.
     * Without the following, you can put the card in, and music will start playing.
    while (true) {
      delay(0);
    }
    */
  }
  Serial.println(F("+ DFPlayer is initialized."));
  //
  pinMode(BUTTON_LOOP_PIN, INPUT);
  pinMode(BUTTON_PAUSE_PIN, INPUT);
  pinMode(BUTTON_NEXT_PIN, INPUT);
  pinMode(BUTTON_PREVIOUS_PIN, INPUT);
  digitalWrite(BUTTON_LOOP_PIN, HIGH);
  digitalWrite(BUTTON_PAUSE_PIN, HIGH);
  digitalWrite(BUTTON_NEXT_PIN, HIGH);
  digitalWrite(BUTTON_PREVIOUS_PIN, HIGH);
  //
  // ---------------------
  // If I add a SD card for state, I can start based on the previous state.
  //
  myDFPlayer.setTimeOut(300);   // Set serial communictaion time out
  myDFPlayer.volume(18);        // Set speaker volume from 0 to 30. Doesn't effect DAC output.
  // myDFPlayer.volumeUp(); //Volume Up
  // myDFPlayer.volumeDown(); //Volume Down
  //
  // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP 
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  //
  // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
  //
  // myDFPlayer.start();
  myDFPlayer.play(currentSingle);
  //
  // ---------------------
  /*
  // The following should work, but doesn't work for me:
  //
  int theState = myDFPlayer.readState();
  Serial.print("+ mp3 state: ");
  Serial.println(theState);
  //
  Serial.print("+ mp3 state: ");
  Serial.println(myDFPlayer.readState());
  Serial.print("+ current volume: ");
  Serial.println(myDFPlayer.readVolume());
  Serial.print("+ readEQ: ");
  Serial.println(myDFPlayer.readEQ());
  Serial.print("+ readFileCounts: ");
  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.print("+ readCurrentFileNumber: ");
  Serial.println(myDFPlayer.readCurrentFileNumber());
  */
  irrecv.enableIRIn();
  Serial.println(F("+ Initialized the infrared receiver."));
}

// -----------------------------------------------------------------------
void loop() {

  delay(50);

  // ---------------------------------------------------------------------
  // Get button press state.
  buttonLoop = !(digitalRead(BUTTON_LOOP_PIN));
  buttonPause = !(digitalRead(BUTTON_PAUSE_PIN));
  buttonNext = !(digitalRead(BUTTON_NEXT_PIN));
  buttonPrevious = !(digitalRead(BUTTON_PREVIOUS_PIN));
  
  // ---------------------------------------------------------------------
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFFFFFFF:
        // Ignore. This is from holding the key down.
        break;
      // -----------------------------------
      // Song control
      case 0xFF10EF:
        Serial.println("+ Key < - previous");
        buttonPrevious = true;
        break;
      case 0xFF5AA5:
        Serial.println("+ Key > - next");
        buttonNext = true;
        break;
      case 0xFF38C7:
        Serial.println("+ Key OK - if paused, start the song.");
        myDFPlayer.start();
        playPause = false;
        break;
      case 0xFF9867:
        Serial.println("+ Key 0 - Pause");
        myDFPlayer.pause();
        playPause = true;
        break;
      // -----------------------------------
      // Single song loop
      case 0xFF6897:
        Serial.println("+ Key * - Loop on: loop this single MP3.");
        myDFPlayer.pause();     // Pause identifies that loop is on. Else I need a LED to indicate loop is on.
        delay(200);
        myDFPlayer.start();
        loopSingle = true;
        break;
      case 0xFFB04F:
        if (loopSingle) {
          Serial.println("+ Key # - Loop off: Single MP3 loop is off.");
          myDFPlayer.next();    // Play next identifies that loop is off.
          loopSingle = false;
        }
        break;
      // -----------------------------------
      // Folder, file directory selection.
      case 0xFF18E7:
        Serial.print("+ Key up - next directory, directory number: ");
        currentDirectory ++;
        Serial.println(currentDirectory);
        myDFPlayer.loopFolder(currentDirectory);
        // If no directory, get the error message: DFPlayerError:Cannot Find File
        break;
      case 0xFF4AB5:
        Serial.print("+ Key down - previous directory, directory number: ");
        if (currentDirectory > 1) {
          currentDirectory --;
        }
        Serial.println(currentDirectory);
        myDFPlayer.loopFolder(currentDirectory);
        break;
      case 0xFFA25D:
        Serial.print("+ Key 1: ");
        Serial.println("File directory 1");
        // myDFPlayer.playFolder(1, 1); // In a specific directory, play a specific file: /01/0001.mp3;
        currentDirectory = 1;
        myDFPlayer.loopFolder(currentDirectory);  // Doesn't require a specific filename in the directory.
        break;
      case 0xFF629D:
        Serial.print("+ Key 2: ");
        Serial.println("File directory 2");
        currentDirectory = 2;
        myDFPlayer.loopFolder(currentDirectory);
        break;
      case 0xFFE21D:
        Serial.print("+ Key 3: ");
        Serial.println("File directory 3");
        currentDirectory = 3;
        myDFPlayer.loopFolder(currentDirectory);
        break;
      // -----------------------------------
      // Equalizer setting selection.
      case 0xFF22DD:
        Serial.print("+ Key 4: ");
        Serial.println("DFPLAYER_EQ_POP");
        myDFPlayer.EQ(DFPLAYER_EQ_POP);
        break;
      case 0xFF02FD:
        Serial.print("+ Key 5: ");
        Serial.println("DFPLAYER_EQ_CLASSIC");
        myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
        break;
      case 0xFFC23D:
        Serial.print("+ Key 6: ");
        Serial.println("DFPLAYER_EQ_NORMAL");
        myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
        break;
      case 0xFFE01F:
        Serial.print("+ Key 7: ");
        Serial.println("DFPLAYER_EQ_JAZZ");
        myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
        break;
      case 0xFFA857:
        Serial.print("+ Key 8: ");
        Serial.println("DFPLAYER_EQ_ROCK");
        myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
        break;
      case 0xFF906F:
        Serial.print("+ Key 9: ");
        Serial.println("DFPLAYER_EQ_BASS");
        myDFPlayer.EQ(DFPLAYER_EQ_BASS);
        break;
      // -----------------------------------
    }
    irrecv.resume();
  }

  // ---------------------------------------------------------------------
  // Process buttons: Pause, Next, Previous, Loop single song

  // ------------------------------
  if (buttonPause) {
    if (!buttonPausePressed) {
      Serial.print("+ Press: buttonPause ");
      Serial.println(playPause);
      // Toggle the option.
      if (playPause) {
        myDFPlayer.start();
        playPause = false;
      } else {
        myDFPlayer.pause();
        playPause = true;
      }
      buttonPausePressed = true;
    }
  } else {
    buttonPausePressed = false;
  }
  // ------------------------------
  if (buttonNext) {
    if (!buttonNextPressed) {
      Serial.println("+ Press: buttonNext");
      myDFPlayer.next();
      buttonNextPressed = true;
    }
  } else {
    buttonNextPressed = false;
  }
  // ------------------------------
  if (buttonPrevious) {
    if (!buttonPreviousPressed) {
      Serial.println("+ Press: buttonPrevious");
      myDFPlayer.previous();
      buttonPreviousPressed = true;
    }
  } else {
    buttonPreviousPressed = false;
  }
  // ------------------------------
  if (buttonLoop) {
    if (!buttonLoopPressed) {
      Serial.print("+ Press: buttonLoop ");
      Serial.println(loopSingle);
      // Toggle the option.
      if (loopSingle) {
        Serial.println("+ Single MP3 loop is off.");
        myDFPlayer.next();    // Play next identifies that loop is off.
        loopSingle = false;
      } else {
        Serial.println("+ Loop this single MP3.");
        myDFPlayer.pause();  // Pause identifies that loop is on. Else I need a LED to indicate loop is on.
        delay(200);
        myDFPlayer.start();
        loopSingle = true;
      }
      buttonLoopPressed = true;
    }
  } else {
    buttonLoopPressed = false;
  }
  
  // ---------------------------------------------------------------------
  // Handle continuous playing, and play errors such as: memory card not inserted.
  
  if (myDFPlayer.available()) {
    int theType = myDFPlayer.readType();
    // ------------------------------
    if (theType == DFPlayerPlayFinished) {
      Serial.print("+ MP3 file play has completed. ");
      if (loopSingle) {
        Serial.println("Loop/play the same MP3.");
        myDFPlayer.start();
      } else {
        Serial.println("Play next MP3.");
        myDFPlayer.next();
      }
      // ------------------------------
    } else if (theType == DFPlayerCardInserted ) {
      Serial.println(F("+ SD mini card inserted. Start playing"));
      myDFPlayer.start();
    }
    // ------------------------------
    else {
      // Print the detail message from DFPlayer to handle different errors and states,
      //   such as memory card not inserted.
      printDetail(theType, myDFPlayer.read());
    }
  }

}

// ---------------------------------------------------------------------
// eof
