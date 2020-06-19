--------------------------------------------------------------------------------
# Altair 101 Software

This version works only tested on the desktop model.

Processor.ino.
+ Documented all the modes.
+ Added clock timer mode. Uses an array of time values which are managed using EXAMINE and DEPOSIT.
+ Added MP3 player file mode to manage sound effect file values using EXAMINE and DEPOSIT.
+ Added sound effects on a number events such as flipping AUX1 to change modes.
+ Added an OUT port to play MP3 files, either once or looping.
+ Option to run a program file (00000000.bin) on startup.
+ Improved when indicator lights are on/off: WAIT and HLDA.
+ Improved AUX1 switching to go between clock and player.
+ Can use PROTECT/UNPROTECT to decrease/increase player volume in all modes.
+ Now requires double flip for some options: CLR, read/write program file.

Yet to do,
+ Set clock date and time using toggles.

--------------------------------------------------------------------------------
Cheers