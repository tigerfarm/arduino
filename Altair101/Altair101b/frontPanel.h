// -------------------------------------------------------------------------------
// From frontPanel.cpp for use in other programs.
//  In other programs, include the following line:
//    #include "frontPanel.h"

byte fpToggleSense();
byte fpToggleData();
uint16_t fpToggleAddress();

void setPcfControlinterrupted(boolean theTruth);
boolean getPcfControlinterrupted();

void checkRunningButtons();
void waitControlSwitches();
void checkAux1();
void checkProtectSetVolume();

boolean setupFrontPanel();

// -------------------------------------------------------------------------------
// eof
