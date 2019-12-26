package asm;

public class opcodes8080 {

    private String[] name;
    private byte[] value;
    private int top = 0;

    public opcodes8080() {
        setOpcodeData();
    }

    public byte getOpcode(String theName) {
        byte returnValue = 0;
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
        value = new byte[255];
        // ---------------------------------------------------------------------
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
        value[top++] = (byte) 0b01110110;
        // ---------------------------------------------------------------------
        name[top] = "jmp";
        value[top++] = (byte) 0b11000011;
        name[top] = "jnz";   //11 000 010
        value[top++] = (byte) 0b11000010;
        name[top] = "jz";
        value[top++] = (byte) 0b11001010;
        name[top] = "jnc";
        value[top++] = (byte) 0b110100010;
        name[top] = "jc";
        value[top++] = (byte) 0b11011010;
        // ---------------------------------------------------------------------
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
        value[top++] = (byte) 0b00000000;
        // ---------------------------------------------------------------------
        name[top] = "out";
        value[top++] = (byte) 0b11100011;

        System.out.println("++ Number opcode values = " + top);
    }

    public void opcodesList() {
        System.out.println("+ List opcode data.");
        for (int i = 0; i < top; i++) {
            System.out.println("++ " + i + ": " + name[i] + " " + value[i]);
        }
        System.out.println("+ End list.");
    }

    public static String toBinary(byte a, int bits) {
    if (--bits > 0)
        return toBinary((byte) (a>>1), bits)+((a&0x1)==0?"0":"1");
    else 
        return (a&0x1)==0?"0":"1";
  }

    public static void main(String args[]) {
        System.out.println("+++ Start.");

        opcodes8080 theOpcodes = new opcodes8080();
        //theOpcodes.opcodesList();
        String anOpcode = "jmp";
        // Integer.toString(100,8) // prints 144 --octal representation
        // Integer.toString(100,2) // prints 1100100 --binary representation
        // Integer.toString(100,16) //prints 64 --Hex representation
        System.out.println("+ Opcode: " + anOpcode + " " + toBinary(theOpcodes.getOpcode(anOpcode), 8));

        System.out.println("+++ Exit.");
    }
}
