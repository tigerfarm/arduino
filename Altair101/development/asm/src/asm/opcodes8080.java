package asm;

/*
--------------------------------------------------------------------------------
Opcodes implemented by this assembler, implemented in Processor.ino, and tested.
Opcode   Binary   Cycles Description
-------------------------------------
adi #    11 000 110     Add immediate number to register A, and set ZSCPA.
ani #    11 100 110  2  AND # (immediate db) with register A.
cmp S    10 111 SSS     Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.
cpi #    11 111 110  2  Compare # to A. Store true or false into flagZeroBit.
dad RP   00 RP1 001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L. And set carry bit.
ldax RP  00 RP1 010  1  Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.
hlt      01 110 110  1  Halt processor.
in pa    11 011 011  2  Read port a data into the accumulator. Example, a=0377 is the sense switches.
inr D    00 DDD 101  1  Increment a register. To do, set flags: ZSPA.
jnc a    11 010 010  3  Jump if not carry bit, i.e. if carry bit value is 0, false, not set.
jmp a    11 000 011  3  Unconditional jump.
jz  a    11 001 010  3  If flagZeroBit is true, jump to address (a = lb hb).
lxi RP,a 00 RP0 001  3  Move the data at the address, a(lb hb), into register pair: B:C, D:E, or H:L. To do: move data to the stack pointer address.
mvi D,#  00 DDD 110  2  Move a number (#/db) to a register.
mov D,S  01 DDD SSS  1  Move source register data, to the destination register.
nop      00 000 000  1  No operation. I added a delay: delay(100).
ora S    10 110 SSS  1  OR register S, with register A.
out pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.
rrc      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit.
sui #    11 010 110     ZSCPA Subtract immedite number from register A.
xra S    10 101 SSS  1  Exclusive OR, the register(R) with register A.

Opcodes implemented by this assembler, implemented in Processor.ino, but not tested.
Opcode   Binary   Cycles Description
-------------------------------------
call a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.
ret      11 001 001  1  Unconditional return from subroutine. Pop the call address from the stack and continue to the next address.

dcr D    00 DDD 101  1  Decrement a register. To do, set flags: ZSPA.
inx RP   00 RP0 011  1  Increment a register pair (a 16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.
lda a    00 110 010  3  Load register A with data from the address, a(hb:lb).
shld a   00 100 010  3  Store data value from memory location: a(hb:lb), to register L. Store value at: a + 1, to register H.
sta a    00 110 010  3  Store register A to the address, a(hb:lb).

Opcodes implemented in Processor.ino, but not yet in this assembler,
Opcode   Binary   Cycles Description
-------------------------------------
push RP  11 RP0 101  1  Push register pair on the stack.
pop  RP  11 RP0 001  1  POP register pair from the stack.

*/

public class opcodes8080 {

    private int top = 0;
    private String[] name;
    private byte[] value;
    private String[] info;
    public final byte OpcodeNotFound = (byte) 255;

    public static String byteToString(byte aByte) {
        return toBinary(aByte, 8);
    }

    private static String toBinary(byte a, int bits) {
        if (--bits > 0) {
            return toBinary((byte) (a >> 1), bits) + ((a & 0x1) == 0 ? "0" : "1");
        } else {
            return (a & 0x1) == 0 ? "0" : "1";
        }
    }

    // -------------------------------------------------------------------------
    public opcodes8080() {
        setOpcodeData();
    }

    public void opcodesList() {
        System.out.println("+ List opcode data.");
        for (int i = 0; i < top; i++) {
            System.out.println("++ " + i + ": " + name[i] + " " + byteToString(value[i]));
        }
        System.out.println("+ End list.");
    }

    public void getOpcodeInfo(String theName) {
        byte returnValue = OpcodeNotFound;
        for (int i = 0; i < top; i++) {
            if (name[i].equals(theName)) {
                if (info[i] != null) {
                    System.out.println("Opcode   Binary   Cycles Description");
                    System.out.println("-------------------------------------");
                    System.out.println(info[i]);
                }
                System.out.println("++ " + i + ": " + name[i] + " " + value[i]);
                returnValue = value[i];
                break;
            }
        }
        if (returnValue == OpcodeNotFound) {
            System.out.println("++ Opcode Not Found: " + theName);
        }
    }

    public byte getOpcode(String theName) {
        byte returnValue = OpcodeNotFound;
        for (int i = 0; i < top; i++) {
            // System.out.println("++ " + i + ": " + name[i] + " " + value[i]);
            if (name[i].equals(theName)) {
                returnValue = value[i];
                break;
            }
        }
        return returnValue;
    }

    private void setOpcodeData() {
        System.out.println("+ Set opcode data.");

        name = new String[255];
        info = new String[255];
        value = new byte[255];
        // ---------------------------------------------------------------------
        name[top] = "adi";
        info[top] = "ADI #      11000110  3  Add immediate number to register A, set: ZSCPA.";
        value[top++] = (byte) 0b11000110;
        // ---------------------------------------------------------------------
        name[top] = "ani";
        info[top] = "ANI #    11 100 110  2  AND # (immediate db) with register A.";
        value[top++] = (byte) 0b11100110;
        // ---------------------------------------------------------------------
        name[top] = "call";
        info[top] = "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.";
        value[top++] = (byte) 0b11001101;
        // ---------------------------------------------------------------------
        name[top] = "cmp";
        info[top] = "CMP S    10 111 SSS  1  Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.";
        value[top++] = (byte) 0b10111111;   // not used value
        name[top] = "cmpa";  // 10111SSS
        value[top++] = (byte) 0b10111111;
        name[top] = "cmpb";
        value[top++] = (byte) 0b10111000;
        name[top] = "cmpc";
        value[top++] = (byte) 0b10111001;
        name[top] = "cmpd";
        value[top++] = (byte) 0b10111010;
        name[top] = "cmpe";
        value[top++] = (byte) 0b10111011;
        name[top] = "cmph";
        value[top++] = (byte) 0b10111100;
        name[top] = "cmpl";
        value[top++] = (byte) 0b10111101;
        // ---------------------------------------------------------------------
        name[top] = "cpi";
        info[top] = "CPI #    11 111 110  2  Compare # to A. Store true or false into flagZeroBit.";
        value[top++] = (byte) 0b11111110;
        // ---------------------------------------------------------------------
        name[top] = "dad";
        info[top] = "DAD RP     00RP1001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L, into H:L. And set carry bit.";
        value[top++] = (byte) 0b00111001;   // not used value
        name[top] = "dadb";
        value[top++] = (byte) 0b00001001;
        name[top] = "dadd";
        value[top++] = (byte) 0b00101001;
        // ---------------------------------------------------------------------
        name[top] = "dcr";
        info[top] = "DCR D    00 DDD 101  1  Decrement a register. To do, set flags: ZSPA.";
        value[top++] = (byte) 0b00111101;   // not used value
        name[top] = "dcra";  // 00DDD101
        value[top++] = (byte) 0b00111101;
        name[top] = "dcrb";
        value[top++] = (byte) 0b00000101;
        name[top] = "dcrc";
        value[top++] = (byte) 0b00001101;
        name[top] = "dcrd";
        value[top++] = (byte) 0b00010101;
        name[top] = "dcre";
        value[top++] = (byte) 0b00011101;
        name[top] = "dcrh";
        value[top++] = (byte) 0b00100101;
        name[top] = "dcrl";
        value[top++] = (byte) 0b00101101;
        // ---------------------------------------------------------------------
        name[top] = "hlt";
        info[top] = "HLT      01 110 110  1  Halt processor.";
        value[top++] = (byte) 0b01110110;
        // ---------------------------------------------------------------------
        name[top] = "in";
        info[top] = "IN pa    11 011 011  2  Read port a data into the accumulator. Example, a=0377 is the sense switches.";
        value[top++] = (byte) 0b11011011;
        // ---------------------------------------------------------------------
        name[top] = "inr";
        info[top] = "INR D    00 DDD 101  1  Increment register DDD. To do, set flags: ZSPA.";
        name[top] = "inra";  // 00DDD101
        value[top++] = (byte) 0b00111101;
        name[top] = "inrb";
        value[top++] = (byte) 0b00000101;
        name[top] = "inrc";
        value[top++] = (byte) 0b00001101;
        name[top] = "inrd";
        value[top++] = (byte) 0b00010101;
        name[top] = "inre";
        value[top++] = (byte) 0b00011101;
        name[top] = "inrh";
        value[top++] = (byte) 0b00100101;
        name[top] = "inrl";
        value[top++] = (byte) 0b00101101;
        // ---------------------------------------------------------------------
        name[top] = "inx";
        info[top] = "INX RP   00 RP0 011  1  Increment a register pair (a 16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.";
        value[top++] = (byte) 0b00110011;   // not used value
        name[top] = "inxb"; //  00RP0011
        value[top++] = (byte) 0b00000011;
        name[top] = "inxd";
        value[top++] = (byte) 0b00010011;
        name[top] = "inxh";
        value[top++] = (byte) 0b00100011;
        // ---------------------------------------------------------------------
        name[top] = "jmp";
        info[top] = "JMP a    11 000 011  3  Unconditional jump.";
        value[top++] = (byte) 0b11000011;
        name[top] = "jnz";   //11 000 010
        info[top] = "JNZ a    11 000 010  3  Jump to a, if Zero bit flag is not set (equals 0).";
        value[top++] = (byte) 0b11000010;
        name[top] = "jz";
        info[top] = "JZ a     11 001 010  3  Jump to a, if zero bit flag is set (equals 1).";
        value[top++] = (byte) 0b11001010;
        name[top] = "jnc";
        info[top] = "JNC a    11 010 010  3  Jump to a, if Carry bit flag is not set (equals 0).";
        value[top++] = (byte) 0b11010010;
        name[top] = "jc";
        info[top] = "JC a     11 011 010  3  Jump to a, if Carry bit flag is set (equals 1).";
        value[top++] = (byte) 0b11011010;
        // ---------------------------------------------------------------------
        name[top] = "lda";
        info[top] = "LDA a    00 110 010  3  Load register A with data from the address, a(hb:lb).";
        value[top++] = (byte) 0b00110010;
        // ---------------------------------------------------------------------
        name[top] = "ldax";
        info[top] = "LDAX RP    00RP1010  1  Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.";
        value[top++] = (byte) 0b00111010;   // not used value
        name[top] = "ldaxb";
        value[top++] = (byte) 0b00001010;
        name[top] = "ldaxd";
        value[top++] = (byte) 0b00011010;
        // ---------------------------------------------------------------------
        name[top] = "lxi";
        info[top] = "LXI RP,a   00RP0001  3  Move the data at the address, a(lb hb), into register pair: B:C, D:E, or H:L. To do: move data to the stack pointer address.";
        value[top++] = (byte) 0b00110001;   // not used value
        name[top] = "lxib";
        value[top++] = (byte) 0b00000001;
        name[top] = "lxid";
        value[top++] = (byte) 0b00010001;
        name[top] = "lxih";
        value[top++] = (byte) 0b00100001;
        // ---------------------------------------------------------------------
        name[top] = "mov";
        info[top] = "MOV D,S    01DDDSSS  1  Move source register data, to the destination register.";
        value[top++] = (byte) 0b01111111;   // not used
        name[top] = "movab";  //01111111
        value[top++] = (byte) 0b01111000;
        name[top] = "movac";
        value[top++] = (byte) 0b01111001;
        name[top] = "movad";
        value[top++] = (byte) 0b01111010;
        name[top] = "movae";
        value[top++] = (byte) 0b01111011;
        name[top] = "movah";
        value[top++] = (byte) 0b01111100;
        name[top] = "moval";
        value[top++] = (byte) 0b01111101;
        name[top] = "movba";  //01000111
        value[top++] = (byte) 0b01000111;
        name[top] = "movbc";
        value[top++] = (byte) 0b01000001;
        name[top] = "movbd";
        value[top++] = (byte) 0b01000010;
        name[top] = "movbe";
        value[top++] = (byte) 0b01000011;
        name[top] = "movbh";
        value[top++] = (byte) 0b01000100;
        name[top] = "movbl";
        value[top++] = (byte) 0b01000101;
        name[top] = "movca";  //01001111
        value[top++] = (byte) 0b01001111;
        name[top] = "movcb";
        value[top++] = (byte) 0b01001000;
        name[top] = "movcd";
        value[top++] = (byte) 0b01001010;
        name[top] = "movce";
        value[top++] = (byte) 0b01001011;
        name[top] = "movch";
        value[top++] = (byte) 0b01001100;
        name[top] = "movcl";
        value[top++] = (byte) 0b01001101;
        name[top] = "movda";  //01010111
        value[top++] = (byte) 0b01010111;
        name[top] = "movdb";
        value[top++] = (byte) 0b01010000;
        name[top] = "movdc";
        value[top++] = (byte) 0b01010001;
        name[top] = "movde";
        value[top++] = (byte) 0b01010011;
        name[top] = "movdh";
        value[top++] = (byte) 0b01010100;
        name[top] = "movdl";
        value[top++] = (byte) 0b01010101;
        name[top] = "movea";  //01011111
        value[top++] = (byte) 0b01011111;
        name[top] = "moveb";
        value[top++] = (byte) 0b01011000;
        name[top] = "movec";
        value[top++] = (byte) 0b01011001;
        name[top] = "moved";
        value[top++] = (byte) 0b01011010;
        name[top] = "moveh";
        value[top++] = (byte) 0b01011100;
        name[top] = "movel";
        value[top++] = (byte) 0b01011101;
        name[top] = "movha";  //01100111
        value[top++] = (byte) 0b01100111;
        name[top] = "movhb";
        value[top++] = (byte) 0b01100000;
        name[top] = "movhc";
        value[top++] = (byte) 0b01100001;
        name[top] = "movhd";
        value[top++] = (byte) 0b01100010;
        name[top] = "movhe";
        value[top++] = (byte) 0b01100011;
        name[top] = "movhl";
        value[top++] = (byte) 0b01100101;
        name[top] = "movla";  //01101111
        value[top++] = (byte) 0b01101111;
        name[top] = "movlb";
        value[top++] = (byte) 0b01101000;
        name[top] = "movlc";
        value[top++] = (byte) 0b01101001;
        name[top] = "movld";
        value[top++] = (byte) 0b01101010;
        name[top] = "movle";
        value[top++] = (byte) 0b01101011;
        name[top] = "movlh";
        value[top++] = (byte) 0b01101100;
        
        // ---------------------------------------------------------------------
        name[top] = "mvi";
        info[top] = "MVI R,#  00 RRR 110  2  Move a number (#, db) to a register.";
        name[top] = "mvia";  // 00RRR110
        value[top++] = (byte) 0b00111110;
        name[top] = "mvib";
        value[top++] = (byte) 0b00000110;
        name[top] = "mvic";
        value[top++] = (byte) 0b00001110;
        name[top] = "mvid";
        value[top++] = (byte) 0b00010110;
        name[top] = "mvie";
        value[top++] = (byte) 0b00011110;
        name[top] = "mvih";
        value[top++] = (byte) 0b00100110;
        name[top] = "mvil";
        value[top++] = (byte) 0b00101110;
        // ---------------------------------------------------------------------
        name[top] = "nop";
        info[top] = "NOP      00 000 000  1  No operation. I added a delay: delay(100).";
        value[top++] = (byte) 0b00000000;
        // ---------------------------------------------------------------------
        name[top] = "ora";
        info[top] = "ORA S    10 110 SSS  1  OR register S, with register A.";
        value[top++] = (byte) 0b10110111;   // not used value
        name[top] = "oraa";  // 10110SSS
        value[top++] = (byte) 0b10110111;
        name[top] = "orab";
        value[top++] = (byte) 0b10110000;
        name[top] = "orac";
        value[top++] = (byte) 0b10110001;
        name[top] = "orad";
        value[top++] = (byte) 0b10110010;
        name[top] = "orae";
        value[top++] = (byte) 0b10110011;
        name[top] = "orah";
        value[top++] = (byte) 0b10110100;
        name[top] = "oral";
        value[top++] = (byte) 0b10110101;
        name[top] = "oram";
        value[top++] = (byte) 0b10110110;
        // ---------------------------------------------------------------------
        name[top] = "out";
        info[top] = "OUT pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.";
        value[top++] = (byte) 0b11100011;
        // ---------------------------------------------------------------------
        name[top] = "ret";
        info[top] = "RET      11 001 001  1  Unconditional return from subroutine. Pop the call address from the stack and continue to the next address.";
        value[top++] = (byte) 0b11001001;
        // ---------------------------------------------------------------------
        name[top] = "rrc";
        info[top] = "RRC      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit.";
        value[top++] = (byte) 0b00001111;
        // ---------------------------------------------------------------------
        name[top] = "shld";
        info[top] = "SHLD a   00 100 010  3  Store data value from memory location: a(hb:lb), to register L. Store value at: a + 1, to register H.";
        value[top++] = (byte) 0b00100010;
        // ---------------------------------------------------------------------
        name[top] = "sta";
        info[top] = "STA a    00 110 010  3  Store register A to the address, a(hb:lb).";
        value[top++] = (byte) 0b00110010;
        // ---------------------------------------------------------------------
        name[top] = "sui";
        info[top] = "SUI #      11010110  3  Subtract immediate number from register A, set ZSCPA.";
        value[top++] = (byte) 0b11010110;
        // ---------------------------------------------------------------------
        name[top] = "xra";
        info[top] = "XRA R      10101SSS  1  Exclusive OR, the register(R) with register A.";
        value[top++] = (byte) 0b10101111;
        name[top] = "xrab";
        value[top++] = (byte) 0b10101000;
        name[top] = "xrac";
        value[top++] = (byte) 0b10101001;
        name[top] = "xrad";
        value[top++] = (byte) 0b10101010;
        name[top] = "xrae";
        value[top++] = (byte) 0b10101011;
        name[top] = "xrah";
        value[top++] = (byte) 0b10101100;
        name[top] = "xral";
        value[top++] = (byte) 0b10101101;        
        // ---------------------------------------------------------------------
        System.out.println("++ Number opcode values = " + top);
    }

    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("+++ Start.");

        opcodes8080 theOpcodes = new opcodes8080();
        theOpcodes.opcodesList();
        String anOpcode = "jmp";
        System.out.println("+ Opcode: " + anOpcode + " " + byteToString(theOpcodes.getOpcode(anOpcode)));

        System.out.println("+++ Exit.");
    }
}
