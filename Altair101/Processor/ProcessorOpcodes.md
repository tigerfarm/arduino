--------------------------------------------------------------------------------
#  Altair 101 Opcodes

I have added 8080 opcodes to run the classic program, Kill the Bit.
Other opcodes are for testing opcodes, for example mvi, to set test values when testing other opcodes.
I am also adding opcodes to run the program Pong.

The opcodes to run Pong and Kill the Bit, are my target opcode base for my 8080 emulator.
I have not planned on implementing all the 8080 opcodes, just the ones that I will use when writing my own programs.

## Programmed Opcodes

From the processor program, [Processor.ino](Processor.ino), the opcode implementation function:
````
void processOpcode() {
    ...
}
void processOpcodeData() {
    ...
}
````

Click [here](https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/8080opcodesBinaryList.txt)
to view a binary opcode list with the opcode number of cycles.

#### Functionally ordered list of implemented opcodes:
````
Opcode   Binary   Cycles Description
-------------------------------------
Initialize and store values:
mvi R,#  00 DDD 110  2  Move a number (#/db) to a register.
mov D,S  01 DDD SSS  1  Move source register data, to the destination register. If source(S) is M(110), move data from address H:L, to D.
lxi RP,a 00 RP0 001  3  Move the data at the address, a(lb hb), into register pair: B:C, D:E, or H:L. To do: move data to the stack pointer address.
sta a    00 110 010  3  Store register A to the address, a(hb:lb).
lda a    00 110 010  3  Load register A with data from the address, a(hb:lb).
ldax RP  00 RP1 010  1  Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.
shld a   00 100 010  3  Store data value from memory location: a(hb:lb), to register L. Store value at: a + 1, to register H.

Jumping and conditions:
jmp a    11 000 011  3  Unconditional jump.
Set condition:
cmp S    10 111 SSS  1  ZSPCA   Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.
cpi #    11 111 110  2  Compare # to A, then set flags. If #=A, set Zero bit to 1. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
Conditional jumps:
jz a     11 001 010  1  Jump to a, if zero bit flag is set (equals 1).
jnz a    11 000 010  1  Jump to a, if Zero bit flag is not set (equals 0).
jc a     11 011 010  1  Jump to a, if Carry bit flag is set (equals 1).
jnc a    11 010 010  1  Jump to a, if Carry bit flag is not set (equals 0).

Logical and bitwise:
ani #    11 100 110  2  AND # (immediate db) with register A.
ora S    10 110 SSS  1  OR register S, with register A.
xra S    10 101 SSS  1  Exclusive OR, the register(S) with register A.
rrc      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit.

Arithmetic:
dad RP   00 RP1 001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L, into H:L. And set carry bit.
inr D    00 DDD 100  1  Increment register DDD. To do, set flags: ZSPA.
dcr D    00 DDD 101  1  Decrement register DDD. To do, set flags: ZSPA.
inx RP   00 RP0 011  1  Increment a register pair (a 16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.

Process:
call a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.
ret      11 001 001  1  Unconditional return from subroutine. Pop the call address from the stack and continue to the next address.
push RP  11 RP0 101  1  Push register pair on the stack.
pop  RP  11 RP0 001  1  Push register pair on the stack.
hlt      01 110 110  1  Halt processor.
nop      00 000 000  1  No operation. I added a delay: delay(100).
out pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.
in pa    11 011 011  2  Read port a data into the accumulator. Example, a=0377 is the sense switches.
````
#### Alphabetically order list of implemented opcodes:
````
Opcode   Binary   Cycles Description
-------------------------------------
ani #    11 100 110  2  AND # (immediate db) with register A.
call a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.
cmp S    10 111 SSS  1  ZSPCA   Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.
cpi #    11 111 110  2  Compare # to A. Store true or false into flagZeroBit.
dad RP   00 RP1 001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L. And set carry bit.
call a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.
dcr D    00 DDD 101  1  Decrement a register. To do, set flags: ZSPA.
hlt      01 110 110  1  Halt processor.
lxi RP,a 00 RP0 001  3  Move the data at the address, a(lb hb), into register pair: B:C, D:E, or H:L. To do: move data to the stack pointer address.
in pa    11 011 011  2  Read port a data into the accumulator. Example, a=0377 is the sense switches.
inr D    00 DDD 100  1  Increment a register. To do, set flags: ZSPA.
inx RP   00 RP0 011  1  Increment a register pair (a 16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.
jnc a    11 010 010  3  Jump if not carry bit, i.e. if carry bit value is 0, false, not set.
jmp a    11 000 011  3  Unconditional jump.
jc a     11 011 010  1  Jump to a, if Carry bit flag is set (equals 1).
jnc a    11 010 010  1  Jump to a, if Carry bit flag is not set (equals 0).
jnz a    11 000 010  1  Jump to a, if Zero bit flag is not set (equals 0).
jz a     11 001 010  1  Jump to a, if zero bit flag is set (equals 1).
lda a    00 110 010  3  Load register A with data from the address, a(hb:lb).
ldax RP  00 RP1 010  1  Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.
mvi R,#  00 RRR 110  2  Move a number (#, db) to a register.
mov D,S  01 DDD SSS  1  Move source register data, to the destination register.
nop      00 000 000  1  No operation. I added a delay: delay(100).
out pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.
ora S    10 110 SSS  1  OR register S, with register A.
push RP  11 RP0 101  1  Push register pair on the stack.
pop  RP  11 RP0 001  1  Push register pair on the stack.
ret      11 001 001  1  Unconditional return from subroutine. Pop the call address from the stack and continue to the next address.
rrc      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit.
shld a   00 100 010  3  Store data value from memory location: a(hb:lb), to register L. Store value at: a + 1, to register H.
sta a    00 110 010  3  Store register A to the address, a(hb:lb).
xra S    10 101 SSS  1  Exclusive OR, the register(S) with register A.
````
--------------------------------------------------------------------------------
### 8080 Opcodes Not added to Altair 101

The following list is from a [popular text list](https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/8080opcodes.txt).

I also use information from a [binary order list](https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/8080opcodesBinaryList.txt),
which I find functional and interesting.
It's also a practical help, in that it describes opcode implementations, better than the other list.
````
Inst      Encoding          Flags   Description
----------------------------------------------------------------------

DCX RP    00RP1011          -       Decrement register pair
LHLD a    00101010 lb hb    -       Load H:L with address value. 
ORI #     11110110          ZSPCA   OR  immediate with A
ANA S     10100SSS          ZSCPA   AND register with A
XRI #     11101110 db       ZSPCA   ExclusiveOR immediate with A
PCHL      11101001          -       Jump to address in H:L
STAX RP   00RP0010 *1       -       Store indirect through BC or DE
XCHG      11101011          -       Exchange DE and HL content

ADD S     10000SSS          ZSPCA   Add register to A
ADI #     11000110 db       ZSCPA   Add immediate to A
ADC S     10001SSS          ZSCPA   Add register to A with carry
ACI #     11001110 db       ZSCPA   Add immediate to A with carry
SUB S     10010SSS          ZSCPA   Subtract register from A
SUI #     11010110 db       ZSCPA   Subtract immediate from A
SBB S     10011SSS          ZSCPA   Subtract register from A with borrow
SBI #     11011110 db       ZSCPA   Subtract immediate from A with borrow

DAA       00100111          ZSPCA   Decimal Adjust accumulator

RLC       00000111          C       Rotate A left
RAL       00010111          C       Rotate A left through carry
RAR       00011111          C       Rotate A right through carry
CMA       00101111          -       Compliment A
CMC       00111111          C       Compliment Carry flag
STC       00110111          C       Set Carry flag
Cccc a    11CCC100 lb hb    -       Conditional subroutine call
Rccc      11CCC000          -       Conditional return from subroutine
RST n     11NNN111          -       Restart (Call n*8)

Other jumps:
JM a      11 100 010 lb hb          Jump if minus.
JP a      11 101 010 lb hb          Jump if positive.
JPE a     11 110 010 lb hb          Jump if parity even.
JPO a     11 111 010 lb hb          Jump if parity odd.

XTHL      11100011          -       Swap H:L with top word on stack
SPHL      11111001          -       Set SP to content of H:L
EI        11111011          -       Enable interrupts
DI        11110011          -       Disable interrupts

*2 = RP=11 refers to PSW for PUSH/POP (cannot push/pop SP).
     When PSW is POP'd, ALL flags are affected.
````
--------------------------------------------------------------------------------
### Conventions used when describing Opcodes

Source line format:
````
Label: Opcode Operand ;Comment
-- or --
Name   Opcode Operand ;Comment
````
A name is required for the SET, EQU, and MACRO directives.

A dollar sign can be used as a symbol to denote the value currently in the location counter.
For example, the command
````
JMP $+6
````
Reference document:
http://www.nj7p.org/Manuals/PDFs/Intel/9800301D.pdf

#### Destination and Source registers and register pairs.
````
byte regA = 0;   111=A  a  register A, or Accumulator
                           --------------------------------
                           Register pair 'RP' fields:
byte regB = 0;   000=B  b  00=BC   (B:C as 16 bit register)
byte regC = 0;   001=C  c
byte regD = 0;   010=D  d  01=DE   (D:E as 16 bit register)
byte regE = 0;   011=E  e
byte regH = 0;   100=H  h  10=HL   (H:L as 16 bit register)
byte regL = 0;   101=L  l
                           11=SP   (Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP)
byte regM = 0;   110=M  m  Memory reference for a memory address in H:L
````
Register pair identifiers:
````
  b = B:C, 00=B:C
  d = D:E, 01=D:E
  m = H:L, 10=H:L
  ? = SP,  11=SP, Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP.
````
Register identifiers (binary value=R):
````
  000=B
  001=C
  010=D
  011=E
  100=H
  101=L
````
#### Instruction encoding parameter index:
````
  a     = A 16 bit address value, often stated in hexadecimal format, example:
          ani 80h ;get A15 alone
  lb    = Low byte of 16 bit value
  hb    = High byte of 16 bit value
  lb hb = A 16 bit address which is 2 bytes, low order byte first, high order byte second.
          Example: LDA a, encoded as 8 bit (1 byte) opcode, 1 byte lb, 1 byte hb: "00111010 lb hb".
  p     = 8 bit port address, example:
          in 0ffh ;A=front panel switches
        Encoding, 0ffh = 00000000:11111111(binary) = 255(decimal):
          11011011:11111111:00000000(in:lb:hb)

  #     = 8 or 16 bit immediate operand, example:
          ani 01h ;get A8 alone
  RP    = Register pair (16 bit): B:C(b), D:E(d), H:L(h or m). For example:
          ldax d ;display bit pattern on upper 8 address lights
  RRR (or R) = Register binary value, example: 010, for register D.
  DDD (or D) = Destination register binary value, example: 011, for register E.
  SSS (or S) = Source register binary value, example: 011, for register E.
  ccc   = Conditional
  db    = Data byte (8 bit) value
  pa    = Port address (8 bit) value

Condition code 'CCC' fields: (FLAGS: S Z x A x P x C)
    000=NZ  ('Z'ero flag not set)
    001=Z   ('Z'ero flag set)
    010=NC  ('C'arry flag not set)
    011=C   ('C'arry flag set)
    100=PO  ('P'arity flag not set - ODD)
    101=PE  ('P'arity flag set - EVEN)
    110=P   ('S'ign flag not set - POSITIVE)
    111=M   ('S'ign flag set - MINUS)

indirect: the data value at the give address.
    Example: LDAX RP - Load indirect through BC or DE.
    Detail explanation:
        Load data value
            at the register pair (BC(RP=00) or DE(RP=01)) address location,
            into register A (accumulator).
immediate: the immediate byte value.
    Example: ANI # - AND immediate with A.
````
-----------------------------------------------------------------------------
### Reference Links

Altair 8800 Operator's Manual.pdf has a description of each opcode.
    https://altairclone.com/downloads/manuals/Altair%208800%20Operator's%20Manual.pdf

From original Altair Manuals:
    https://altairclone.com/altair_manuals.htm

Reference document, Intel 8080 Assembly Language Programming Manual:
    https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf

This section is base on section 26: 8080 Instruction Set
    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf

Text listing of 8080 opcodes:
    http://www.classiccmp.org/dunfield/r/8080.txt

Binary calculator:
    https://www.calculator.net/binary-calculator.html

-----------------------------------------------------------------------------
Cheers