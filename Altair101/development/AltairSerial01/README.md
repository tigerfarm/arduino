--------------------------------------------------------------------------------
# Altair 8800 Simulator Serial interface Software


I have minimized most of the code and
consolidated the code to run machine instructions to 2 program files with 2 header files.

The serial interface works well.
I use the Arduino IDE monitor. The responses are logging information and text prints of the front panel.

--------------------------------------------------------------------------------

My goal is to use the Altair-Duino code to process machine byte code
in my Altair 101 Processor.ino program.
This will save me finishing the writing and testing of my byte code interpreter.
And, I will know that the interpreter works properly for all the opcodes.

Steps to use the code for running opcode instructions.

Create a minimum code base to run machine code:
+ 1. Get the simulator to run on Mega, maybe the Due. I have it running on Mega.
+ 2. Get the serial output to work properly. I'm using MacWise VT100 emulator.
+ 3. I found the documentation. I can interact with the Simulator over serial command line.
+ 4. I can enter and run simple programs. Load and run CPM?
+ 5. Remove programs not require to run a machine code programs, such as: dazzler.*, disassembler.*, and others.

After the above, I chose to modify Altair101b to not use VT100 emulation
but to simply output text to the serial port which works well in the Arduino IDE monitor.

Modify the minimum code base to work with Altair 101:
+ 6. Manage Altair 101 memory size.
+ 7. Starting with a simple Serial control program, Add Altair 880 Simulator functions.
        Get memory manage to work (EXAMINE and DEPOSIT).
+ 8. Get a Program to Run and Add monitor/debug functions.
+ 9. Consolidate the program files (*.h and *.cpp).
+ 10. Run on the Altair 101 desktop machine.
+ 11. Output to Altair 101 panel LED.
+ 12. Input from Altair 101 panel switches.


--------------------------------------------------------------------------------
Cheers
