--------------------------------------------------------------------------------
# Altair 101 Project Notes

The Model-T was the first popular car. The Altair 8800 was the first popular personal computer, the Model-T of the home computers.
I wanted an Altair 8800 when it was first available in 1975, but I was short on funds.
Now, I am designing and building a computer that has similarities to the 8800.
I call mine, the Altair 101. The 101 will replicate the basic user experience of using an 8800:
entering programs by flipping toggles, and then the enjoyment of watching flashing LED lights as the program runs.

The following photo is my Altair 8800 clone front panel.
The front panel is equivalent to a smartphone's touch screen display only retro.
As it only has LED lights and toggles, it's very retro.

<img width="600px"  src="FrontPanel01c.jpg"/>

The Altair 101 will have an Altair 8800 look and feel from the user's point of view.
It will be functional and have the ability to load programs from an SD card and save programs to an SD card.
The lack of inexpensive method to save and load programs was the reason I didn't buy an original when it was first available in 1975.

The current processor software program runs on an Arduino Nano.
I have a few working operational codes that I used to run a few basic Altair 8800 machine code programs.

My goal is to,
+ Build a computer that has an [Altair 8800 clone front panel](https://www.adwaterandstir.com/product/front-panel/),
similar LEDs and toggles to an [original Altair 8800](https://en.wikipedia.org/wiki/Altair_8800).
+ It will be built with a minimum amount of soldering as the chips, module boards, and other electronic components will be built on breadboards.
+ It will run original machine code which gives it the basic Altair 8800 functionality.

The first phase is to write the processor software and build the hardware.
The Altair 101 software needs to run programs such as Kill the Bit, and Pong, on the front panel.
The base hardware components need to be built and put into the clone case I bought.
I will be able to save, and load programs and data using an SD card.

Once the base machine is complete, I will add modern components and functionality:
+ MP3 player for playing sounds and music,
+ A real time clock that can display the time on the front panel,
+ A digital display,
+ Infrared receiver to receive commands and data from a remote control, such as a TV remote.
+ and yes, connect it to the internet.

#### Getting Started with an Altair 8800 Clone Front Panel

A major milestone will be when I have enough opcodes, operational instructions, to run the classic program, 
[Kill the Bit](https://youtu.be/ZKeiQ8e18QY).
I will use an Arduino Nano microprocessor sketch program to interpret and process each operational instruction.

I don't intend to implement full Altair 8800 operational instructions.
For example, my machine will not run Basic, nor will it run CPM. Anyway, you can run those on a laptop computer using an emulator.
I just want a computer that looks and feels like the limited edition that was available in 1975.
That version did not have any interfaces. It cost $621 with extremely limited utility.

If I had worked during one high school summer, I could have bought one and had a few bucks to spare.
I didn't buy one because without other parts, there was little it could do.
After entering a program, it couldn't even save the program. I would need to re-enter the program every time it was turn off and back on.
A usable disk drive cost 3 times as much as the computer.
Too much a poor high school student doing golf course maintenance work for under $2/hour.

--------------------------------------------------------------------------------
### Development Steps

Mid October

Seen an Altair 8800 on the British show, [The IT Crowd](https://theitcrowd.fandom.com/wiki/The_IT_Crowd).
The Centre for Computing History had loaned the show an Altair 8800.
This got me to thinking, and remembering, how I wanted one when they came out in 1975.
At the time, I could afford the base machine, but, it had no save and load method.
So, once I entered a program, it would be lost when powered down.
Save and load hardware cost even more than the machine, which I couldn't afford.

I began watching Altair 8800 videos.
I read Altair clone websites.
[Altair 8800 clone](https://altairclone.com/) for $600, replicas for 
[Altair-Duino replica](https://www.adwaterandstir.com/2017/08/14/but-what-could-you-actually-do-with-an-altair-8800/)
kits for $180 or $250. But my soldering skills suck, I wasn't about to attempt a PCB kit.

I took note of their design features:
+ A case, plastic or metal
+ An [Arduino](https://www.arduino.cc/en/main/products) Duino or Mega
+ Free [open-source software simulator](https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6),
    by David Hansel. Or write your own.

All I needed was case, an Arduino microcontroller, and simulation software to emulate an Altair 8800.
It took a while to figure out what constituted a simulator,
+ The ability to run Intel 8080 machine code.
+ Interact with the front panel toggles and LED lights.
+ Make use of external I/O devices such as serial communications to a terminal or device reader such as an SD card module.

After I had mostly designed my machine and had half of it working, I found two others that used a Nano.
They also had written their own emulator program.
Viewing their code, websites, and videos, I noted that I was on track, that my design and program would work.

The simulator emulator clone replicas, used SD card for reading programs.
An SD card module was under $2. I could build a replica with an SD card module.

Next, came the research and design stage.

To get started, I put together components on a breadboard,
+ Nano, SD card read/write module.
+ 
````

  ---------------------------------------------
  Hardware and software updates to complete the core system:

  Completed: Add 8 toggles and a total of 3 x 595 chips, to the dev machine.
  + Solder and add 8 toggles (on/off) to the dev machine.
  ++ Add toggle software controls from the shiftRegisterInputToggle program.
  Emulator Program Logic,
  + Add Reset, which resets the program counter 0, to start a program over.
  + When a program is running,
  ++ Only monitor and execute options: STOP and RESET.
  + When a program is not running,
  ++ Monitor and execute options: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, and RESET.
  ++ Use the toggle address to Examine data in program memory.
  ++ Use the toggle binary value to Deposit data into program memory.

  Emulator Program Logic,
  + When a program is running,
  ++ Use the toggles as sense switches (input) via the IN opcode.
  ++ Load and play, Kill the Bit.
  ++ Pause and change the bit move speed. Then start it up again using RUN or RESET.

  The computer finally has the basic functionality of an Altair 8800.
  + Kill the Bit, is the standard defacto basic demostration program of an Altair 8800 and its clones, and replicas.
  + And at this point, Altair 101 if functionaly complete!
  + The only major difference, is that I don't have all the 8080 opcodes implemented.

  ---------------------------------------------
  The basic development computer is complete!

  Test it out...

  ---------------------------------------------
  Add modern I/O components,

  Add SD card,
  + Save program memory to card, load program memory from card.
  ++ Use an on/off/on toggle: up to save (upload), down to load (download).

  Add a 1602 LCD,
  ++ Confirm messages on the LCD: "Confirm, save to file x." Or, "Confirm, load file x."
  +++ The file number, is the toggle value. For example, 003.MEM, is A8 and A9 toggles up.
  ++ View the result: "Saved.", "Loaded.", or "Error."

  Add DS3231 clock.
  + Use an on/off/on toggle to display the time on the LCD.
  + Time is shown on the LCD, when the LCD isn't used by a running program.
  + Program option to take over the LEDs to display the time.

  Add MP3 player (DFPlayer) and amp.
  ++ Controled using an infrared controller. At first, independent from programs running.

  ---------------------------------------------
  Build my first Altair 101 machine,

  + Complete the final design.
  + Order parts to build the machine.
  + Make enhancements to the case so that it's ready for the electronic parts.
  + Build a new breadboard computer to fit into the case.
  ++ Use an Audruino UNO.
  + Put it all together.

  ---------------------------------------------
  Program Development Phase

  + A basic assembler to convert assembly programs into machine code.
  + Implement the next major Altair 8800 sample program, Pong.
  + Update and enhance my set of test and development programs.
  ++ Samples: looping, branching, calling subroutines.
  ++ Continue adding opcodes with sample programs to confirm that the opcodes are working correctly.

````

--------------------------------------------------------------------------------
#### Reference links

David Hansel wrote an [Altair 8800 simulator](https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6).
It is the code people use when using an Arduino Due to build an Altair 8800 clone/simulator computer.

[Altair 8800 Emulator Kit](https://www.adwaterandstir.com/product/altair-8800-emulator-kit/), $179.95 – $249.95
+ Home page
https://www.adwaterandstir.com/altair/

Very nice [Altair 8800 Clone](https://altairclone.com/).
+ It has an amazing case that was designed based on the original.
+ Click [here](https://altairclone.com/downloads/interview.mp3)
    to hear an interview with the Altair 8800 clone creator, Mike Douglas.
+ I am following his [Altair 8800 instructional videos](https://www.youtube.com/playlist?list=PLB3mwSROoJ4KLWM8KwK0cD1dhX35wILBj)
    to build my processor emulation software and hardware. I want my machine to do the basics of what his machine can do.
+ Altair 8800 Clone [order information](https://altairclone.com/ordering.htm), (assembled) $621.

Another Altair Clone: [GitHub](https://github.com/companje/Altair8800), [website](http://www.companje.nl/altair.html).
This one resembles some of my goals: runs on an Arduino UNO(Nano), and simple code base.
I uses 74HC595 for LED outputs, and 74HC166 for inputs, and a 23LC1024 for 128KB RAM.

[Directory](https://altairclone.com/downloads/cpu_tests/) of nice sample assembler programs
such as this [8080 opcode test program](https://altairclone.com/downloads/cpu_tests/TST8080.PRN).

  Extract highByte()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/highbyte/
  Extract lowByte()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/lowbyte/

  Reference document, Intel 8080 Assembly Language Programming Manual:
    https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf
  This section is base on section 26: 8080 Instruction Set
    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf
  Text listing of 8080 opcodes:
    https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/ProcessorOpcodes.txt
    https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/8080opcodesBinaryList.txt

--------------------------------------------------------------------------------
eof