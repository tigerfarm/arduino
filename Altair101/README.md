--------------------------------------------------------------------------------
# Altair 101 Computer

I have designed and built an Altair 8800 computer emulator.
It emulates the basic Altair 8800 hardware functionality from 1975.
The mother board, CPU, and RAM, is an Arduino Mega microcontroller.
The case is from the [Adwater & Stir emulator kit](https://www.adwaterandstir.com/product/altair-8800-emulator-kit/).
LED lights, toggles, chips, and other components are from China factories, bought on [eBay](https://eBay.com).
At first, I programmed enough of the 8080/8085 microprocessor machine instructions (opcodes),
to run the classic program, Kill the Bit.
Running Kill the Bit, was major milestone as it's the quasi essential Altair 8800 demonstration program.

In the next phase, I replaced my simulator code to execute machine instructions
with David Hansel's Altair 8800 code.
Hansel's code is proven to work, as it's the software engine of the Altair-Duino.
Now, Altair 101 runs Kill the Bit, sample assembler programs I wrote, and MITS 4K Basic.
My assembler program is great for assembling programs into byte code and uploading to the Altair101a.
I used the assembler to programming a modified version the program GALAXY80.
GALAXY80 is simple variation of the classic Altair 8800 Star Trek game.

Simulator models:
+ Altair101a: for running on a standalone Mega or Due board, no front panel.
+ Altair101f: Full model that can work with a hardware front panel and other other addons.
+ Both models can be interacted with, using a serial program.
    For example, the Arduino IDE, "screen" from a Mac, Putty on a Windows computer.

The Altair101f model, includes the integration of modern I/O components:
+ SD card module to save and load program memory,
+ Real time clock which is used when displaying the time and date,
+ MP3 player and external amp to play sounds and music.

--------------------------------------------------------------------------------
My [steps](READMEhistory.md) to build the machine.

[User guide](READMEguideCli.md) to use the machine from command line.

[User guide](READMEguideFrontPanel.md) to use the machine from a front panel.

--------------------------------------------------------------------------------
## Background

This project started late October 2019 after seeing an Altair 8800 as a background prop in the UK TV series, IT Crowd.
It re-sparked my interest in the Altair 8800.
Online, I found information on the Altair 8800 and that there were a number of clones.
I began with a studying the teaching videos on how to use an Altair 8800.
The [first video](https://www.youtube.com/watch?v=suyiMfzmZKs) was a great intro to using the Altair front panel.
I could follow what he was talking about, and I wanted to try it.

There was video of the 8-Bit Guy talking about what his dream computer machine would be.
Not only is a great design concept, it's.
Here are the videos about the 8-Bit Guy's dream computer
[Part I](https://www.youtube.com/watch?v=ayh0qebfD2g) and
[Part II](https://www.youtube.com/watch?v=sg-6Cjzzg8s)

The 8-Bit Guy has some great ideas on how to design and build a retro computer.
The 2 fundemental guidelines are practical philosophy for everything retro that I build:
+ Keep what was enjoyable from the past.
+ Update with the modern components to avoid the difficulties of the past.

What I liked about the Altair 8800:
+ Front panel layout with LED lights and toggles.
+ Assembly programming and running assembler programs.
+ Learning a new system like it's 1975.
+ To know what it would have been like to have one, if I had bought one in 1975.

I don't want to deal with:
+ Old storage system: floppies and tape.
+ Old boards, heavy bulky machines.
+ Reliability and getting an old system up and running.
+ Difficulty interfacing with modern devices.

What I wanted new:
+ SD card storage.
+ Extend it's functionality to make it useful, not just a novelty.
+ A modern accurate clock module.
+ An MP3 player. I opted for the DFPlayer.
+ Cross assembler. Write and assemble code on my laptop, then upload and run on the machine.
+ Simple serial communications with a laptop.
+ Multi-function uses for the LED lights and toggles, such as display the time and which MP3 song is playing(index number).

Possible future long term funcationality:
+ Programming in Basic.
+ Running CPM.

--------------------------------------------------------------------------------
## Harware Development

A phase approach was mentioned and it's a favorite of mine when doing large projects.
+ I started simple by developing the basic hardware functions such as switch logic and multiple LED light display.
+ Once the building blocks were tested, I selected a final microprocessor.
I had started with a Nano for unit testing. Due to Nano pin and memory limitations and my extended requirements, I moved to a Mega.
+ I built a prototype on a clipboard which is my tablet version.

My Altair 101 development machine, the steampunk tablet version.

<img width="300px"  src="AltairSteampunk.jpg"/>

Desktop model phase.
+ With the core component integrations tested, I build the core desktop hardware version.
+ With the desktop module, I designed part placements in the case and I refined the wiring.
+ Then I started to use it. Debugging and software enhancements were a daily task.

Now I have a desktop machine I was using on a daily bases.

<img width="600px"  src="AltairDesktop01a.jpg"/>

--------------------------------------------------------------------------------
## Software Development

--------------------------------------------------------------------------------
Cheers