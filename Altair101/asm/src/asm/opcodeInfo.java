package asm;

import java.util.Arrays;
import java.util.Comparator;

public class opcodeInfo {

    public byte value;
    public String name;
    public String info;

    // Constructor 
    public opcodeInfo(byte value, String name, String info) {
        this.value = value;
        this.name = name;
        this.info = info;
    }

    // Used to print student details in main() 
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

class Opcodes {

    // static opcodeInfo[] opcodeArray = new opcodeInfo[2];
    static int opcodeCount;
    private int errorCount = 0;

    /*
    /*
     */
    public static void main(String[] args) {
        /*
        opcodeInfo[] opcodeArray = new opcodeInfo[255];
        opcodeArray[0] = new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA.");
        opcodeArray[1] = new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A.");
        opcodeArray[2] = new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.");
         */
        opcodeInfo[] opcodeArray = {
            new opcodeInfo((byte) 0b11000110, "adi", "ADI #    11 000 110  3  Add immediate number to register A, set: ZSCPA."),
            new opcodeInfo((byte) 0b11100110, "ani", "ANI #    11 100 110  2  AND # (immediate db) with register A."),
            new opcodeInfo((byte) 0b11001101, "call", "CALL a   11 001 101  3  Unconditional subroutine call. Push current address onto the stack and jump the subroutine address.")
        };

        opcodeCount = 3;
        System.out.println("Unsorted");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
        Arrays.sort(opcodeArray, new SortbyValue());
        System.out.println("\nSorted by rollno");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
        Arrays.sort(opcodeArray, new SortbyName());
        System.out.println("\nSorted by name");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println(opcodeArray[i]);
        }
    }
}
