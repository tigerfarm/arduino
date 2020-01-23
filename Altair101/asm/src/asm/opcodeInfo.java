package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

// References:
//  https://www.geeksforgeeks.org/arrays-in-java/
//  https://www.edureka.co/blog/array-of-objects-in-java/
// -----------------------------------------------------------------------------
public class opcodeInfo {

    int value;
    String name, info;

    // Constructor 
    public opcodeInfo(byte value, String name, String info) {
        this.value = value;
        this.name = name;
        this.info = info;
    }

    @Override
    public String toString() {
        return byteToString((byte) this.value)
                + " " + this.name
                + ": " + this.info;
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
        return a.value - b.value;
    }
}

class SortbyName implements Comparator<opcodeInfo> {

    @Override
    public int compare(opcodeInfo a, opcodeInfo b) {
        return a.name.compareTo(b.name);
    }
}

// -----------------------------------------------------------------------------
// For testing.
class Opcodes {

    private int errorCount = 0;

    // -------------------------------------------------------------------------
    public void fileLoadOpcodes(String theReadFilename) {
        String SEPARATOR = ":";
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                errorCount++;
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            while (theLine != null) {
                // System.out.println("+ " + theLine);
                int c1 = theLine.indexOf(":");
                if (c1 > 0) {
                    String opcode = theLine.substring(0, c1);
                    int c2 = theLine.substring(c1 + 1).indexOf(":");
                    if (c2 > 0) {
                        String value = theLine.substring(c1+1,c1+8+1);
                        String info = theLine.substring(c1+8+1+1, theLine.length());
                        System.out.println("+ opcode:" + opcode + ":" + value + ":" + info);
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

    public static void main(String[] args) {

        // Need a method to load the data from a text file.
        // Declare and initialize. 
        /*
        // First declaration iteration:
        opcodeInfo[] arr = {
            new opcodeInfo((byte)0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA."),
            new opcodeInfo((byte)0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A."),
            new opcodeInfo((byte)0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.")
        };
         */
        // Second declaration iteration:
        opcodeInfo[] arr = new opcodeInfo[3];
        arr[0] = new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        arr[1] = new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        arr[2] = new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");

        System.out.println("\n+ List opcode data text file.");
        Opcodes opcodeCodes = new Opcodes();
        opcodeCodes.fileLoadOpcodes("opcodes8080.txt");
        // parseOpcodeData("opcodes8080.txt");

        // ---------------------------------------------------------------------
        System.out.println("\n+ List unsorted");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
        Arrays.sort(arr, new SortbyName());
        System.out.println("\n+ List sorted by name.");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
        Arrays.sort(arr, new SortbyValue());
        System.out.println("\n+ List sorted by value.");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }

    }
}
