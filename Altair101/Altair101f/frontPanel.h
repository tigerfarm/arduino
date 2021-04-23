// -------------------------------------------------------------------------------
// From frontPanel.cpp for use in other programs.
//  In other programs, include the following line:
//    #include "frontPanel.h"

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits);

byte fpToggleSense();
byte fpToggleData();
uint16_t fpToggleAddress();

void setPcfControlinterrupted(boolean theTruth);
boolean getPcfControlinterrupted();

void checkRunningButtons();
void waitControlSwitches();
void fpCheckAux1();
void checkProtectSetVolume();
void playerControlSwitches();
byte fpCheckAux2();
byte fpTimerControlSwitches();

boolean setupFrontPanel();

// -------------------------------------------------------------------------------
// eof
