--------------------------------------------------------------------------------
# Altair 101a Simulator Machine

This machine has a serial port interface to an Altair 8800 simulator.
It has a rudimentary OS to run the simulator from command line.
Or, the simulator can be run using a virtual Altair 8800 front panel.
I'm using David Hansel's Altair 8800 Simulator code to run the machine instructions. 
The hardware is a Mega 2560 board, micro SD card reader, serial port module, and a 4 port USB hub.

How to connect from a UNIX or Mac OS terminal:
````
$ ls /dev/tty.*
... /dev/tty.wchusbserial14210  /dev/tty.wchusbserial14220
$ screen /dev/tty.wchusbserial14210 115200
+++ Setup.
+ SD card module is initialized.
+++ Altair 101a initialized.
?- 
````

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
