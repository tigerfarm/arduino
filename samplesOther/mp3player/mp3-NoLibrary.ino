/*           MP3 PLAYER PROJECT
/// http://educ8s.tv/arduino-mp3-player/

  Documentation: https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299
*/

#include "SoftwareSerial.h"
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

# define ACTIVATED LOW

SoftwareSerial mySerial(10, 11);

int buttonNext = 2;
int buttonPause = 3;
int buttonPrevious = 4;
boolean isPlaying = false;

// -----------------------------------------------------------------------
void initializePlayer() {
  execute_CMD(0x3F, 0, 0);  // Initialization
  delay(300);
  setVolume(28);            // 0 to 30.
  execute_CMD(0x07, 0, 3);  // Equalizer: 1:Normal 2:Pop 3:Rock 4:Jazz 5:Classic 6:Base
  delay(300);
  execute_CMD(0x11, 0, 1);  // Repeat play, 1:repeat, 0:stop play
  // execute_CMD(0x03, 0, 2);  // Play a specific track.
  // execute_CMD(0x0D, 0, 1);  // Playback
  // delay(300);
  // execute_CMD(0x08, 1, 7);  // Repeat, 1:folder 2:single 3:random
/*
 * Nothing plays.
  execute_CMD(0x03, 0, 2);  // Play a specific track.
  execute_CMD(0x08, 0, 0);  // Repeat, 1:folder 2:single 3:random
  
 * Plays track 1, ignores the specified track. Repeats track 1.
  execute_CMD(0x03, 0, 2);  // Play a specific track.
  execute_CMD(0x08, 0, 1);  // Repeat, 1:folder 2:single 3:random
  
 * Plays track 1. Repeats track 1.
  execute_CMD(0x08, 0, 1);  // Repeat, 1:folder 2:single 3:random
  
 * Plays track 2, and repeats track 2:
  execute_CMD(0x03, 0, 2);  // Play a specific track.
  execute_CMD(0x08, 0, 2);  // Repeat, 1:folder 2:single 3:random

 * Plays random track (track 3), ignores the specified track. Repeats track 3.
  execute_CMD(0x03, 0, 2);  // Play a specific track.
  execute_CMD(0x08, 0, 3);  // Repeat, 1:folder 2:single 3:random

 * Same as above.
  execute_CMD(0x08, 0, 3);  // Repeat, 1:folder 2:single 3:random

 * Nothing placys. I press pause/play button, and track 7 plays.
  execute_CMD(0x08, 1, 7);  // Repeat, 1:folder 2:single 3:random
  
 * Plays track 7.
  execute_CMD(0x08, 1, 7);  // Repeat, 1:folder 2:single 3:random
  delay(300);
  execute_CMD(0x0D, 0, 1);  // Playback

 */
}
void pause() {
  execute_CMD(0x0E, 0, 0);  // Pause command
  delay(500);
}
void play() {
  execute_CMD(0x0D, 0, 1);  // Playback
  delay(500);
}
void playNext() {
  execute_CMD(0x01, 0, 1);  // Next
  delay(500);
}
void playPrevious() {
  execute_CMD(0x02, 0, 1);  // Previous
  delay(500);
}
void setVolume(int volume) {
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
  delay(500);
}

// -----------------------------------------------------------------------
// Excecute the command and parameters
void execute_CMD( byte CMD, byte Par1, byte Par2 ) {
  // Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = {
    Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
    Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
  };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++) {
    mySerial.write( Command_line[k]);
  }
}

// -----------------------------------------------------------------------
void setup () {
  Serial.println("+++ Setup started.");
  pinMode(buttonPause, INPUT);
  pinMode(buttonNext, INPUT);
  pinMode(buttonPrevious, INPUT);
  digitalWrite(buttonPause, HIGH);
  digitalWrite(buttonNext, HIGH);
  digitalWrite(buttonPrevious, HIGH);
  //
  mySerial.begin (9600);
  // delay(1000);
  initializePlayer();
  isPlaying = true;
  //
  Serial.begin(9600);
}

// -----------------------------------------------------------------------
void loop () {
  Serial.print("isPlaying = ");
  Serial.println(isPlaying);
  delay(500);

  // I should program in toggle rather than depending on how long the button is pressed.
  if (digitalRead(buttonPause) == ACTIVATED) {
    if (isPlaying) {
      pause();
      isPlaying = false;
    } else {
      play();
      isPlaying = true;
    }
  }
  if (digitalRead(buttonNext) == ACTIVATED) {
    if (isPlaying) {
      playNext();
    }
  }
  if (digitalRead(buttonPrevious) == ACTIVATED) {
    if (isPlaying) {
      playPrevious();
    }
  }
  
} // end loop

// -----------------------------------------------------------------------
// eof
