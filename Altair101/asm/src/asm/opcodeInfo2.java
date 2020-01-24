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
public class opcodeInfo2 {

    int value;
    String name, info;

    // Constructor 
    public opcodeInfo2(byte value, String name, String info) {
        this.value = value;
        this.name = name;
        this.info = info;
    }

    @Override
    public String toString() {
        return byteToString((byte) this.value)
                + ": " + this.name
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

class SortbyValue2 implements Comparator<opcodeInfo2> {

    @Override
    public int compare(opcodeInfo2 a, opcodeInfo2 b) {
        return a.value - b.value;
    }
}

class SortbyName2 implements Comparator<opcodeInfo2> {

    @Override
    public int compare(opcodeInfo2 a, opcodeInfo2 b) {
        return a.name.compareTo(b.name);
    }
}

// -----------------------------------------------------------------------------
// For testing.
class Opcodes2 {

    static opcodeInfo2[] opcodeArray = new opcodeInfo2[255];
    static int opcodeCount;
    private int errorCount = 0;

    // -------------------------------------------------------------------------
    public void fileLoadOpcodes(String theReadFilename) {
        String SEPARATOR = ":";
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            // Get a count of the number of opcodes.
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                errorCount++;
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
                        opcodeArray[opcodeCount++] = new opcodeInfo2((byte) Integer.parseInt(value,2), opcode, info);
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
        /*
        opcodeInfo[] arr = new opcodeInfo[3];
        arr[0] = new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        arr[1] = new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        arr[2] = new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
         */
        // Third declaration iteration, load from a file:
        System.out.println("\n+ List opcode data text file.");
        Opcodes opcodeCodes = new Opcodes();
        //
        // opcodeCodes.fileLoadOpcodes("opcodes8080.txt");
        /*
        */
        opcodeArray[0] = new opcodeInfo2((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeArray[1] = new opcodeInfo2((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeArray[2] = new opcodeInfo2((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
        opcodeCount = 3;
        Arrays.sort(opcodeArray, new SortbyName2());
        System.out.println("\n+ List sorted by name.");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
        /*
        opcodeCodes.opcodeArray[0] = new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeCodes.opcodeArray[1] = new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeCodes.opcodeArray[2] = new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
        opcodeCodes.opcodeCount = 3;
        // ---------------------------------------------------------------------
        System.out.println("\n+ List unsorted");
        for (int i = 0; i < opcodeCodes.opcodeCount; i++) {
            System.out.println(opcodeCodes.opcodeArray[i]);
        }
        Arrays.sort(opcodeCodes.opcodeArray, new SortbyName());
        System.out.println("\n+ List sorted by name.");
        for (int i = 0; i < opcodeCodes.opcodeCount; i++) {
            System.out.println(opcodeCodes.opcodeArray[i]);
        }
        Arrays.sort(opcodeCodes.opcodeArray, new SortbyValue());
        System.out.println("\n+ List sorted by value.");
        for (int i = 0; i < opcodeCodes.opcodeCount; i++) {
            System.out.println(opcodeCodes.opcodeArray[i]);
        }
*/
    }
}
