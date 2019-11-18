--------------------------------------------------------------------------------
# Altair 101 Project Notes

The Model-T was the first popular car. The Altair 8800 was the first popular personal computer, the Model-T of the home computers.
I designing and building a computer to give a person the experience of using that personal computer.
The Altair 101 will replicate the user experience of flipping toggles for program and data input.
And then the user will enjoy watching flashing LED lights, which is the computer's output.

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

--------------------------------------------------------------------------------
eof