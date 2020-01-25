/*
    Retrieves opcode data from a text.
    Can sort the data before printing.

--------------------------------------------------------------------------------
Opcodes implemented by this assembler, implemented in Processor.ino, and tested.
Opcode   Binary   Cycles Description
-------------------------------------
adi #    11 000 110     Add immediate number to register A, and set ZSCPA.
ani #    11 100 110  2  AND # (immediate db) with register A.
call a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.
cmp S    10 111 SSS     Compare register(S) with register A, then set flags. If S=A, set Zero bit to 1. If S>A, Carry bit = 1. If S<A, Carry bit = 0.
cpi #    11 111 110  2  Compare # to A. Store true or false into flagZeroBit.
hlt      01 110 110  1  Halt processor.
in pa    11 011 011  2  Read port a data into the accumulator. Example, a=0377 is the sense switches.
jnc a    11 010 010  3  Jump if not carry bit, i.e. if carry bit value is 0, false, not set.
jmp a    11 000 011  3  Unconditional jump.
jz  a    11 001 010  3  If flagZeroBit is true, jump to address (a = lb hb).
mov D,S  01 DDD SSS  1  Move source register data, to the destination register.
mvi D,#  00 DDD 110  2  Move a number (#/db) to a register.
nop      00 000 000  1  No operation. I added a delay: delay(100).
ora S    10 110 SSS  1  OR register S, with register A.
out pa   11 010 011  2  Write the accumulator data out to port a. I'm using this opcode to write custom log messages such as echoing the registers.
rlc      00 000 111  1  Rotate accumulator left by shift left 1 bit, and wrapping the first bit to the last position. Need to handle carry bit (CY = prev bit 7).
rrc      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit (CY = prev bit 0).
ret      11 001 001  1  Unconditional return from subroutine. Pop the call address from the stack and continue to the next address.
sui #    11 010 110     ZSCPA Subtract immedite number from register A.
xra S    10 101 SSS  1  Exclusive OR, the register(R) with register A.

Opcodes implemented by this assembler, implemented in Processor.ino, but not fully tested.
Opcode   Binary   Cycles Description
-------------------------------------
lxi RP,a 00 RP0 001  3  Move the data at the address, a(lb hb), into address of the register pair: B:C, D:E, or H:L. To do: move data to the stack pointer address.
dad RP   00 RP1 001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L. And set carry bit.
ldax RP  00 RP1 010  1  Load data value at the register pair address (B:C(RP=00) or D:E(RP=01)), into register A.
inr D    00 DDD 100  1  Increment a register. To do, set flags: ZSPA.
dcr D    00 DDD 101  1  Decrement a register. To do, set flags: ZSPA.
lda a    00 111 010  3  Load register A with data from the address, a(hb:lb).
sta a    00 110 010  3  Store register A to the address, a(hb:lb).
inx RP   00 RP0 011  1  Increment a register pair(16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.

shld a   00 100 010  3  Store data value from memory location: a(address hb:lb), to register L. Store value at: a + 1, to register H.

Opcodes implemented in Processor.ino, but not yet in this assembler,
Opcode   Binary   Cycles Description
-------------------------------------
push RP  11 RP0 101  1  Push register pair on the stack.
pop  RP  11 RP0 001  1  POP register pair from the stack.

+ Hex, Octal, Binary, Decimal conversion calculator
https://coderstoolbox.net/number/
 */
package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode data objects.
class asmOpcode {

    String value;
    String name;
    String info;

    // Constructor 
    public asmOpcode(String value, String name, String info) {
        this.value = value;
        this.name = name;
        this.info = info;
    }

    // Used to print the data.
    @Override
    public String toString() {
        String thePadding = "";
        switch (this.name.length()) {
            case 2:
                thePadding = "   ";
                break;
            case 3:
                thePadding = "  ";
                break;
            case 4:
                thePadding = " ";
                break;
            default:
                break;
        }
        return this.value + " : " + this.name + thePadding + " : " + this.info;
    }

}

class SortbyValue implements Comparator<asmOpcode> {

    @Override
    public int compare(asmOpcode a, asmOpcode b) {
        return a.value.compareTo(b.value);
    }
}

class SortbyName implements Comparator<asmOpcode> {

    @Override
    public int compare(asmOpcode a, asmOpcode b) {
        return a.name.compareTo(b.name);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode processing.
public class asmOpcodes {

    private static String opcodeFilename = "asmOpcodes.txt";
    static int opcodeCount = 3;
    static asmOpcode[] opcodeArray = new asmOpcode[opcodeCount];
    private int errorCount = 0;
    public final byte OpcodeNotFound = (byte) 255;

    // -------------------------------------------------------------------------
    // Constructor to initialize the opcode data.
    public asmOpcodes() {
        opcodeCount = getOpcodeCount(opcodeFilename);
        System.out.println("+ Assembler opcode file: " + opcodeFilename);
        System.out.println("+ Number of opcode byte values = " + opcodeCount);
        opcodeArray = new asmOpcode[opcodeCount];
        fileLoadOpcodes(opcodeFilename);
    }

    // -------------------------------------------------------------------------
    private static void fileLoadOpcodes(String theReadFilename) {
        String SEPARATOR = ":";
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            // Get a count of the number of opcodes.
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                // errorCount++;
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            opcodeCount = 0;
            while (theLine != null) {
                // System.out.println("+ " + theLine);
                int c1 = theLine.indexOf(SEPARATOR);
                if (c1 > 0) {
                    String opcode = theLine.substring(0, c1);
                    int c2 = theLine.substring(c1 + 1).indexOf(SEPARATOR);
                    if (c2 > 0) {
                        String value = theLine.substring(c1 + 1, c1 + 8 + 1);
                        String info = theLine.substring(c1 + 8 + 1 + 1, theLine.length());
                        // System.out.println("+ opcode:" + opcode + ":" + value + ":" + info);
                        opcodeArray[opcodeCount++] = new asmOpcode(value, opcode, info);
                    }
                }
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
    }

    private int getOpcodeCount(String theReadFilename) {
        String SEPARATOR = ":";
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            // Get a count of the number of opcodes.
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                // errorCount++;
                return 0;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            opcodeCount = 0;
            while (theLine != null) {
                // System.out.println("+ " + theLine);
                int c1 = theLine.indexOf(SEPARATOR);
                if (c1 > 0) {
                    String opcode = theLine.substring(0, c1);
                    int c2 = theLine.substring(c1 + 1).indexOf(SEPARATOR);
                    if (c2 > 0) {
                        opcodeCount++;
                        String value = theLine.substring(c1 + 1, c1 + 8 + 1);
                        String info = theLine.substring(c1 + 8 + 1 + 1, theLine.length());
                        // System.out.println("+ opcode:" + opcode + ":" + value + ":" + info);
                    }
                }
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
        return opcodeCount;
    }

    // -------------------------------------------------------------------------
    public String byteToString(byte aByte) {
        return toBinary(aByte, 8);
    }

    private String toBinary(byte a, int bits) {
        if (--bits > 0) {
            return toBinary((byte) (a >> 1), bits) + ((a & 0x1) == 0 ? "0" : "1");
        } else {
            return (a & 0x1) == 0 ? "0" : "1";
        }
    }

    // -------------------------------------------------------------------------
    public byte getOpcode(String theName) {
        byte returnValue = OpcodeNotFound;
        for (int i = 0; i < opcodeCount; i++) {
            // System.out.println("++ " + i + ": " + name[i] + " " + value[i]);
            if (opcodeArray[i].name.equals(theName)) {
                returnValue = (byte) Integer.parseInt(opcodeArray[i].value, 2);
                break;
            }
        }
        return returnValue;
    }

    public void printOpcodeInfo(String theName) {
        byte returnValue = OpcodeNotFound;
        for (int i = 0; i < opcodeCount; i++) {
            if (opcodeArray[i].name.equals(theName)) {
                // System.out.println("++ "+ name[i] + " " + byteToString(value[i]));
                if (!opcodeArray[i].info.equals("")) {
                    System.out.println("Opcode   Binary   Cycles Description");
                    System.out.println("-------------------------------------");
                    System.out.println(opcodeArray[i].info);
                    returnValue = 1;
                }
                break;
            }
        }
        if (returnValue == OpcodeNotFound) {
            System.out.println("++ Opcode Not Found: " + theName);
        }
    }

    // -------------------------------------------------------------------------
    public void opcodeInfoList() {
        Arrays.sort(opcodeArray, new SortbyName());
        System.out.println("+ List opcode info ordered by opcode name.");
        System.out.println("Binary             Opcode   Binary   Cycles Description");
        System.out.println("-------------------------------------");
        //                  11100110 : ani   : ANI #    11 100 110  2  AND # (immediate db) with register A.
        for (int i = 0; i < opcodeCount; i++) {
            if (!opcodeArray[i].info.equals("")) {
                System.out.println(opcodeArray[i]);
            }
        }
        System.out.println("+ End list.");
    }

    // -------------------------------------------------------------------------
    public void opcodesList() {
        System.out.println("+ List opcode data.");
        System.out.println("        Name: Value");
        //                  ++   1: adi   11000110
        System.out.println("-------------------------------------");
        int iCounter = 0;
        String counterPadding;
        for (int i = 0; i < opcodeCount; i++) {
            iCounter++;
            counterPadding = "";
            if (iCounter < 10) {
                counterPadding = "  ";
            } else if (iCounter < 100) {
                counterPadding = " ";
            }
            String thePadding = " ";
            switch (opcodeArray[i].name.length()) {
                case 2:
                    thePadding = "    ";
                    break;
                case 3:
                    thePadding = "   ";
                    break;
                case 4:
                    thePadding = "  ";
                    break;
                default:
                    break;
            }

            System.out.println(
                    "++ " + counterPadding + iCounter
                    + ": " + opcodeArray[i].name
                    + thePadding + opcodeArray[i].value
                    + " " + opcodeArray[i].info
            );
        }
        System.out.println("+ End list.");
    }

    // -------------------------------------------------------------------------
    public void opcodesListByName() {
        Arrays.sort(opcodeArray, new SortbyName());
        System.out.println("+ List opcode data ordered by opcode name.");
        System.out.println("        Name: Value");
        //                  ++   1: adi   11000110
        System.out.println("-------------------------------------");
        int iCounter = 0;
        String counterPadding;
        for (int i = 0; i < opcodeCount; i++) {
            iCounter++;
            counterPadding = "";
            if (iCounter < 10) {
                counterPadding = "  ";
            } else if (iCounter < 100) {
                counterPadding = " ";
            }
            String thePadding = " ";
            switch (opcodeArray[i].name.length()) {
                case 2:
                    thePadding = "    ";
                    break;
                case 3:
                    thePadding = "   ";
                    break;
                case 4:
                    thePadding = "  ";
                    break;
                default:
                    break;
            }

            System.out.println(
                    "++ " + counterPadding + iCounter
                    + ": " + opcodeArray[i].name
                    + thePadding + opcodeArray[i].value
                    + " " + opcodeArray[i].info
            );
        }
        System.out.println("+ End list.");
    }

    // -------------------------------------------------------------------------
    public void opcodesListByValue() {
        Arrays.sort(opcodeArray, new SortbyValue());
        System.out.println("+ List opcode data ordered by value.");
        System.out.println("        Binary          Opcode   Binary   Cycles Description");
        //                  ++   1: 00000000 nop    NOP      00 000 000  1  No operation.
        System.out.println("-------------------------------------");
        int iCounter = 0;
        String counterPadding;
        for (int i = 0; i < opcodeCount; i++) {
            iCounter++;
            counterPadding = "";
            if (iCounter < 10) {
                counterPadding = "  ";
            } else if (iCounter < 100) {
                counterPadding = " ";
            }
            String thePadding = " ";
            switch (opcodeArray[i].name.length()) {
                case 2:
                    thePadding = "    ";
                    break;
                case 3:
                    thePadding = "   ";
                    break;
                case 4:
                    thePadding = "  ";
                    break;
                default:
                    break;
            }

            System.out.println(
                    "++ " + counterPadding + iCounter
                    + ": " + opcodeArray[i].value
                    + " " + opcodeArray[i].name
                    + thePadding+ " " + opcodeArray[i].info
            );
        }
        System.out.println("+ End list.");
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) {

        System.out.println("+++ Start.");

        /* First setup.
        asmOpcode[] opcodeArray2 = {
            new asmOpcode((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA."),
            new asmOpcode((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A."),
            new asmOpcode((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.")
        };
        /* Second setup.
        opcodeArray = new asmOpcode[3];
        opcodeArray[0] = new asmOpcode((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeArray[1] = new asmOpcode((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeArray[2] = new asmOpcode((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
         */
        // opcodeCount = 3;
        // Third setup, using a file of data and non-static methods.
        asmOpcodes theOpcodes = new asmOpcodes();

        System.out.println("\n-----------------------------------------------");
        System.out.println("+ Test retrieve methods.\n");
        //
        String sOpcode = "jmp";
        theOpcodes.printOpcodeInfo(sOpcode);
        //
        byte bOpcode = theOpcodes.getOpcode(sOpcode);
        if (bOpcode == theOpcodes.OpcodeNotFound) {
            System.out.println("\n- Opcode, Not found: " + sOpcode + ".");
        } else {
            System.out.println("\n+ Opcode, " + sOpcode + " value: " + theOpcodes.byteToString(bOpcode));
        }
        //
        sOpcode = "jmpx";
        bOpcode = theOpcodes.getOpcode(sOpcode);
        if (bOpcode == theOpcodes.OpcodeNotFound) {
            System.out.println("- Opcode, Not found: " + sOpcode + ".");
        } else {
            System.out.println("+ Opcode, " + sOpcode + " value: " + theOpcodes.byteToString(bOpcode));
        }

        System.out.println("\n-----------------------------------------------");
        System.out.println("");
        theOpcodes.opcodeInfoList();
        System.out.println("");
        theOpcodes.opcodesList();
        System.out.println("");
        theOpcodes.opcodesListByName();
        System.out.println("");
        theOpcodes.opcodesListByValue();

        System.out.println("\n+++ Exit.\n");
    }
}
