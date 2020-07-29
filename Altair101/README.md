--------------------------------------------------------------------------------
# Altair 101 Computer
  
I have designed and built an Altair 8800 computer emulator.
that emulates the basic Altair 8800 hardware functionality from 1975.
The mother board, CPU, and RAM, is an Arduino Mega microcontroller.
The case is from the [Adwater & Stir emulator kit](https://www.adwaterandstir.com/product/altair-8800-emulator-kit/).
LED lights, toggles, chips, and other components are from China factories, bought on [eBay](https://eBay.com).
Together, it executes and emulates more than enough of the 8080/8085 microprocessor machine instructions (opcodes),
to run the classic program, Kill the Bit. 
Running Kill the Bit, was major milestone as it's the quasi essential Altair 8800 demonstration program.

To the base model, I added modern I/O components:
an SD card module to save and load program memory,
a real time clock which is used when displaying the time and date,
and an MP3 player and external amp to play sounds and music.

The project started late October 2019 after seeing an Altair 8800 as a background prop in the UK TV series, IT Crowd.
It re-sparked my interest. Online, I found information on the Altair 8800 and that there were a number of clones.
I began with a studying the videos.
The [first video](https://www.youtube.com/watch?v=suyiMfzmZKs) was a great intro to using the Altair front panel.
I could follow what he was talking about. I wanted to try it.

There was video of the 8-Bit Guy talking about what his dream computer machine would be.
It had some great tips: keep what was enjoyable from the past and update with the modern components to avoid the difficulties of the past.
Not only is a great design concept, it's practical philosophy for everything.
Building the 8-Bit Guy's dream computer [Part I](https://www.youtube.com/watch?v=ayh0qebfD2g) and
[Part II](https://www.youtube.com/watch?v=sg-6Cjzzg8s)

What I liked about the Altair 8800:
+ Front panel layout with LED lights and toggles.
+ Assembly programming and running assembler programs.
+ Learning a new system like it's 1975.
+ To know what it would have been like to have one then.

I don't like the original Altair 8800:
+ Old storage system: floppies and tape.
+ Old boards, heavy bulky machines.
+ Reliability and getting up and running.
+ Difficulty interfacing with modern devices.

What I want new:
+ SD card storage.
+ Extend it's functionality to make it useful, not just a novelty.
+ A modern accurate clock module.
+ An MP3 player, DFPlayer.
+ Cross assembler. Write and assemble code on my laptop, then upload and run on the machine.
+ Simple serial communications with a laptop.
+ Multi-function uses for the LED lights and toggles, such as display the time.

Not interested:
+ Programming in Basic.
+ Running CPM.

A phase approach was mentioned and it's a favorite of mine when doing large projects.
+ I started simple by development the basic functions such as switch logic and multiple LED light display.
+ Once the building blocks were tested, I selected a final microprocessor.
I had started with a Nano for unit testing. Due to pin and memory requirements I'm now using a Mega.
+ I built a prototype on a clipboard which is my tablet version.
+ With the core components integration tested, I build the desktop version.
+ With the desktop module, I needed part placements in the case and I refined the wiring of the MP3 player
+ Then I started to use it. Debugging and software enhancement were a daily task.

Now I have a desktop machine I use on a daily bases.

--------------------------------------------------------------------------------
My [steps](READMEhistory.md) to build the machine.

[User guide](READMEuserGuide.md) to use the machine.

--------------------------------------------------------------------------------
Since my Altair 101 development machine is working properly, I can show my steampunk tablet to the world.
Time to generate videos.

<img width="300px"  src="AltairSteampunk.jpg"/>

--------------------------------------------------------------------------------
Cheers