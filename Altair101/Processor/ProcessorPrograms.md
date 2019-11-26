# Development Programs for the Altair 101

Following are my machine code programs.

I found an assembler that I need to try:
+ Video showing how to [download and make](https://www.youtube.com/watch?v=oO8_2JJV0B4) the VASM assembler.
+ vasm [home page](http://sun.hasenbraten.de/vasm/),
    [source](http://sun.hasenbraten.de/vasm/index.php?view=relsrc)

After downloading and unzipping the source, run the following:
````
cd /Users/.../work/vasm
$ make CPU=Z80 SYNTAX=oldstyle
...
$ ./vasmZ80_oldstyle
...
$ cp vasmZ80_oldstyle ../asm
$ cd ..
````

Sample Method Programs
+ [Loop](#loop)

### Opcode Test Programs

+ [Stack opcodes](#test-stack-opcodes) : Test stack opcodes: CALL, RET, PUSH, and POP.

+ [ANI](#test-opcode-ani) : AND # (immediate db) with register A.
+ [XRA](#test-opcode-xra) : Exclusive OR, the register(R) with register A.

+ [DAD](#test-opcode-dad) : Add register pair(RP, B:C or D:E) to H:L (16 bit add). And set carry bit.
+ [INX](#test-opcode-inx) : Increment a register pair (a 16 bit value): B:C, D:E, H:L.
+ [INR](#test-opcodes-inr-and-dcr) : Increment a register.
+ [DCR](#test-opcodes-inr-and-dcr) : Decrement a register.

+ [LDA](#test-opcodes-lda-and-sta) : Load register A with data from the address, a(hb:lb).
+ [STA](#test-opcodes-lda-and-sta) : Store register A, as data, to the address, a(hb:lb).
+ [LDAX](#test-opcode-ldax) : Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.
+ [LXI](#test-opcode-lxi) : Move the data at the address, a(lb hb), into register pair: B:C, D:E, or H:L.
+ [MVI and MOV](#test-opcodes-mvi-and-mov) : Move source register data, to the destination register.
+ [MVI](#test-opcode-mvi) : Move a number (#, db) to a register.
+ [SHLD](#test-opcode-shld) : Store L value to memory location: a(hb:lb). Store H value at: a + 1.

Other Test Programs,
+ [The Initial Programs that I started with](#the-initial-programs-that-i-started-with)
+ [Altair 8800 Pong](#altair-8800-pong-program)
+ [Altair 8800 Kill the Bit](#kill-the-bit-program)
+ [Other test programs](#other-test-programs-1)

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
--------------------------------------------------------------------------------
### Programing links

[8080 opcodes](ProcessorOpcodes.md)

[Bitwise, shift right](https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitshiftright/)

[Exclusive or (XOR)](https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitwisexor/)

--------------------------------------------------------------------------------
## Sample Method Programs

##### Loop
````
// -----------------------------------------------------------------------------
// Sample loop

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Start
  //
  //                //            ; --------------------------------------
  //                //            ; Initialize register values.
  //
  0041, 0000, 0000, // lxi h,Start  ; lxi_HL lb hb. Load into register H:L = 0000:0000.
  0176,             // mov M      ; Move the data in register M address(register address H:L), to register A.
  //
  0006, 0001,       // mvi b,1    ; Move db to register B.
  0016, 0002,       // mvi c,2    ; Move db to register C.
  0127,             // mov d,a    ; Move register a to register D.
  0036, 0003,       // mvi e,3    ; Move db to register E.
  //
  0343, 38,         // out 38     ; Print the Initialized register values.
  0166,             // hlt
  //
  //                //            ; --------------------------------------
  //                // Loop:      ; Change values and loop.
  0043,             // inx M      ; Increment register M(register address H:L).
  0176,             // mov M      ; Move the data in register M(register address H:L), to register A.
  0343, 38,         // out 38     ; Print the registers.
  0166,             // hlt
  0303, 14, 0000,   // jmp Loop   ; Jump to the Loop label address.
  0000              //            ; End.
};
````

--------------------------------------------------------------------------------
## Opcode Test Programs

Programs to test the functionality of specific opcodes.

-----------------------------------------------------------------------------
##### Test stack opcodes
````

// -----------------------------------------------------------------------------
// Program to test stack opcodes.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                //            ; Test stack opcodes.
  // CALL a    11001101 lb hb   Unconditional subroutine call.              (SP-1)<-PC.
  // RET       11001001         Unconditional return from subroutine        PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
  // PUSH RP   11RP0101 Push    register pair (B, D, or H) onto the stack.  (sp-2) <- RP;
  // POP RP    11RP0001 Pop     register pair (B, D, or H) from the stack.  RP <- (sp);
  //
  //                // Start:     ; Test stack opcodes:
  0303, 6, 0,       // jmp Test   ; Jump to bypass the subroutine and the halt command.
  //
  //                //            ; --------------------------------------
  //                //            ; Subroutine to increment register B.
  //                // IncrementB:
  B00000100,        // inr b      ; Increment register B.
  B11001001,        // ret        ; Return to the caller address + 1.
  //
  //                //            ; --------------------------------------
  //                // Halt:      ; Halt, address: 5.
  0166,             // hlt        ; Then, the program will halt at each iteration.
  //                // Test:
  //
  //                //            ; --------------------------------------
  0,                // NOP
  B00000110, 7,     // mvi b,7
  B11001101, 3, 0,  // call IncrementB
  0343, 30,         // out b      ; Print register B, value = 8.
  0,                // NOP
  //
  //                //            ; --------------------------------------
  //0RRR110 mvi
  B00000110, 1,     // mvi b,1    ; Move # to register B and C.
  B00001110, 2,     // mvi c,2
  //1RP0101 push
  B11000101,        // push b     ; Push register pair B:C onto the stack.
  //
  B00000110, 0,     // mvi b,0
  B00001110, 0,     // mvi c,0
  //1RP0001 pop
  B11000001,        // pop b      ; Pop register pair B:C from the stack.
  //
  //                //            ; --------------------------------------
  0,                // NOP
  B00010110, 3,     // mvi d,3
  B00011110, 5,     // mvi e,5
  //1RP0101 push
  B11010101,        // push d     ; Push register pair D:E onto the stack.
  //
  B00010110, 0,     // mvi d,0
  B00011110, 0,     // mvi e,0
  //1RP0001 pop
  B11010001,        // pop d      ; Pop register pair D:E from the stack.
  //
  //                //            ; --------------------------------------
  0,                // NOP
  B00100110, 7,     // mvi h,7
  B00101110, 11,    // mvi l,11
  //1RP0101 push
  B11100101,        // push h     ; Push register pair H:L onto the stack.
  //
  B00100110, 0,     // mvi h,0
  B00101110, 0,     // mvi l,0
  //1RP0001 pop
  B11100001,        // pop h      ; Pop register pair H:L from the stack.
  //
  //                //            ; --------------------------------------
  0,                // NOP
  //1RP0101 push
  B11000101,        // push b     ; Push register pair B:C onto the stack.
  B11010101,        // push d     ; Push register pair D:E onto the stack.
  B11100101,        // push h     ; Push register pair H:L onto the stack.
  //0RRR110 mvi
  B00000110, 0,     // mvi b,0
  B00001110, 0,     // mvi c,0
  B00010110, 0,     // mvi d,0
  B00011110, 0,     // mvi e,0
  B00100110, 0,     // mvi h,0
  B00101110, 0,     // mvi l,0
  0343, 38,         // out 38     ; Print the register values.
  //1RP0001 pop
  B11100001,        // pop h      ; Pop register pair H:L from the stack.
  B11010001,        // pop d      ; Pop register pair D:E from the stack.
  B11000001,        // pop b      ; Pop register pair B:C from the stack.
  0343, 38,         // out 38     ; Print the register values.
  //
  //                //            ; --------------------------------------
  0,                // NOP
  0303, 5, 0,       // jmp Halt   ; Jump back to the start halt command.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode ORA
````
// -----------------------------------------------------------------------------
// Program to test opcodes.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode ora.
  //                //            ; OR source register, with register A.
  //
  0303, 4, 0,       // jmp Test   ; Jump to bypass the halt.
  //                // Halt:
  0166,             // hlt        ; Then, the program will halt at each iteration.
  //                // Test:
  //
  // -----------------------------------------------------------------------------
  // ORA S     10110SSS          ZSPCA   OR source register with A
  //
  //1RRR110 mvi
  B00111110, 73,    // mvi a,73   ; Move # to register A:    01 001 001 = 73
  B00000110, 70,    // mvi b,70   ; Move # to register B:    01 000 110 = 70
  //0110SSS ora
  B10110000,        // ora b      ; OR register B, with register A. Answer: 01 001 111 = 79.
  //
  B00111110, 73,    // mvi a,73
  B00001110, 70,    // mvi c,70
  B10110001,        // ora c
  //
  B00111110, 73,    // mvi a,73
  B00010110, 70,    // mvi d,70
  B10110010,        // ora d
  //
  B00111110, 73,    // mvi a,73
  B00011110, 70,    // mvi e,3
  B10110011,        // ora e
  //
  B00111110, 73,    // mvi a,73
  B00100110, 70,    // mvi h,70
  B10110100,        // ora h
  //
  B00111110, 73,    // mvi a,73
  B00101110, 70,    // mvi l,70
  B10110101,        // ora l
  //
  B00111110, 73,    // mvi a,73
  B00100110, 0,     // mvi h,0
  B00101110, 0,     // mvi l,0    ; Register M address data = 11 000 011
  B10110110,        // ora m      ; OR data a register M address, with register A. Answer: 11 001 011.
  //
  // -----------------------------------------------------------------------------
  0303, 3, 0,       // jmp Start  ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode ANI
````
// -----------------------------------------------------------------------------
// Test opcode ani.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode ani.
  0303, 4, 0,       // jmp Test   ; Jump to bypass the halt.
  //                // Halt:
  0166,             // hlt        ; Then, the program will halt at each iteration.
  //                // Test:
  //
  // -----------------------------------------------------------------------------
  //ANI #     11100110 db       ZSPCA   AND immediate with A
  //
  //1RRR110 mvi                                        ORA S 10 110 SSS
  B00111110, 176,   // mvi a,176  ; Move # to register A:    10 110 000 = 176 which is ora b.
  //1100110 ani
  B11100110, 248,   // ani 248    ; AND # with register A:   11 111 000 = 248
  0343, 37,         // out 37     ; Print register A answer: 10 110 000 = 176 which identifies the opcode ora.
  //
  B00111110, 177,   // mvi a,177  ; Move # to register A:    10 110 001 = 177 which is ora c.
  B11100110, 248,   // ani 248    ; AND # with register A:   11 111 000 = 248
  0343, 37,         // out 37     ; Print register A answer: 10 110 000 = 176 which identifies the opcode ora.
  //
  B00111110, 178,   // mvi a,178  ; Move # to register A:    10 110 010 = 178 which is ora d.
  B11100110, 7,     // ani b      ; AND # with register A:   00 000 111 =   7
  0343, 37,         // out 37     ; Print register A answer: 00 000 010 =   2 which is the code for register d.
  //
  0343, 38,         // out 38     ; Print the register values.
  //
  // -----------------------------------------------------------------------------
  0303, 0003, 0000, // jmp Halt   ; Jump back to the halt statement.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode SHLD
````
// -----------------------------------------------------------------------------
// Test opcode shld.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode shld.
  //                //            ; Store register L to memory address hb:lb. Store register H to hb:lb + 1.
  //
  0303, 0006, 0000, // jmp Start  ; Jump to program Intialization.
  0000, 0000, 0000, //            ; These next 3 bytes (3,4,5) can be used for memory storage testing.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //                              ; Start at memory location: 6.
  B00101110, 4,     // mvi l,0
  B00100110, 5,     // mvi h,0
  0343, 36,         // out 36     ; Print register pair H:L and it's address data value.
  //
  // -----------------------------------------------------------------------------
  // shld a    00100010 lb hb    -  Store register L to memory address hb:lb. Store register H to hb:lb + 1.
  // The contents of register L, are stored in the memory address specified in bytes lb and hb (hb:lb).
  // The contents of register H, are stored in the memory at the next higher address (hb:lb + 1).
  //
  //0100010
  B00100010, 3, 0,  // shld 3     ; Store L value to memory location: 3(0:3). Store H value at: 3 + 1.
  // View the memory:
  B00111010, 3, 0,  // lda 3      ; Load register A from the address(hb:lb).
  0343, 37,         // out 37     ; Print register A.
  B00111010, 4, 0,  // lda 4      ; Load register A from the address(hb:lb).
  0343, 37,         // out 37     ; Print register A.
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode DAD
````
// -----------------------------------------------------------------------------
// Test opcode dad.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode dad.
  //                //            ; Add register pair to H:L. Set carry bit.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,0
  B00101110, 5,     // mvi l,0
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  // DAD RP 00RP1001  Add register pair to H:L. Set carry bit.
  //          00=BC   (B:C as 16 bit register)
  //          01=DE   (D:E as 16 bit register)
  //          10=HL   (H:L as 16 bit register)
  //
  //0RP1001
  B00001001,        // dad b      ; Add register pair B:C to H:L.
  0343, 36,         // out 36     ; Print register pair, H:L.
  B00011001,        // dad d      ; Add register pair D:E to H:L.
  0343, 36,         // out 36     ; Print register pair, H:L.
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode INX
````
// -----------------------------------------------------------------------------
// Test opcode inx.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode inx.
  //                //            ; INX RP 00RP0011 Increment register pair
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,0
  B00101110, 5,     // mvi l,0
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  // INX RP 00RP0011 Increment register pair
  //          00=BC   (B:C as 16 bit register)
  //          01=DE   (D:E as 16 bit register)
  //          10=HL   (H:L as 16 bit register)
  //
  //0RP0011
  B00000011,             // inx b      ; Increment register pair B:C.
  B00010011,             // inx d      ; D:E
  B00100011,             // inx m      ; Increment register M(register address H:L).
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode LDAX
````
// -----------------------------------------------------------------------------
// Test opcode ldax.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode ldax.
  //                //            ; Load register A with data value from address B:C or D:E.
  0303, 4, 0,       // jmp Test   ; Jump to bypass the halt.
  //                // Halt:
  0166,             // hlt        ; Then, the program will halt at each iteration.
  //                // Test:
  //
  // -----------------------------------------------------------------------------
  // ldax RP  00RP1010 - Load register A with indirect through BC(RP=00) or DE(RP=01)
  // ---------------
  //
  B00000110, 0,     // mvi b,0    ; Set registers, then move data to register A.
  B00001110, 0,     // mvi c,0    ; B:C data = octal 303
  //0RP1010
  B00001010,        // ldax b
  0343, 37,         // out 37     ; Print register A = 195 = 303 = 11000011
  //
  B00010110, 0,     // mvi d,0
  B00011110, 3,     // mvi e,3    ; D:E data = octal 166
  //0RP1010
  B00011010,        // ldax d
  0343, 37,         // out 37     ; Print register A = 118 = 166 = 01110110
  //
  // -----------------------------------------------------------------------------
  0303, 3, 0,       // jmp Halt   ; Jump back to beginning of the test.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode XRA
````
// -----------------------------------------------------------------------------
// Test opcode xra.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode xra. Register exclusive OR with register with A.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,4
  B00101110, 5,     // mvi l,5
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  //                        xra R    10 101 SSS  Register exclusive OR with register with A.
  //
  B00111110, 6,     // mvi a,6    ; Set register A, then do the exclusive OR.
  B10101000,        // xra b
  B00111110, 6,     // mvi a,6
  B10101001,        // xra c
  B00111110, 6,     // mvi a,6
  B10101010,        // xra d
  B00111110, 6,     // mvi a,6
  B10101011,        // xra e
  B00111110, 6,     // mvi a,6
  B10101100,        // xra h
  B00111110, 6,     // mvi a,6
  B10101101,        // xra l
  //
  0343, 38,         // out 38     ; Print the updated register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode LXI
````

// -----------------------------------------------------------------------------
// Test opcode lxi.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode lxi. Load register pair immediate.
  //                              ; Move a(hb:lb) into register pair RP, example, B:C = hb:lb.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,4
  B00101110, 5,     // mvi l,5
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  //
  B00000001, 0002,0001, // lxi b,513  ; lxi b,16-bit-address: 00 000 010 : 00 000 001 = 002 : 001
  B00010001, 0004,0021, // lxi d,1041 ; lxi b,16-bit-address. 00 000 100 : 00 010 001 = 004 : 021
  B00100001, 0006,0041, // lxi h,1569 ; lxi b,16-bit-address. 00 000 110 : 00 100 001 = 010 : 041
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcodes LDA and STA
````
// -----------------------------------------------------------------------------
// Test opcodes sta and lda.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcodes sta and lda.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //1RRR110
  B00111110, 6,     // mvi a,0    ; Move # to register A.
  //
  B00100110, 0,     // mvi h,0    ; Address for memory address testing.
  B00101110, 60,    // mvi l,60
  //
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  //
  B00111110, 6,     // mvi a,0    ; Move # to register A.
  0343, 37,         // out 37     ; Print register A.
  //
  0000,             // nop
  B00110010, 60, 0, // sta 60     ; Store register A's data to the address(hb:lb).
  0343, 36,         // out 36     ; Print memory address data value. "36" prints the register pair H:L and data at the address.
  //
  0000,             // nop
  B00111110, 0,     // mvi a,0    ; Move # to register A.
  B00111010, 60, 0, // lda 60     ; Load register A from the address(hb:lb).
  0343, 37,         // out 37     ; Print register A.
  //
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode INR and DCR
````
// -----------------------------------------------------------------------------
// INR: Increment opcode test.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Testing opcodes inr and dcr.
  // ------------------------------------------------------------------------------------------
  //                //            ; Initialize register values.
  //1RRR110
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,4
  B00101110, 5,     // mvi l,5
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  //0DDD100
  B00111100,        // inr a      ; Increment the register
  B00000100,        // inr b 
  B00001100,        // inr C
  B00010100,        // inr D
  B00011100,        // inr E
  B00100100,        // inr H
  B00101100,        // inr L
  0343, 38,         // out 38     ; Print the incrmented register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  //0DDD101
  B00111101,        // dcr a      ; Decrement the register
  B00000101,        // dcr b 
  B00001101,        // dcr C
  B00010101,        // dcr D
  B00011101,        // dcr E
  B00100101,        // dcr H
  B00101101,        // dcr L
  0343, 38,         // out 38     ; Print the decremented register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump to the beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcodes MVI and MOV
````
// -----------------------------------------------------------------------------
byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Start: Testing opcodes mvi, mov, inr.
  //                              ;   Also uses: lxi, OUT, hlt.
  //
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  //                //            ; Intialize register values.
  //
  //1RRR110
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,4
  B00101110, 5,     // mvi l,5
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  // mov d,S  01 DDD SSS   Move register to a register. 36 register to register combinations.
  //                 111 A
  //                 000 B
  //                 001 C
  //                 010 D
  //                 011 E
  //                 100 H
  //                 101 L
  //
  //1RRR110
  B00111110, 7,     // mvi a,7    ; Move # to register A.
  //1DDDSSS
  B01000111,        // mov b,a    ; 01 000 111  ; Move register a to each register.
  B00111100,        // inr a      ; Increment the register
  B01001111,        // mov c,a    ; 01 001 111
  B00111100,        // inr A
  B01010111,        // mov d,a    ; 01 010 111
  B00111100,        // inr A
  B01011111,        // mov e,a    ; 01 011 111
  B00111100,        // inr A
  B01100111,        // mov h,a    ; 01 100 111
  B00111100,        // inr A
  B01101111,        // mov l,a    ; 01 101 111
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  //1RRR110
  B00000110, 0,     // mvi b,0  ; Move # to register B.
  //1DDDSSS
  B01111000,        // mov a,b  ; Move register b to each register.
  B00000100,        // inr b 
  B01001000,        // mov c,b
  B00000100,        // inr b 
  B01010000,        // mov d,b
  B00000100,        // inr b 
  B01011000,        // mov e,b
  B00000100,        // inr b 
  B01100000,        // mov h,b
  B00000100,        // inr b 
  B01101000,        // mov l,b
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //1RRR110
  B00001110, 1,     // mvi c,1  ; Move # to register C.
  //1DDDSSS
  B01111001,        // mov a,c  ; Move register c to each register.
  B00001100,        // inr C
  B01000001,        // mov b,c
  B00001100,        // inr C
  B01010001,        // mov d,c
  B00001100,        // inr C
  B01011001,        // mov e,c
  B00001100,        // inr C
  B01100001,        // mov h,c
  B00001100,        // inr C
  B01101001,        // mov l,c
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //1RRR110
  B00010110, 2,     // mvi d,2  ; Move # to register D.
  //1DDDSSS
  B01111010,        // mov a,d  ; Move register d to each register.
  B00010100,        // inr D
  B01000010,        // mov b,d
  B00010100,        // inr D
  B01001010,        // mov c,d
  B00010100,        // inr D
  B01011010,        // mov e,d
  B00010100,        // inr D
  B01100010,        // mov h,d
  B00010100,        // inr D
  B01101010,        // mov l,d
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //1RRR110
  B00011110, 3,     // mvi e,3  ; Move # to register E.
  //1DDDSSS
  B01111011,        // mov a,E  ; Move register e to each register.
  B00011100,        // inr E
  B01000011,        // mov b,E
  B00011100,        // inr E
  B01001011,        // mov c,E
  B00011100,        // inr E
  B01010011,        // mov d,E
  B00011100,        // inr E
  B01100011,        // mov h,E
  B00011100,        // inr E
  B01101011,        // mov l,E
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //1RRR110
  B00100110, 4,     // mvi h,4  ; Move # to register H.
  //1DDDSSS
  B01111100,        // mov a,h  ; Move register h to each register.
  B00100100,        // inr H
  B01000100,        // mov b,h
  B00100100,        // inr H
  B01001100,        // mov c,h
  B00100100,        // inr H
  B01010100,        // mov d,h
  B00100100,        // inr H
  B01011100,        // mov e,h
  B00100100,        // inr H
  B01101100,        // mov l,h
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //1RRR110
  B00101110, 5,     // mvi l,5  ; Move # to register L.
  //1DDDSSS
  B01111101,        // mov a,l  ; Move register l to each register.
  B00101100,        // inr L
  B01000101,        // mov b,l
  B00101100,        // inr L
  B01001101,        // mov c,l
  B00101100,        // inr L
  B01010101,        // mov d,l
  B00101100,        // inr L
  B01011101,        // mov e,l
  B00101100,        // inr L
  B01100101,        // mov h,l
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // ------------------------------------------------------------------------------------------
  //
  0041, 0000, 0000, // lxi h,Start  ; lxi_HL lb hb. Load into register H:L = 0000:0000.
  0176,             // mov M,a    ; Move the data in register M(register address H:L), to register A.
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};
````
-----------------------------------------------------------------------------
##### Test opcode MVI
````
// -----------------------------------------------------------------------------
// Opcode mvi test program.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // BEG:       ; Start: Testing opcode mvi.
  //
  0343, 38,         // OUT 30     ; Print the intial register values.
  0166,             // HLT
  //
  // ------------------------------------------------------------------------------------------
  // mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
  // mvi A,#  00 111 110  0036
  // mvi B,#  00 000 110  0006
  // mvi C,#  00 001 110  0016
  // mvi D,#  00 010 110  0026
  // mvi E,#  00 011 110  0036
  // mvi H,#  00 100 110  0046
  // mvi L,#  00 101 110  0056
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  B00111110, 7,       // mvi A,7    ; Move db to register A.
  B00000110, 0,       // mvi B,0    ; Move db to register B.
  B00001110, 1,       // mvi C,1    ; Move db to register C.
  B00010110, 2,       // mvi D,2    ; Move db to register D.
  B00011110, 3,       // mvi E,3    ; Move db to register E.
  B00100110, 4,       // mvi H,4    ; Move db to register H.
  B00101110, 5,       // mvi L,5    ; Move db to register L.
  //
  0343, 38,         // OUT 30     ; Print the Intialized register values.
  0166,             // HLT
  //
  0000              //            ; End.
};
````

--------------------------------------------------------------------------------
### Other Test Programs

````
// -----------------------------------------------------------------------------
// I/O program.
// IN this program, terminal output is to the 1602 LCD.

0041, 26, 0,    // LXI M        ; M address is H:L registers. 26 is address of the first data byte.
//
//              // OSTATUS:     ; Check output status availability
0333, 0,        // IN STATUS    ; Input terminal output status
0346, 0200,     // ANI          ; AND register A with 0200.
0302, 3, 0,     // JNZ OSTATUS  ; If status is not ready, jump to OSTATUS and check again.
//
0176,           // MOV A,M      ; Move data at address M(H:L) to A.
0376, 0377,     // CPI          ; Compare A with 0377.
0312, ?, 0,     // JZ EOP       ; If stop code character, jump to halt the program.
//
0323, 1,        // OUT DATA     ; Output data byte to the terminal.
0043,           // INX M        ; Increment
0303, 3, 0,     // JMP INPUT
//
//              // EOP:
0166,           // HLT
//
//              // DATA:
0111            //              ; ASCII I
0040            //              ; ASCII space
0114            //              ; ASCII L
0111            //              ; ASCII I
0113            //              ; ASCII K
0105            //              ; ASCII E
0015            //              ; ASCII CR
0377            //              ; Stop code for CPI instruction.
````
--------------------------------------------------------------------------------
### The Initial Programs that I started with

````
// -----------------------------------------------------------------------------
// Sample machine code programs.

// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
  0303, 0006, 0000, // 0 1 2
  0000, 0000, 0000, // 3 4 5
  0303, 0000, 0000  // 6 7 8
};

// Define a jump loop program with NOP instructions.
byte jumpLoopNopProgram[] = {
  0303, 0004, 0000, // 0 1 2
  0000, 0000, 0000, // 3 4 5
  0303, 0000, 0000  // 6 7 8
};

byte TestProgram[] = {
  0303, 0003, 0000, // 0 1 2 JMP to address: 4 
  0041, 0006, 0000, // 3 4 5 LXI_HL lb hb. Load 0000:0006 into register H:L.
  0303, 0000, 0000  // 6 7 8
};

// Define a jump loop program
//    with a halt(HLT binary 01 110 110 = octal 166) instruction,
//    and NOP instructions.
byte jumpHaltLoopProgram[] = {
  0303, 0006, 0000,
  0000, 0000, 0000,
  0166, 0000, 0000,
  0303, 0000, 0000
};
````

--------------------------------------------------------------------------------
### Altair 8800 Pong Program

Click [here](https://altairclone.com/downloads/pong.pdf) code PDF document.

Click [here](https://www.youtube.com/watch?v=BZykunYYeQk) to watch a video for Pong.
````
                ;---------------------------------------------------------------
                ;  PONG for Altair front panel.
                ;     May 2014, Mike Douglas
                ; 
                ;  Left player quickly toggles A15 to hit the "ball."
                ;  Right player toggles A8.
                ;  Score is kept in memory locations
                ;  80h and 81h (left and right).
                ;  Score is missed balls, so the lower number wins.
                ;---------------------------------------------------------------
                ; Parameters:
                ;   SPEED determines how fast the ball moves. Higher values
                ;       move faster. Speed can easily be patched at address 1.
                ;
                ;   HITMSK, HITEDG determines how easy it is to hit the ball.
                ;       These are best changed by re-assembling the program.
                ;       Frankly, even the medium setting is too easy. Probably
                ;       best to stick with "hard" and change difficulty by
                ;       adjusting SPEED.
                ;
                ;   DEMO mode can be set by patching 35h and 65h to zero
                ;       and raising A15 and A8.
                ;
000E =          SPEED   equ     0eh         ;higher value is faster

0001 =          HITMSKR equ     01h         ;01h=hard, 03h=med, 07h=easy 
0002 =          HITEDGR equ     02h         ;02h=hard, 04h=med, 08h=easy
                                            ;00h=demo with A15,A8 up

0010 =          HITMSKL equ     10h         ;10h=hard, 18h=med, 1ch=easy 
0008 =          HITEDGL equ     08h         ;08h=hard, 04h=med, 02h=easy
                                            ;00h=demo with A15,A8 up

                ;---------------------------------------------------------------- 
                ; Initialize
0000                    org     0
0000 010E00             lxi     b,SPEED     ;BC=adder for speed 
0003 317D00             lxi     sp,stack    ;init stack pointer 
0006 210000             lxi     h,0         ;zero the score 
0009 228000             shld    scoreL 
000C 110080             lxi     d,8000h     ;D=ball bit, E=switch status 
000F C31E00             jmp     rLoop       ;begin moving right

                ;------------------------------------------------------------------
                ; ledOut - Write D to LEDs A15-A8.
                ;   Loop accessing the address in DE which causes the proper LED
                ;   to light on the address lights. This routine is placed low
                ;   in memory so that address light A7-A5 remain off to make
                ;   A15-A8 look better.
                ;------------------------------------------------------------------ 
0012 210000     ledOut  lxi     h,0         ;HL=16 bit counter 
0015 1A         ledLoop ldax    d           ;display bit pattern on 
0016 1A                 ldax    d           ;...upper 8 address lights 
0017 1A                 ldax    d 
0018 1A                 ldax    d 
0019 09                 dad     b           ;increment display counter 
001A D21500             jnc     ledLoop 
001D C9                 ret

                ;----------------------------------------------------------------
                ;  Moving Right
                ;---------------------------------------------------------------- 
001E CD1200     rLoop   call    ledOut      ;output to LEDs A15-A8 from D
                ; Record the current right paddle state (A8) in the bit position
                ;   in E corresponding to the present ball position. 
0021 DBFF               in      0ffh        ;A=front panel switches 
0023 E601               ani     01h         ;get A8 alone 
0025 CA2D00             jz      chkRt       ;switch not up, bit already zero 
0028 7A                 mov     a,d         ;set bit in E corresponding to... 
0029 B3                 ora     e           ;   the current ball position 
002A E61F               ani     1fh         ;keep b7-b5 zero
002C 5F                 mov     e,a
                ; See if at the right edge. If so, see if A8 "paddle" has a hit 
002D 7A         chkRt   mov     a,d         ;is ball at right edge? 
002E E601               ani     1 
0030 CA4500             jz      moveRt      ;no, continue moving right 
0033 7B                 mov     a,e         ;switch state for each ball position 
0034 E602               ani     HITEDGR     ;test edge for switch too early 
0036 C23F00             jnz     missRt      ;hit too early 
0039 7B                 mov     a,e         ;test for hit 
003A E601               ani     HITMSKR 
003C C27300             jnz     moveLfR     ;have a hit, switch direction
                ; missRt - right player missed the ball. Increment count 
003F 218100     missRt  lxi     h,scoreR    ;increment right misses 
0042 34                 inr     m
                ; moveRt - Move the ball right again. 
0043 1E00       moveRtR mvi     e,0         ;reset switch state 
0045 7A         moveRt  mov     a,d         ;move right again 
0046 0F                 rrc 
0047 57                 mov     d,a
0048 C31E00             jmp     rLoop

                ;----------------------------------------------------------------
                ;  Moving left
                ;---------------------------------------------------------------- 
04B CD1200      lLoop   call    ledOut      ;output to LEDs A15-A8 from D
                ; Record the current left paddle state (A15) in the bit position
                ;   in E corresponding to the present ball position. 
004E DBFF               in      0ffh        ;A=front panel switches 
0050 E680               ani     80h         ;get A15 alone 
0052 CA5D00             jz      chkLft      ;switch not up, bit already zero 
0055 7A                 mov     a,d         ;A=ball position 
0056 0F                 rrc                 ;move b7..b3 to b4..b0 
0057 0F                 rrc                 ;   so LEDs A7-A5 stay off 
0058 0F                 rrc
0059 B3                 ora     e           ;form switch state in E 
005A E61F               ani     1fh         ;keep b7-b5 zero 
005C 5F                 mov     e,a
                ; See if at the left edge. If so, see if A15 "paddle" has a hit 
005D 7A         chkLft  mov     a,d         ;is ball at left edge? 
005E E680               ani     80h 
0060 CA7500             jz      moveLf      ;no, continue moving left 
0063 7B                 mov     a,e         ;switch state for each ball position 
0064 E608               ani     HITEDGL     ;test edge for switch too early 
0066 C26F00             jnz     missLf      ;hit too early 
0069 7B                 mov     a,e         ;test for hit 
006A E610               ani     HITMSKL 
006C C24300             jnz     moveRtR     ;have a hit, switch direction
                ; missLf - left player missed the ball. Increment count 
006F 218000     missLf  lxi     h,scoreL    ;increment left misses 
0072 34                 inr     m
                ; moveLf - Move the ball left again. 
0073 1E00       LfR     mvi     e,0         ;reset switch state 
0075 7A         moveLf  mov     a,d         ;move right again 
0076 07                 rlc
0077 57                 mov     d,a 
0078 C34B00             jmp     lLoop

                ;------------------------------------------------------------------
                ; Data Storage
                ;------------------------------------------------------------------ 
007B                    ds      2           ;stack space
007D =         stack    equ     $ 
0080                    org     80h         ;put at 80h and 81h 
0080           scoreL   ds      1           ;score for left paddle 
0081           scoreR   ds      1           ;score for right paddle
0082                    end
````
// -----------------------------------------------------------------------------
Here is PONG in octal if you really want to enter it manually!
````
   0: 001 016 000 061 175 000 041 000   000 042 200 000 021 000 200 303
  20: 036 000 041 000 000 032 032 032   032 011 322 025 000 311 315 022
  40: 000 333 377 346 001 312 055 000   172 263 346 037 137 172 346 001
  60: 312 105 000 173 346 002 302 077   000 173 346 001 302 163 000 041
 100: 201 000 064 036 000 172 017 127   303 036 000 315 022 000 333 377
 120: 346 200 312 135 000 172 017 017   017 263 346 037 137 172 346 200
 140: 312 165 000 173 346 010 302 157   000 173 346 020 302 103 000 041
 160: 200 000 064 036 000 172 007 127   303 113 000
````
Or, load it from an array. Which can be saved as a binary file on an SD card.
````
byte theProgramPong[] = {
  0001, 0016, 0000, 0061, 0175, 0000, 0041, 0000, 0000, 0042, 0200, 0000, 0021, 0000, 0200, 0303,
  0036, 0000, 0041, 0000, 0000, 0032, 0032, 0032, 0032, 0011, 0322, 0025, 0000, 0311, 0315, 0022,
  0000, 0333, 0377, 0346, 0001, 0312, 0055, 0000, 0172, 0263, 0346, 0037, 0137, 0172, 0346, 0001,
  0312, 0105, 0000, 0173, 0346, 0002, 0302, 0077, 0000, 0173, 0346, 0001, 0302, 0163, 0000, 0041,
  0201, 0000, 0064, 0036, 0000, 0172, 0017, 0127, 0303, 0036, 0000, 0315, 0022, 0000, 0333, 0377,
  0346, 0200, 0312, 0135, 0000, 0172, 0017, 0017, 0017, 0263, 0346, 0037, 0137, 0172, 0346, 0200,
  0312, 0165, 0000, 0173, 0346, 0010, 0302, 0157, 0000, 0173, 0346, 0020, 0302, 0103, 0000, 0041,
  0200, 0000, 0064, 0036, 0000, 0172, 0007, 0127, 0303, 0113, 0000
};
````
--------------------------------------------------------------------------------
### Kill the Bit program.
````
byte theProgramKtb[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  //
  //                Start program.
  0041, 0000, 0000, // LXI H,0    ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
  0026, 0200,       // mvi D,080h ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
  0001, 0036, 0000, // LXI B,0eh  ; Load a(lb:hb) into register B:C. Higher value = faster. Default: 0016 = B:C  = 00 010 000
  //
  //  ; Display bit pattern on upper 8 address lights.
  //                // BEG:
  0166,             // HLT
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  //
  0011,             // DAD B      ; Add B:C to H:L. Set carry bit. Increments the display counter
  // 0322, 0010, 0000, // JNC BEG    ; If carry bit false, jump to lb hb, LDAX instruction start.
  //
  0333, 0377,       // IN 0ffh    ; Check for toggled input, at port 377 (toggle sense switches), that can kill the bit.
  0252,             // XRA D      ; Exclusive OR register with A
  0017,             // RRC        ; Rotate A right (shift byte right 1 bit). Set carry bit. Rotate display right one bit
  0127,             // MOV D,A    ; Move register A to register D. Move data to display reg
  //
  0303, 0010, 0000, // JMP BEG    ; Jump to lb hb, LDAX instruction start.
  // 0000,             // NOP
  // 0166,             // HLT
  // ------------------------------------------------------------------
  0000, 0000, 0000  //       end
};
````
#### Kill the Bit program expanded to byte level
````
  Addr Data toggles  Octal Value
  00   00 100 001     041 lxi  : Move the data at lb hb address, into register pair H(hb):L(lb)
  01   00 000 000     000      : lb
  02   00 000 000     000      : hb
  03   00 010 110     026 mvi  : Move db to register D.
  04   10 000 000     200      : db
  05   00 000 001     001 lxi  : Move the lb hb data, into register pair > B:C = hb:lb.
  06   00 001 110     016      : lb = 016
  07   00 000 000     000      : hb = 000
  //                         Make the bit that move across the hb address LED lights.
  08   00 011 010 beg:032 ldax : Load register A with the data at address D:E.
  09   00 011 010     032 ldax
  10   00 011 010     032 ldax
  11   00 011 010     032 ldax
  12   00 001 001     011 dad  : Add B:C to H:L. Set carry bit.
  13   11 010 010     322 jnc  : Jump to address 8(000:010)
  14   00 001 000     010
  15   00 000 000     000
  //
  16   11 011 011     333 in   : Check for the toggled input that can kill the bit.
  17   11 111 111     377
  18   10 101 010     252 xra
  19   00 001 111     017 rrc
  20   01 010 111     127 mov  : Move register A to register D.
  21   11 000 011     303 jmp  : Jump instruction: jmp beg
  22   00 001 000     010      : lb = 8. 00 001 000 = 8. Low order address bits.
  23   00 000 000     000      : hb = 0, to get a 16 bit address(hb:lb): 00 000 000 : 00 001 000 = 8.
  24                      end
````

--------------------------------------------------------------------------------
### Other test programs
````
// Sample to test:
//  LXI_HL : Intialize address value in H:L.
//  MOV_HL : Move the data at address H:L, to register A.
//  INX_HL : Increment the address H:L.
//  HLT    : Halt the program.
//  JMP    : Jump back to the move, and loop.
byte lxiNopLoopProgram[] = {
  //                         Start program.
  0000,                   // NOP
  0041, 14, 0000,         // LXI_HL lb hb. Load 0000:14 into register H:L.
  //
  0176,                   // MOV M:address(H:L):data > register A
  0000,                   // NOP
  0043,                   // INX > Increment H:L
  0000,                   // NOP
  0166,                   // HLT
  0000,                   // NOP
  0303, 0006, 0000,       // JMP to 6, jump to the MOV operation.
  //                         Data
  0000, 0101, 0110,       // 3 4 5
  0111, 0000, 0000,       // 6 7 8
  //
  0000, 0000, 0000  //       end
};
// byte NopLxiMovInxHltJmpCpiProgram[] = {
byte NopLxiMovInxHltJmpCpiProgram[] = {
  // ------------------------------------------------------------------
  // Initialize memory read location to memory start.
  // Move the first data byte to the accumulator, which is register A.
  // While not hit the end character (0111)
  //    Increment the memory pointer (H:L).
  //    Move the next data byte to the accumulator.
  //    Halt the process.
  //    Restart the while loop.
  // End while.
  // Restart the program from memory initialization.
  //
  //                Start program.
  0000,             // NOP
  0041, 24, 0000,   // LXI_HL lb hb. Load hb:lb into registers H(hb):L(lb).
  0176,             // MOV M:address(H:L):data > register A
  //
  //                While address:data != 0111
  0376, 0111,             // CPI Compare A with 0111.
  0312, 19, 0000,         // JZ lb hb. If it matches, jump to lb hb (end while)
  0000,                   // NOP
  0166,                   // HLT
  0043,                   // INX > Increment H:L
  0176,                   // MOV M:address(H:L):data > register A
  0000,                   // NOP
  0303, 0005, 0000,       // JMP to the start of the while loop.
  //                End while.
  0000,             // NOP
  0166,             // HLT
  0000,             // NOP
  0303, 1, 0000,    // JMP to 1. Restart: jump to program start.
  // ------------------------------------------------------------------
  //                Data, starts at address 24.
  0000, 0101, 0001, // 5 6 7
  0010, 0110, 0111, // 8 9 0
  //
  0000, 0000, 0000  //       end
};
byte theProgram[] = {
  /*
//         Code     Octal    Inst Param  Encoding Param  Flags  Description
const byte DAD_BC = 0011; // DAD         00001001          C    Add B:C to H:L. Set carry bit.
//                           DAD  RP     00RP1001          C    Add register pair to HL (16 bit add)
//                           Set carry bit if the addition causes a carry out.
const byte JNC =    0322; // JNC  lb hb  11010010               Jump if carry bit is 0 (false).

   */
  // ------------------------------------------------------------------
  // Initialize memory read location to memory start.
  // Move the first data byte to the accumulator, which is register A.
  // While not hit the end character (0111)
  //    Increment the memory pointer (H:L).
  //    Move the next data byte to the accumulator.
  //    Halt the process.
  //    Restart the while loop.
  // End while.
  // Restart the program from memory initialization.
  //
  //                Start program.
  0000,             // NOP
  0166,             // HLT  : Halt to check the program listing.
  0006, 0776,       // mvi  B,db : Move db to register B.
  0016, 0777,       // mvi  C,db : Move db to register B.
  //                While address:data != 0111
  0376, 0111,             // DAD Add B:C to H:L. Set carry bit.
  0312, 19, 0000,         // JNC lb hb. If carry bit false, jump to lb hb (end while)
  0000,                   // NOP
  0000,                   // NOP
  0000,                   // NOP
  0000,                   // NOP
  0166,                   // HLT
  0043,                   // INX > Increment B
  0000,                   // NOP
  0303, 0005, 0000,       // JMP to the start of the while loop.
  //                End while.
  0000,             // NOP
  0166,             // HLT
  0000,             // NOP
  0303, 1, 0000,    // JMP to 1. Restart: jump to program start.
  // ------------------------------------------------------------------
  //                Data, starts at address 24.
  0000, 0101, 0001, // 5 6 7
  0010, 0110, 0111, // 8 9 0
  //
  0000, 0000, 0000  //       end
};

byte theProgram[] = {
  // ------------------------------------------------------------------
  // Test: mvi, DAD, JNC
  //                Start program.
  0000,             // NOP
  //                               Set B:C to 0000:1
  0006, 0000,       // mvi  B,db : Move db to register B.
  0016, 0001,       // mvi  C,db : Move db to register C.
  //                               Set
  0041, 0373, 0377, // LXI_HL lb hb. Load into register H:L = 377:373 = 65531.
  //                Until !carry bit {
  0000,               // NOP
  0166,               // HLT
  0011,               // DAD Add B:C to H:L. Set carry bit at: 65535.
  0322,  8, 0000,     // JNC lb hb. If carry bit false, jump to lb hb (end while)
  0303, 17, 0000,     // JMP to the start of the while loop.
  //                }
  0166,             // HLT
  0000,             // NOP
  0303, 1, 0000,    // JMP to 1. Restart: jump to program start.
  // ------------------------------------------------------------------
  //                Data, starts at address ?
  0000, 0101, 0001, // 5 6 7
  0010, 0110, 0111, // 8 9 0
  0000, 0000, 0000  //       end
};

/*
  00 000 000 = 000 =   0 2^0
  00 000 001 = 002 =   1 2^0
  00 000 010 = 002 =   2 2^1
  00 000 100 = 040 =   4 2^2
  00 001 000 = 010 =   8 2^3
  00 010 000 = 020 =  16 2^4
  00 100 000 = 014 =  32 2^5
  01 000 000 = 014 =  64 2^6
  10 000 000 = 014 = 128 2^7
                     256 2^8
                     512 2^9
                    1024 2^10  1K
                    2048 2^11  2K
                    4096 2^12  4K
                    8192 2^13  8K
                   16384 2^14 16K
                   32768 2^15 32k
                   65535 2^16 64k
*/
````

--------------------------------------------------------------------------------
Cheers