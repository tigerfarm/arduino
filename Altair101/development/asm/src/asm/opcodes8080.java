package asm;

public class opcodes8080 {

    private int top = 0;
    private String[] name;
    private byte[] value;
    private String[] info;
    public final byte OpcodeNotFound = (byte) 255;

    public static String printByte(byte aByte) {
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
            System.out.println("++ " + i + ": " + name[i] + " " + printByte(value[i]));
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
        name[top] = "cmp";
        info[top] = "CMP S     10 111 SSS        ZSPCA   Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.";
        value[top++] = (byte) 0b10111111;
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
        name[top] = "hlt";
        info[top] = "hlt      01 110 110  1  Halt processor.";
        value[top++] = (byte) 0b01110110;
        // ---------------------------------------------------------------------
        name[top] = "jmp";
        info[top] = "jmp a    11 000 011  3  Unconditional jump.";
        value[top++] = (byte) 0b11000011;
        name[top] = "jnz";   //11 000 010
        info[top] = "JNZ a     11 000 010 lb hb          Jump to a, if Zero bit flag is not set (equals 0).";
        value[top++] = (byte) 0b11000010;
        name[top] = "jz";
        info[top] = "JZ a      11 001 010 lb hb          Jump to a, if zero bit flag is set (equals 1).";
        value[top++] = (byte) 0b11001010;
        name[top] = "jnc";
        info[top] = "JNC a     11 010 010 lb hb          Jump to a, if Carry bit flag is not set (equals 0).";
        value[top++] = (byte) 0b110100010;
        name[top] = "jc";
        info[top] = "JC a      11 011 010 lb hb          Jump to a, if Carry bit flag is set (equals 1).";
        value[top++] = (byte) 0b11011010;
        // ---------------------------------------------------------------------
        name[top] = "mvi";
        info[top] = "mvi R,#  00 RRR 110  2  Move a number (#, db) to a register.";
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
        info[top] = "nop      00 000 000  1  No operation. I added a delay: delay(100).";
        value[top++] = (byte) 0b00000000;
        // ---------------------------------------------------------------------
        name[top] = "out";
        info[top] = "out pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.";
        value[top++] = (byte) 0b11100011;

        System.out.println("++ Number opcode values = " + top);
    }

    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("+++ Start.");

        opcodes8080 theOpcodes = new opcodes8080();
        theOpcodes.opcodesList();
        String anOpcode = "jmp";
        System.out.println("+ Opcode: " + anOpcode + " " + printByte(theOpcodes.getOpcode(anOpcode)));

        System.out.println("+++ Exit.");
    }
}
