--------------------------------------------------------------------------------
# Writing Altair 101 Assembler Programs

Tested:
````
Program             Opcodes tested, and other tests.
programs/pLoop.asm      JMP
programs/opMvi.asm      MVI with immediate types: decimal, hex, character (eg. 'a'), and EQU value.
                        Program length = 72 bytes.
                        Because the serial read buffer is 64 bytes, I had to add "Thread.sleep(30);" in the Java program to slow the upload.
programs/opMov.asm      MOV with all combination of registers: A,B,C,D,E,H,L.
                        INR with all combination of registers: A,B,C,D,E,H,L.
                        NOP
programs/opOra.asm      ORA, OR with register A and registers: B,C,D,E,H,L and M.
                        Test: out 3, to print messages.
                        This program requires, LOG_MESSAGES not defined, because it outputs messages, eg. "+ Success: ORA".
                        Program length = 255.
programs/opOut.asm
programs/opRlcRrc.asm
programs/opAdiSui.asm
programs/opAni.asm
programs/opCallRet.asm
programs/opCmp.asm
programs/opCpi.asm
programs/opInOut.asm
programs/opLdaSta.asm

programs/pKillTheBit.asm
programs/pSenseSwitchInput.asm
programs/printDbString.asm
````
The programs in this directory are used to test and demonstrate the use of each opcode.
I'm also developing programs to demonstrate the Altair 101 abilities.

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