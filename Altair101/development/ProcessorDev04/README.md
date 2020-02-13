--------------------------------------------------------------------------------
# Altair 101 Processor Version

In this version:
+ I fixed the status lights.
+ I added a function to display the current program values displayed in the panel LED lights: processDataLights().
+ I confirmed that status lights worked for the opcodes LDA and STA, as seen in the Altair 8800 clone front panel video.
+ I cleaned up the data processing function: processData(), and the functions it calls.
+ The control switch functions were updated for correctness: EXAMINE, EXAMINE-NEXT, DEPOSIT, DEPOSIT-NEXT.

--------------------------------------------------------------------------------
Cheers