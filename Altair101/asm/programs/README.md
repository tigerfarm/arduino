--------------------------------------------------------------------------------
# Writing Altair 101 Assembler Programs

I have moved into the program development stage.
I wrote an assembler program that runs my laptop, that assembles programs into byte code.
The assembler can also upload the code to the dev machine over a serial port.
The dev machine can run the programs with, or without,
detail step logging messages that are displayed on my laptop over a serial port.

My first programs are to test, study, and demonstrate the implemented Intel 8080/8085 opcodes.
I have also started to develop programs to demonstrate the Altair 101 abilities.
To prove that Altair 101 is a true emulator, I can assemble and run the classic program, Kill the Bit.
That was a celebrated accomplishment after months of work.

### Opcode Test Program Development

Test each opcode using various parameter types to study how to use the opcodes in a program.
Document the details to show what can be used in a program.

The programs are ordered, where opcodes are tested and then used in later programs.
For example, opMvi.asm is before opInrDcr.asm because MVI is used to initialize of opInrDcr.asm register values.
````
Program                 Opcodes tested, and test details.
-------                 -------------------------------------
pLoop.asm               JMP : Jump to a label. The first program tested.
                        NOP : Used as a spacer.
opJmp.asm               JMP : Jump to label or to a decimal or hex address.
                        Test: out 3, to print register A to the Arduino serial port.
opMvi.asm               MVI : Move immediate values to registers, using immediate types: decimal, hex, character (eg. 'a'), and EQU value.
                        HLT : Halt and restart.
                        EQU : Used to set an immediate value.
                        Program length = 72 bytes.
                        Because the serial read buffer is 64 bytes, I had to add "Thread.sleep(30);" in the Java program to slow the upload.
opInrDcr.asm            INR : Increment value in the registers: A,B,C,D,E,H,L.
                        DCR : Decrement value in the registers: A,B,C,D,E,H,L.
opMov.asm               MOV : Move values between the registers: A,B,C,D,E,H,L.
opCpi.asm               CPI : Compare immediate decimal, or an EQU value, with register A.
                        JZ
                        JNZ
                        JC
                        JNC
opCmp.asm               CMP : Compare register A and registers: B,C,D,E,H,L, and M. Register M, is memory data, H:L.
                            Register data == A, set Zero bit to 1. Carry bit = 0.
                            Register data != A, set Zero bit to 0.
                            Register > A, Carry bit = 1.
                            Register < A, Carry bit = 0.
                        JZ  Jump to a, if zero bit equals 1, flag is set.
                        JNZ Jump to a, if Zero bit equals 0, flag is not set.
                        JC  Jump to a, if Carry bit equals 1, flag is set.
                        JNC Jump to a, if Carry bit equals 0, flag is not set.
                        + Program has over 256 bytes, and works fine.

opOra.asm               ORA, OR with register A and registers: B,C,D,E,H,L, and M.
                        This program requires, LOG_MESSAGES not defined, because it outputs messages, example: "+ Success: ORA".
                        Program length = 255.

opAni.asm
opRlcRrc.asm
opAdiSui.asm

opDad.asm
opInx.asm
opLdax.asm
opShld.asm

opCallRet.asm
opLdaSta.asm

opOut.asm
opInOut.asm

pKillTheBit.asm
pSenseSwitchInput.asm
printDbString.asm
````

## Background Development

During the first early phase, programs were hard coded bytes in a memory array, in the processor program, Processor.ino.

My first test program, a jump loop.
````
// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
                    // Memory address:
  0303, 0006, 0000, // 0 1 2
  0000, 0000, 0000, // 3 4 5
  0303, 0000, 0000  // 6 7 8
};
````

In the second phase, I began writing assembler commands beside the byte arrays.
This sped up my program development.
I also began using program labels, my first assembler directive.

I complied sample programs into the file, ProcessorPrograms.md.
````
// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
                    // Start:
  0303, 6, 0,       // jmp 4
  0000,             // nop
  0303, 0, 0        // jmp Start
};
````

It was slow and difficult to write simple programs.
Re-adjusting jumps was a major issue without labels.
I needed an assembler program. But I couldn't find one online.

In the third phase, this phase, I'm writing an assembler program to convert assembler programs into byte arrays.
I then copy the assembler output, the byte arrays, into the Altair 101 process program, Processor.ino.
I run the assembled program in the processor program where I further debug:
+ The assembler program,
+ How the processor program processes opcodes,
+ And debug the assembler program itself.

As the third phase ends, I'm enter the program development phase.
The assembler program has become reliable and complete enough for me to write the programs I want to write.
Rather than debugging the processor or assembler programs,
I write programs to study and test the capabilities and uses of each opcode.
This leads to the writing of programs to demonstrate the capabilities of the Altair 101.

The program development phase will include the ability to upload byte programs to the 101.
The new steps will be:
+ Write assembler programs on my laptop.
+ Assemble the programs into a byte code file; no longer using byte array output.
+ Use a new serial program (in development) to upload the byte file to the processor.
+ The Altair 101 processor can then run the program, and optionally, store the program into a file for future use.

--------------------------------------------------------------------------------
## Program notes

--------------------------------------------------------------------------------
Cheers