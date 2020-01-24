package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

public class asmOpcodes {

    String value;
    String name;
    String info;

    // Constructor 
    public asmOpcodes(String value, String name, String info) {
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

class SortbyValue implements Comparator<asmOpcodes> {

    @Override
    public int compare(asmOpcodes a, asmOpcodes b) {
        // Need to do something about unsigned value. There's likely something over effient, but this works.
        return a.value.compareTo(b.value);
    }
}

class SortbyName implements Comparator<asmOpcodes> {

    @Override
    public int compare(asmOpcodes a, asmOpcodes b) {
        return a.name.compareTo(b.name);
    }
}

class Opcodes {

    static int opcodeCount = 3;
    static asmOpcodes[] opcodeArray = new asmOpcodes[opcodeCount];
    private int errorCount = 0;
    public final byte OpcodeNotFound = (byte) 255;

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
                        opcodeArray[opcodeCount++] = new asmOpcodes(value, opcode, info);
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

    private static int getOpcodeCount(String theReadFilename) {
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
    public static void main(String[] args) {
        
        System.out.println("+++ Start.");

        /* First test.
        asmOpcodes[] opcodeArray2 = {
            new asmOpcodes((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA."),
            new asmOpcodes((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A."),
            new asmOpcodes((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.")
        };
        /* Second test.
        opcodeArray = new asmOpcodes[3];
        opcodeArray[0] = new asmOpcodes((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeArray[1] = new asmOpcodes((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeArray[2] = new asmOpcodes((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
         */
        // opcodeCount = 3;

        // Third set up test, using a file of data.
        Opcodes theOpcodes = new Opcodes();

        opcodeCount = theOpcodes.getOpcodeCount("asmOpcodes.txt");
        System.out.println("+ opcodeCount = " + opcodeCount);
        opcodeArray = new asmOpcodes[opcodeCount];
        fileLoadOpcodes("asmOpcodes.txt");
        
        String sOpcode = "jmp";
        byte bOpcode = theOpcodes.getOpcode(sOpcode);
        if (bOpcode == theOpcodes.OpcodeNotFound) {
            System.out.println("\n- Not found, Opcode, " + sOpcode + ".");            
        } else {
            System.out.println("\n+ Opcode, " + sOpcode + " value: " + theOpcodes.byteToString(bOpcode));
        }

        Arrays.sort(opcodeArray, new SortbyValue());
        System.out.println("\n+ Sorted by value.");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
        /*
        System.out.println("\n+ Unsorted list of opcode data.");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
        Arrays.sort(opcodeArray, new SortbyName());
        System.out.println("\n+ Sorted by name.");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
         */
        
        System.out.println("\n+++ Exit.\n");
    }
}
