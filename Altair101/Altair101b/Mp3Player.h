// -------------------------------------------------------------------------------
// From Mp3Player.cpp for use in other programs.
//  In other programs, include the following line:
//    #include "Mp3Player.h"

#define PLAY_ALL 0                              // Play all songs and then loop back and start over.
#define PLAY_SINGLE 1                           // Play a single MP3 and then stop.
#define LOOP_SINGLE 2                           // Loop play a single MP3.
//
extern int READ_FILE;
extern int TIMER_COMPLETE;
extern int RESET_COMPLETE;
extern int CLOCK_ON;
extern int CLOCK_OFF;
extern int PLAYER_ON;
extern int PLAYER_OFF;
extern int KR5;
extern int CLOCK_CUCKOO;
extern int TIMER_MINUTE;
extern int DOWNLOAD_COMPLETE;
extern int WRITE_FILE;
extern int KNIGHT_RIDER_SCANNER;
//
boolean setupMp3Player();
void setPlayMode(uint8_t setTo);
extern uint8_t playerStatus;
extern uint8_t playerVolume;
uint8_t getPlayMode();
void playerSwitch(int resultsValue);
void playerContinuous();
void mp3PlayerRun();
void mp3PlayerPause();
void mp3PlayerStart();
void mp3playerSinglePlay(byte theFileNumber);
void mp3PlayerSingleLoop(byte theFileNumber);
void mp3playerPlaywait(byte theFileNumber);
void playerSoundEffect(byte theFileNumber);
void playerSoundEffectWait(byte theFileNumber);

// -------------------------------------------------------------------------------
// eof
