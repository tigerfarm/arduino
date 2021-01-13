--------------------------------------------------------------------------------
# Altair 101a Simulator Machine

This machine is a serial port interactive Altair 8800 simulator.
It has rudimentary OS to run programs from command line.
Or it can be run using a virtual Altair 8800 front panel display and virtual switches.
The hardware is a Mega 2560 board, micro SD card reader, second hardware serial port, on a 4 port USB hub.

Description:
+ Runs 8080 machine code programs.
+ It can be controlled from command line.
+ Or it can be run using a virtual Altair 8800 front panel display.
+ The virtual front panel has the usual switches: program control, address, sense, and data.
+ Interactivity has been tested using the Arduino IDE monitor and a Mac OS VT100 terminal.
+ It has 5K of program memory.

Hardware components:
+ Arduino Mega 2560 board
+ Micro SD card reader with a 32gig card
+ USB serial port module
+ 4 port USB hub with on/off switches for each port
+ 8 wires and a USB cable

Software:
+ I wrote a rudimentary OS.
+ Interactivity between a laptop and the Mega is through USB serial ports.
+ David Hansel's Altair 8800 Simulator code is used to run the machine instructions. 
+ I wrote an assembler application to assemble programs and upload the machine code to the Altair 101a.

OS:
+ Control initialization.
+ Download programs into program memory through a serial port: Mega Serial2 port.
+ Store and load programs and data from the micro SD card.
+ Manage the running of programs.
+ Control interactive modes: virtual front panel or command line.

--------------------------------------------------------------------------------
````
````
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
Cheers
