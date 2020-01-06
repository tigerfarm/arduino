--------------------------------------------------------------------------------
# Altair 101 Development Steps

  The Altair 101 is a hardware and software emulator of the basic Altair 8800 computer from 1975.
  The current development computer is functionally complete!

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

````
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Cheers