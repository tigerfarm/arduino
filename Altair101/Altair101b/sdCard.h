// -------------------------------------------------------------------------------
// From sdCard.cpp for use in other programs.
//  In other programs, include the following line:
//    #include "sdCard.h"

boolean setupSdCard();
void sdCardRun();
void sdCardSwitch(int resultsValue);
boolean readFileToMemory(String theFilename);
boolean writeFileByte(String theFilename, byte theByte);
int readFileByte(String theFilename);
void mp3PlayerPause();

// -------------------------------------------------------------------------------
// eof
