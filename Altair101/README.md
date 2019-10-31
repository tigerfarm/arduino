--------------------------------------------------------------------------------
# Altair 101 Project Notes

This computer, which I call the Altair 101, is designed and will be built for a user experience.
It is meant to replicate the user experience of flipping toggles for data entry and watching lights flash to see results.

My goal is to build a computer that has an [Altair 8800 clone front panel](https://www.adwaterandstir.com/product/front-panel/),
similar LEDs and toggles to an Altair 8800, and have the base Altair 8800 functionality.
This will give the computer an Altair 8800 look and feel from the user's point of view.
Then, I will add modern components: real time clock, MP3 player, SD drive for saving and loading programs, and yes, connect it to the internet.

#### Altair 8800 Front Panel

[<img width="600px"  src="Altair8800frontPanel15x6.jpg"/>](https://s2js.com/altair/sim.html)

I will need to handle a limited number of operational instructions, such as the jump (JMP) instruction.
A major milestone will be when I have enough instructions to run the classic, [Kill the Bit](https://youtu.be/ZKeiQ8e18QY) program, on my Altair 101.
I will use an Arduino Nano microprocessor sketch program to interpret and process each operational instruction.

I don't intend to implement full Altair 8800 operational instructions.
Which means, my machine will not run Basic, nor will it run CPM. Anyway, you can run those on a laptop computer using an emulator.
I just want a computer that looks and feels like the limited edition that was available in 1975.
That version did not have any interfaces. It cost $621 with extremely limited utility.

If I had worked during one high school summer, I could have bought one and had a few bucks to spare.
I didn't buy one because without the other parts, there was little it could do.
After entering a program, it couldn't even save program. I would need to re-enter the program every time it was turn off and back on.
A usable disk drive cost 3 times as much as the computer.
Too much a poor high school student doing golf course maintenance work for under $2/hour.

Following are reference links,
+ Altair 101 parts listed [below](https://github.com/tigerfarm/arduino/tree/master/Altair101#parts-to-order-to-build-my-altair-101).
+ Click [here](https://altairclone.com/downloads/)
    for the website of a high end Altair 8800 clone.
+ Click [here](https://altairclone.com/downloads/)
    for Altair 8800 original and clone documents.
+ Click [here](https://altairclone.com/downloads/interview.mp3)
    to hear an interview with the Altair 8800 clone creator, Mike Douglas.
+ Altair 8800 Instructional Videos
    https://www.youtube.com/playlist?list=PLB3mwSROoJ4KLWM8KwK0cD1dhX35wILBj


--------------------------------------------------------------------------------
### Reference links

+ Instruction set.
http://brooknet.no-ip.org/~lex/altair/_altair88b/manual/instructMain.html
+ Example: jump (jmp) 11 000 011 

+ How to load software onto the Arduino Due.
https://www.adwaterandstir.com/install/
+ Code repository
https://github.com/dhansel/Altair8800

+ How to build one.
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6
+ Build your own.
https://spectrum.ieee.org/geek-life/hands-on/build-your-own-altair-8800-personal-computer
+ About, with parts list
https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6

+ Getting started with the Arduino Due
https://www.arduino.cc/en/Guide/ArduinoDue
+ The Arduino Due is a microcontroller board based on the Atmel SAM3X8E ARM Cortex-M3 CPU.
+ It is the first Arduino board based on a 32-bit ARM core microcontroller.
+ The microcontroller mounted on the Arduino Due runs at 3.3V
+ 54 digital input/output pins, 12 analog inputs
+ Uses CH340G drivers
+ DUE R3 Board SAM3X8E 32-bit ARM Cortex-M3 Control Board Module For Arduino, US $14.64
https://www.ebay.com/itm/DUE-R3-Board-SAM3X8E-32-bit-ARM-Cortex-M3-Control-Board-Module-For-Arduino/141976885203
+ Intro to the Arduino Due
https://www.theengineeringprojects.com/2018/09/introduction-to-arduino-due.html
+ Uses the Arduino Due which is an Arduino for more powerful larger scale projects.
https://store.arduino.cc/usa/due
++ The Arduino Due is the first Arduino board based on a 32-bit ARM core microcontroller.
++ 54 digital input/output pins, 12 analog inputs, a 84 MHz clock

+ Assembled & Tested, $279.95 – $349.95
https://www.adwaterandstir.com/product/altair-assembled/

Google forums:
https://groups.google.com/forum/#!forum/altair-duino

+ Manual
https://grantmestrength.github.io/RetroComputerInstructionManual/
https://github.com/GrantMeStrength/RetroComputerInstructionManual

+ Original Altair manual
http://www.classiccmp.org/dunfield/altair/d/88opman.pdf

+ Altair 8800 Clone, Ordering Information, (assembled) $621
https://altairclone.com/ordering.htm

+ MITS Altair Simulator, written entirely in Javascript.
+ and with thanks to Martin Maly and Chris Double for their 8080.js Intel 8080 emulator. 
https://s2js.com/altair/
https://s2js.com/altair/sim.html
https://github.com/maly/8080js

--------------------------------------------------------------------------------
eof