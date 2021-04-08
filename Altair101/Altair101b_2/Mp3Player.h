// -------------------------------------------------------------------------------
// From Mp3PlayerDue.cpp for use in other programs.
//  In other programs, include the following line:
//    #include "Mp3Player.h"

#define PLAY_ALL 0                              // Play all songs and then loop back and start over.
#define PLAY_SINGLE 1                           // Play a single MP3 and then stop.
#define LOOP_SINGLE 2                           // Loop play a single MP3.

void setupMp3Player();
void setPlayMode(uint8_t setTo);
uint8_t getPlayMode();
void playerSwitch(int resultsValue);
void playerContinuous();
void mp3PlayerRun();
void mp3PlayerPause();
void mp3PlayerStart();
void mp3playerSinglePlay(byte theFileNumber);
void mp3PlayerSingleLoop(byte theFileNumber);
void mp3playerPlaywait(byte theFileNumber);

// -------------------------------------------------------------------------------
// eof
