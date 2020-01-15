--------------------------------------------------------------------------------
# Altair 101 Assembler Programs

The programs in this directory are used to test opcodes and develop programming methods.

First phase programs were hard coded bytes in a memory array, in Processor.ino.

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
This aided my program development.
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

As I wrote more programs, my assembler skills increased.
However it was slow and difficult to write simple programs.
Readjusting jumps was a major issue without labels.
I needed an assembler program. But I couldn't find one online.

The third phase, this phase, is writing an assembler program to convert assembler programs into byte arrays.
I then copy the assembler output, the byte arrays, into Processor.ino.
I run the assembler program in Processor.ino.
This is where I further debug the assembler program,
debug the opcode processing of Processor.ino, and debug the assembler program itself.

As the third phase ends, the assembler program is becoming reliable and complete enough for me to write the programs I want to write.
I'm enter the program development phase.
Rather than debugging the processor or assembler programs,
I'm writing programs to demonstrate the capabilities of the Altair 101.

--------------------------------------------------------------------------------
## Program notes

--------------------------------------------------------------------------------
Cheers