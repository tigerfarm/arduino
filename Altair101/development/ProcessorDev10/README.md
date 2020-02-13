--------------------------------------------------------------------------------
# Altair 101 Processor Version

Major milestone version.
+ Processor.ino emulates the basic interactivity of the Altair 8800.
+ The toggles and LED lights work as expected.
+ The test program, Kill the Bit, runs great!
+ The Java assembler program works to generate and upload machine code to the Altair 101 dev machine.
+ The emulator's memory can be write to the micro SD card, and then later, read back into memory.
+ The opcode programs I tested work.

In this version:
+ I cleaned up the data processing function: processData(), and the functions it calls.
+ The status lights now display properly when running or stepping through a program.
+ I confirmed that status lights worked for the opcodes LDA and STA, as seen in the Altair 8800 clone front panel video.
+ I added a function to display the current program values in the panel LED lights:
++  processDataLights() use statusByte, curProgramCounter, and dataByte.
+ The control switch functions were updated to work as expected:
++ STOP, RUN, STEP, EXAMINE, EXAMINE-NEXT, DEPOSIT, DEPOSIT-NEXT.
++ Can STOP a program. Then flip RUN and the program will continue.
++ EXAMINE, EXAMINE-NEXT, DEPOSIT, DEPOSIT-NEXT, all set the program counter.

--------------------------------------------------------------------------------
Cheers