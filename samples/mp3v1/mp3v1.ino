/***************************************************
   DFPlayer - A Mini MP3 Player For Arduino

   Original GitHub project:
   https://github.com/DFRobot/DFRobotDFPlayerMini
   Get started sample:
   https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/GetStarted/GetStarted.ino
   Full functions test:
   https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/FullFunction/FullFunction.ino

  Created 2016-12-07 bBy [Angelo qiao](Angelo.qiao@dfrobot.com)
  GNU Lesser General Public License.
  See <http://www.gnu.org/licenses/> for details.
  All above must be included in any redistribution

  Connection and Diagram can be found here
  https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram
*/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define BUTTON_NEXT_PIN 2
#define BUTTON_PAUSE_PIN 3
#define BUTTON_PREVIOUS_PIN 4

boolean loopAll = true;
boolean loopSingle = false;

boolean playPause = false;
boolean buttonPause = false;
boolean buttonNext = false;
boolean buttonPrevious = false;
boolean buttonPausePressed = false;
boolean buttonNextPressed = false;
boolean buttonPreviousPressed = false;

// -----------------------------------------------------------------------
SoftwareSerial mySoftwareSerial(10, 11);      // RX, TX
DFRobotDFPlayerMini myDFPlayer;
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
  Serial.println(F("++ Initializing, may take a few seconds..."));
  //
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  //
  pinMode(BUTTON_PAUSE_PIN, INPUT);
  pinMode(BUTTON_NEXT_PIN, INPUT);
  pinMode(BUTTON_PREVIOUS_PIN, INPUT);
  digitalWrite(BUTTON_PAUSE_PIN, HIGH);
  digitalWrite(BUTTON_NEXT_PIN, HIGH);
  digitalWrite(BUTTON_PREVIOUS_PIN, HIGH);
  //
  myDFPlayer.volume(28);  //Set volume value. From 0 to 30
  // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  //
  // myDFPlayer.play(2);
  myDFPlayer.start();
}

// -----------------------------------------------------------------------
void getInputs() {
  buttonPause = !(digitalRead(BUTTON_PAUSE_PIN));
  buttonNext = !(digitalRead(BUTTON_NEXT_PIN));
  buttonPrevious = !(digitalRead(BUTTON_PREVIOUS_PIN));
}

void loop() {
  // Serial.println("+ Loop.");
  getInputs();
  delay(50);
  
  // ---------------------------------------------------------------------
  // Buttons: Pause, Next, Previous
  
  if (buttonPause) {
    if (!buttonPausePressed) {
      Serial.print("+ Press: buttonPause ");
      Serial.println(playPause);
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
  if (buttonNext) {
    if (!buttonNextPressed) {
      Serial.println("+ Press: buttonNext");
      myDFPlayer.next();
      buttonNextPressed = true;
    }
  } else {
    buttonNextPressed = false;
  }
  if (buttonPrevious) {
    if (!buttonPreviousPressed) {
      Serial.println("+ Press: buttonPrevious");
      myDFPlayer.previous();
      buttonPreviousPressed = true;
    }
  } else {
    buttonPreviousPressed = false;
  }
  
  // ---------------------------------------------------------------------
  // Handle continuous playing, and play errors such as, memory card not inserted.
  // Note, after card is inserted, the player automatically starts playing.
  
  if (myDFPlayer.available()) {
    int theType = myDFPlayer.readType();
    if (theType == DFPlayerPlayFinished) {
      Serial.println("+ The playing of the MP3 file has completed.");
      // When one song ends, play the next song.
      myDFPlayer.next();
    } else if (theType == DFPlayerCardInserted ) {
      myDFPlayer.start();
    }
    else {
      //Print the detail message from DFPlayer to handle different errors and states,
      // such as memory card not inserted.
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

}

// ---------------------------------------------------------------------
// eof
