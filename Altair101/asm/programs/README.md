--------------------------------------------------------------------------------
# Writing Altair 101 Assembler Programs

I wrote an assembler program that runs my laptop. It assembles source programs into machine byte code.
Then, I use the assembler program to upload the code to the Altair 101 machine over a serial port.
The machine receives the code and loads it into the emulator's memory.

My first programs are to test, study, and demonstrate the implemented Intel 8080/8085 opcodes.
I have also started to develop programs to demonstrate the Altair 101 abilities.
To prove that Altair 101 is a true emulator, I can assemble and run the classic program, Kill the Bit.
That was a celebrated accomplishment after months of work.

Altair-Duino forum:
https://groups.google.com/forum/#!forum/altair-duino

## Setup to Develop and Run Programs

Steps:
````
On the laptop, go to the assembler program directory.
cd /Users/<userid>/Projects/arduino/Altair101/asm

Run assembler program and list programs.
$ java -jar asm.jar 
> ls
+ -------------------------------------
+ Directory listing for: programs
+ Program Directory = /Users/dthurston/Projects/arduino/Altair101/asm/programs
++ opAdd.asm
++ opAddDemo.asm
>

Select the program to work on and assemble it into byte code.
> file opAdd.asm
+ Program source file name: opadd.asm
+ Program full file name: programs/opadd.asm
+ Machine byte code file name: p1.bin
> asm
+ -------------------------------------
+ Print and parse the program: opadd.asm:
++ parseLine, part1|jmp| theRest|Start|
...
++      60:00111100: 00000101 : 05:005 > lb: 5
++      61:00111101: 00000000 : 00:000 > hb: 0
+ End of list.
+ Machine code file created:  p1.bin
> 

Power on the Altair 101.
Start the Arduino IDE. This is used to monitor the output from the machine.
From the IDE menu, select Tools/Serial Monitor. The machine will reboot.
The monitor displays the machine log messages.
+++ Setup.
+ AUX device toggle switches are configured for input.
...
+ HLT, program halted.
+ runProcessorWait()

Put the machine in download mode, ready to receive the assembled byte code.
+ Set the Sense switches to zero and flip AUX2 down.

Upload from the laptop to the machine.
> upload
+ -------------------------------------
+ Write to the serial port, the program file: p1.bin:
+ Serial port is open.
+ Write to serial port. Number of bytes: 62 in the file: p1.bin
11000011 0000011 ...
...
00000101 00000000 
+ Serial port is closed.
>

The machine will automatically return to the processor mode and is ready to run the program.
EXAMINE bytes, if you like.
Flip RUN to run the program.
````

### Opcode Test Programs

Demonstration programs.
````
Program                 Filename#  Opcodes tested, and test details.
-------                 ---------  --------------------------
                        00000000   NOPs

pKillTheBit.asm         00000011   Famous demo game program. Address 7 is the speed,
                                        00 000 101 : default.
                                        00 010 000 : Faster.
pBootPlay.asm           00001111   Play MP3 files, halting between each. I sometimes use it when the machine boots.

programList.asm         00000100   List programs on the LCD.
opAddDemo.asm           00000101   ADD : Add content of address 1 and 3, and store the answer in address 64.
                        00001000   Jump program that turns on all data byte lights.
opOutLcdOffOn.asm       00000001   LCD backlight on, or backlight off.
                        00000010   Jump program
````

Test each opcode using various parameter types to study how to use the opcodes.
Document the details to show what options work.

The programs are ordered, where opcodes are tested and then used in later programs.
For example, opMvi.asm is before opInrDcr.asm because MVI is used to initialize of opInrDcr.asm register values.
````
Program                 Opcodes tested, and test details.
-------                 -------------------------------------
pLoop.asm               JMP : Jump to a label. The first program tested.
                        NOP : Used as a spacer.
opJmp.asm               JMP : Jump to label or to a decimal or hex address.
                        OUT 3 : Print register A to the Arduino serial port.
opMvi.asm               MVI : Move immediate values to registers, using immediate types: decimal, hex, character (eg. 'a'), and EQU value.
                        HLT : Halt and restart.
                        EQU : Used to set an immediate value.
                        Program length = 72 bytes.
                        Because the serial read buffer is 64 bytes,
                            I had to add "Thread.sleep(30);" in the Java program to slow the upload.
opInrDcr.asm            INR : Increment value in the registers: A,B,C,D,E,H,L.
                        DCR : Decrement value in the registers: A,B,C,D,E,H,L.
opMov.asm               MOV : Move values between the registers: A,B,C,D,E,H,L.
opCpi.asm               CPI : Compare immediate decimal, or an EQU value, with register A.
                            Register data == A, set Zero bit to 1. Carry bit = 0.
                            Register data != A, set Zero bit to 0.
                            Register > A, Carry bit = 1.
                            Register < A, Carry bit = 0.
                        JZ  Jump to a, if zero bit equals 1, flag is set.
                        JNZ Jump to a, if Zero bit equals 0, flag is not set.
                        JC  Jump to a, if Carry bit equals 1, flag is set.
                        JNC Jump to a, if Carry bit equals 0, flag is not set.

opCmp.asm               CMP : Compare register A and registers: B,C,D,E,H,L, and M. Register M, is memory data, H:L.
                        JZ
                        JNZ
                        JC
                        JNC
                        + Program has 400+ bytes which is over 256 bytes, and works fine.

opCallRetShort.asm      CALL : words with label or a number.
                        RET : returns properly.
opCallRet.asm           CALL and RET : work fine.
opPushPop.asm           PUSH RP : Push a register pair (RP) onto the stack: B:C, D:E, H:L. To do, push flags.
                        Pop : Pop a register pair (RP) from the stack: B:C, D:E, H:L. To do, pop flags.
opLdaSta.asm            LDA : works with an address number and label.
                        STA : works with an address number and label.
                        LXI : load an address (value of Addr1) into H:L.
pLdaSta.asm             STEP control light testing of LDA and STA.
                        Status lights are tested and working correctly.
opLdax.asm              LDAX : Load register A with data value from address B:C or D:E.
opOut.asm               OUT pa : Demonstrate the various output options.
                        DB character string declaration, then print it.
printString.asm         OUT : Subroutine using OUT to print DB variable strings to the serial monitor.
                        sPrint : Subroutine to print a string.
                        sPrintln : Subroutine to print a string and finish with a new line character.
opInSwitches.asm        IN : Get and process a data byte from sense switches.
                        HLT : is used to stop the process and wait for the switches to be set.
                        RUN switch : used to continue the process.
                        Basically, the process stops. Sense switches are set.
                        Flip the RUN switch (similar return key) to continue.
opAdiSui.asm            ADI : Add immediate number to register A. Tested with decimal and EQU values.
                        SUI : Subtract immediate number from register A.
opAdi.asm               ADI : Add immediate number to register A. Tested with decimal and EQU values.
                        Simple program to demo adding 2 numbers.
                        Numbers can be easily changed using the Examine and Deposit.
                        Sum can viewed using Examine.
opAdd.asm               ADD : Add register data: B,C,D,E,H,L, and M(H:L); to register A.

opOutLcd.asm            OUT 1 : Output characters to LCD screen, and output screen options: backlight on/off and clear screen.

-------------------------------
Untested programs

opIxi.asm               LXI : test to load H:L with an address from a number or label.
opShld.asm              SHLD a : L -> (address a); H -> (address a+1)

opOra.asm               ORA : Register A, OR'ed with registers: B,C,D,E,H,L, and M.
                        This program requires, LOG_MESSAGES not defined, because it outputs messages, example: "+ Success: ORA".
                        Program length = 255.
opAni.asm               ANI : AND and immediate value with register A.
opXra.asm               XRA : Register A, Exclusive OR'ed with registers: B,C,D,E,H, and L.
opRlcRrc.asm            RLC : Rotate (shift) register A left. Wrap the left most, to the first.
                        RRC : Rotate (shift) register A right. Wrap the right most, to the last.

opDad.asm               DAD : Add register pair(RP: B:C or D:E) to H:L. Set carry bit.
opInx.asm               INX : Increment a register pair: B:C, D:E, H:L.

opImmediate.asm         Using various types of immediate values, with various opcodes.
````

--------------------------------------------------------------------------------
## Background Development Stages

In the first phase, programs were hard coded bytes in a memory array of the processor program: Processor.ino.

My first test program, a jump loop.
````
// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
  // Program bytes: // Memory address:
  0303, 0006, 0000, // 0 1 2 Jump to address 6.
  0000, 0000, 0000, // 3 4 5 NOPs
  0303, 0000, 0000  // 6 7 8 Jump to address 0.
};
````
The first program proved my basic program structure.
I could next add more opcodes and improve the fundamental program.

I began writing assembler commands beside the byte array values.
This sped up my program development and gave structure to my assembler programs as I was re-learner to write assembler programs.
````
// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
                    // Start:
  0303, 6, 0,       // jmp 4
  0000,             // nop
  0303, 0, 0        // jmp Start
};
````

I began storing sample programs into ProcessorPrograms.md.

It was slow and difficult to write simple programs.
Re-adjusting jumps was a major issue without labels.
I needed an assembler program. But I couldn't find one online.
I began writing an assembler program to convert assembler programs into byte arrays.

I would copy the byte array output into the processor program, Processor.ino.
The Arduino IDE would compile the Processor.ino program and upload it to the Arduino board.
I ran the assembled array program where I further debugged:
+ The assembler program,
+ The running of the opcodes in the processor program,
+ And debug the assembler program itself.

When the assembler program was reliable enough, I changed to output machine code bytes into a file, no longer into an array.
I added a serial module to the Altair 101.
I added an upload function to the assembler program and a download function to the processor program.
This allows writing and assembling on the laptop.
Then upload the bytes into the Altair 101 program memory.
The Altair 101 would then run the program.
Optionally I could store the program into an SD card file.
Later, I could load the program from the card and run it.

Now, I write programs to study and test the capabilities and uses of each opcode.
And, I write programs to demonstrate the capabilities of the Altair 101.
Today, I wrote a program to play MP3 files when the machine boots.

--------------------------------------------------------------------------------
Cheers