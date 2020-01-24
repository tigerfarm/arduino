package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

public class opcodeInfo {

    String value;
    String name;
    String info;

    // Constructor 
    public opcodeInfo(String value, String name, String info) {
        this.value = value;
        this.name = name;
        this.info = info;
    }

    // Used to print student details in main() 
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

    // -------------------------------------------------------------------------
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

}

class SortbyValue implements Comparator<opcodeInfo> {

    @Override
    public int compare(opcodeInfo a, opcodeInfo b) {
        // Need to do something about unsigned value. There's likely something over effient, but this works.
        return a.value.compareTo(b.value);
    }
}

class SortbyName implements Comparator<opcodeInfo> {

    @Override
    public int compare(opcodeInfo a, opcodeInfo b) {
        return a.name.compareTo(b.name);
    }
}

class Opcodes {

    static int opcodeCount = 3;
    static opcodeInfo[] opcodeArray = new opcodeInfo[opcodeCount];
    private int errorCount = 0;

    // -------------------------------------------------------------------------
    static public void fileLoadOpcodes(String theReadFilename) {
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
                        opcodeArray[opcodeCount++] = new opcodeInfo(value, opcode, info);
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

    static public int getOpcodeCount(String theReadFilename) {
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

    public static void main(String[] args) {
        /* First test.
        opcodeInfo[] opcodeArray2 = {
            new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA."),
            new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A."),
            new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.")
        };
        /* Second test.
        opcodeArray = new opcodeInfo[3];
        opcodeArray[0] = new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeArray[1] = new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeArray[2] = new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
         */
        // opcodeCount = 3;

        // Third set up test, using a file of data.
        opcodeCount = getOpcodeCount("opcodes8080.txt");
        System.out.println("+ opcodeCount = " + opcodeCount);
        opcodeArray = new opcodeInfo[opcodeCount];
        fileLoadOpcodes("opcodes8080.txt");

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
    }
}
