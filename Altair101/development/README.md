--------------------------------------------------------------------------------
# Altair 101 Development Programs

The programs in this directory are programs I used to develop and test specific code
with the goal to use it in Processor.ino, which is the Altair 101 emulator program.
For example, inputSwitchToggle.ino, tests using a 74HC595 Shift Register, serial to parallel chip, 
to read multiple pin inputs. It uses 4 pins on a microcontroller.
Once tested, I then could use the tested code in Processor.ino

pcf8574inputLogic.ino is key program I use when developing code for my toggle inputs.
pcf8574inputBasic.ino was used first, to developed the basic code that is used in pcf8574inputLogic.ino.
pcf8574inputLogic.ino further develops the code by adding logic that is used in Processor.ino.

````
  ---------------------------------------------
  Work,

  + In the process of writing and testing the opcode CMP test program.

  + Fix the 4 and 7 bit wiring on the toggle console.
  + Re-connect the toggle keyboard to the Dev machine using a PCF8574.
  + The toggle console will replace the current breadboard control buttons.

  Connect AUX1 and AUX2 switches to Mega pins.
  + AUX1 to switch between the clock process and the emulator process.
  + AUX1 to switch between the MP3 player and the emulator process.
  + Use RESET to switch back to the emulator process.
  + AUX2 to Read(download) and write(uplaod) program files.

  Add clock,
  + Add clock routines to tell the time using the LED lights.
  ++ Use: clockLedDisplay.ino
  + AUX1, switch up connected to Mega pin to switch to clock process verses emulator process.
  ----------
  Add clock logic,
  + Add which-buttom-pushed,
  ++ Because, if STOP(HLT) or STEP, use "programCounter-1" because programCounter hold the next program step.
  + Wire and test the code implemention of the HLDA LED status indicator,
  ++ Status light: clock function or emulator control.
  ++ HLDA : 8080 processor go into a hold state because of other hardware.

  ----------
  SD card program read and write,
  + Read(download) and write(uplaod) switches need to be connected to Mega pins.
  + Logic to monitor and react to the switches.
  + When saving or reading a file, get the filename from the sense switches.
  ++ If switches are set to: 00000101, then the filename is: 00000101.bin.
  + When rebooting or resetting the Mega, if 00000000.bin exists, load and run it.
  + Display LED lights to notify read/write success or failure.
  + Confirm saving or reading a file.

  ---------------------------------------------
  Program Development Phase

  Create an assembler to convert assembly programs into machine code.
  + Basic assembler works.
  + Add more opcodes and create more opcode test programs.
  + Need to handle directives:
  ++ org : initial address location to load the following bytes.
  ++ equ : Variable value declarations.
  ++ db : String label address declarations, and covert to bytes.
  ++ Sample program: p1.asm.
  + Compile and run the next major Altair 8800 sample program, Pong.
  + Create more samples: looping, branching, calling subroutines, sense switch interation.

````
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Cheers