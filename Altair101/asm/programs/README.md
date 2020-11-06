--------------------------------------------------------------------------------
# Writing Altair 101 Assembler Programs

I wrote an assembler program that runs my laptop. It assembles source programs into machine byte code.
The assembler program has the option to upload the byte code to the Altair 101 machine over a USB serial port.
The machine receives the bytes and loads them into the emulator's memory.

My first programs were to test, study, and demonstrate the implemented Intel 8080/8085 opcodes.
I have also wrote programs to demonstrate the Altair 101 abilities.
To prove that Altair 101 is a true emulator, I assembled and ran the classic Altair 8800 program, Kill the Bit.
That was a celebrated accomplishment after months of work.

## Steps to Develop and Run Programs

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
++ pAdiAddDemo.asm
...
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
From the IDE menu, select Tools/Serial Monitor. The Altair 101 machine will reboot.
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

### Demonstration Programs Stored on the SD Card

List:
````
Program                 Filename#  Opcodes tested, and test details.
-------                 ---------  --------------------------
                        00000000   NOPs

pKillTheBit.asm         00000011   Famous demo game program. Address 7 is the speed,
                                        00 000 101 : default.
                                        00 010 000 : Faster.
pBootPlay.asm           00001111   Play MP3 files, halting between each. I sometimes use it when the machine boots.

programList.asm         00000100   List programs on the LCD.
pAdiAddDemo.asm         00000101   ADD : Add content of address 1 and 3, and store the answer in address 64.
                        00001000   Jump program that turns on all data byte lights.
opOutLcdOffOn.asm       00000001   LCD backlight on, or backlight off.
                        00000010   Jump program
````

### Opcode Test Programs

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
opCpi.asm               CPI #: Compare immediate number(#) and an EQU value, with register A.
                            # = A: Zero bit = 1. Carry bit = 0.
                            #!= A: Zero bit = 0.
                            # > A: Zero bit = 0, Carry bit = 1.
                            # < A: Zero bit = 0, Carry bit = 0.
                        JZ  Jump to a, if zero  bit equals 1, flag is set.
                        JNZ Jump to a, if Zero  bit equals 0, flag is not set.
                        JC  Jump to a, if Carry bit equals 1, flag is set.
                        JNC Jump to a, if Carry bit equals 0, flag is not set.
opCmp.asm               CMP : Compare register A with the other registers: B,C,D,E,H,L, and M. Register M, is memory data, H:L.
                        JZ, JNZ, JC, JNC: same as CPI.
                        + Program has 600+ bytes (16 bit address) which is well over 256 bytes (8 bit address), and works fine.
opCallRetShort.asm      CALL : words with label or an immediate number address.
                        RET : returns properly.
opCallRet.asm           CALL and RET : work fine.
                        Single call and return.
opCallMultiple.asm      Multiple call and returns, call with calls in the call. I.E. stacked calls.
opPushPop.asm           PUSH RP : Push a register pair (RP) onto the stack: B:C, D:E, H:L. Push register A and flags.
                        Pop : Pop a register pair (RP) from the stack: B:C, D:E, H:L. To do, pop flags.
opLdaSta.asm            STA a: Store register A data to an address a(hb:lb).
                        LDA a: Load register A with the data at the address a(hb:lb).
                        STA and LDA : work with an address number and label.
                        LXI : load an address (value of Addr1) into H:L.
opLdax.asm              LDAX : Load register A with data value from address B:C or D:E.
                        Procedures: PrintTestCounter and PrintDigit.
                        Increment a counter variable (DB).
                        Uses opcodes: LDAX, MVI, MOV, LDA, STA, OUT, HLT, CALL and RET.
opInSwitches.asm        IN : Get and process a data byte from sense switches.
                        HLT : is used to stop the process and wait for the switches to be set.
                        RUN switch : used to continue the process.
                        Basically, the process stops. Sense switches are set.
                        Flip the RUN switch (similar return key) to continue.
-------------------------------
I now have a rich enough opcode set and samples, to add subroutines to the programs.

opAdiSui.asm            ADI : Add immediate number to register A. Tested with decimal and EQU values.
                        SUI : Subtract immediate number from register A.
                        Subroutines: NewTest, PrintDigit, PrintStr, PrintStrln, and Error.
                        Counter to display the test number.
opLxi.asm               LXI : load address values from a number or label into register pairs: B:C, D:E, and H:L.
                        Echo the register pair address and the data at that 16 bit address.
opShld.asm              SHLD a : L -> (address a); H -> (address a+1)
opAni.asm               ANI : AND an immediate value with register A.
                        Example: 11101101 AND 10110000 = 10100000
opOra.asm               ORA : Register A, OR'ed with registers: B,C,D,E,H,L, and M. Result each time is stored in register A.

+++ Assembler issue when the 2 are one after the other, and the program is over 255 bytes:
        OrStr       db  '\n--- OR ---'
        EqualStr    db  '\n=========='

pPrintDigit.asm         CALL procedure to print a digit that is in register A.
pPlaySoundEffects.asm   Play sound effects: on(regA=1) or off(regA=0). OUT 69
printString.asm         OUT : Subroutine using OUT to print DB variable strings to the serial monitor.
                        PrintStr : Subroutine to print a string.
                        PrintStrln : Subroutine to print a string and finish with a new line character.

-------------------------------
Untested programs for pong.asm

opRlcRrc.asm            RLC : Rotate (shift) register A left. Wrap the left most, to the first.
                        RRC : Rotate (shift) register A right. Wrap the right most, to the last.
opDad.asm               DAD : Add register pair(RP: B:C or D:E) to H:L. Set carry bit.

-------------------------------
Needs testing:

pLdaSta.asm             STEP control light testing of LDA and STA.
                        Status lights are tested and working correctly.

opOut.asm               OUT pa : Demonstrate the various output options.
                        DB character string declaration, then print it.

opAdi.asm               ADI : Add immediate number to register A. Tested with decimal and EQU values.
                        Simple program to demo adding 2 numbers.
                        Numbers can be easily changed using the Examine and Deposit.
                        Sum can viewed using Examine.
opAdd.asm               ADD : Add register data: B,C,D,E,H,L, and M(H:L); to register A.

opOutLcd.asm            OUT 1 : Output characters to LCD screen, and output screen options: backlight on/off and clear screen.

opXra.asm               XRA : Register A, Exclusive OR'ed with registers: B,C,D,E,H, and L.
opInx.asm               INX : Increment a register pair: B:C, D:E, H:L.
opImmediate.asm         Using various types of immediate values, with various opcodes.
````

--------------------------------------------------------------------------------
## Background Development Stages

In the first phase programs were hard coded bytes in a memory array of the processor program: Processor.ino.

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
I could next add more opcodes and improve Processor.ino.

I began writing assembler commands beside the byte array values.
This sped up my program development and gave structure to my assembler programs as I was re-learning how to write assembler programs.
````
// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
                    // Start:
  0303, 6, 0,       //      jmp 6
  0000,             //      nop
  0303, 0, 0        //      jmp Start
};
````

I began storing sample program arrays into ProcessorPrograms.md.
The samples could copied back into Process.ino and run.

It was slow and difficult to write simple programs.
Re-adjusting jumps was a major issue without labels.
I needed an assembler program. But I couldn't find one online.
I began writing an assembler program to convert assembler instructions into byte arrays.

I would copy the byte array output into the processor program, Processor.ino.
The Arduino IDE would compile the Processor.ino program and upload it to the Arduino board.
I ran the assembled array program where I further debugged:
+ The assembler program,
+ The running of the opcodes in the processor program,
+ And debug the assembler program itself.

When the assembler program was reliable enough, I changed to output machine code bytes into a file, no longer into an array.
I added a serial module to the Altair 101.
I added an upload function to the assembler program and a download function to the processor program.
This allows writing and assembling on the laptop, then uploading the bytes into the Altair 101 program memory.
The Altair 101 would then run the program.
Next, was to store the byte code into an SD card file.
Then, load the program from the card into emulator memory, and run it.

I had achieved my goal of being able to write assembler programs, run them, and save them to loaded and run later.

Now, I write programs to study and test the capabilities and uses of each opcode.
I write programs to demonstrate the capabilities of the Altair 101.
I write a program to play MP3 files, for example to play HAL sound bytes when the machine boots.

Altair-Duino forum:
https://groups.google.com/forum/#!forum/altair-duino

--------------------------------------------------------------------------------
Cheers