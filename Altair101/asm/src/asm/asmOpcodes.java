/*
    Retrieve and list opcode data from a text file.
    Can sort and select data before printing.

+ Hex, Octal, Binary, Decimal conversion calculator
https://coderstoolbox.net/number/

------------------------------------
Steps to add opcodes:

+ Add into asmOpcodes.txt.
-------
+ Add opcode into asmProcessor.java: parseOpcode(...), based on opcode parameters:
++ opcode (no parameters)                           example: nop
++ opcode <address label>                           example: jmp There
++ opcode <immediate>                               example: out 39
++ opcode <register|RegisterPair>                   example: cmp c
++ opcode <register>,<immediate>                    example: mvi a,1 or mvi a,aValue
++ opcode <register>,<register>                     example: mov a,b
++ opcode <register>,<address label|address number> example: lxi b,5 or lxi b,aValue
-------
+ Implement into Processor.ino. Binary values match the values in asmOpcodes.txt.

// Register identifiers (binary value=R):
//  111=A
//  000=B
//  001=C
//  010=D
//  011=E
//  100=H
//  101=L
//  110=M   Uses H:L content as an address. The content at that address, is useable data.

 */
package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode data objects.
class asmOpcode {

    String value;
    String name;
    String info;
    String logic;

    // Constructor 
    public asmOpcode(String value, String name, String info, String logic) {
        this.value = value;
        this.name = name;
        this.info = info;
        this.logic = logic;
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
        String returnString = this.value + " : " + this.name + thePadding + " : " + this.info;
        if (!this.logic.equals("")) {
            returnString = this.value + " : " + this.name + thePadding + " : " + this.logic;
        }
        return returnString;
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
    // private int errorCount = 0;
    public final byte OpcodeNotFound = (byte) 255;

    // Keep the file lines in memory for listing.
    private static final String SEPARATOR = ":";
    private int programTop = 0;
    private final static List<String> opcodeDoc = new ArrayList<>();

    // -------------------------------------------------------------------------
    public String getOpcodeFilename() {
        return this.opcodeFilename;
    }

    public void setOpcodeFilename(String theOpcodeFilename) {
        // Stacy, should validate that the file exists.
        opcodeFilename = theOpcodeFilename;
    }

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
    public void opcodesList() {
        System.out.println("+ Opcodes ordered as is, including comments.");
        System.out.println("");
        for (Iterator<String> it = opcodeDoc.iterator(); it.hasNext();) {
            String theLine = it.next();
            if ((theLine.startsWith("//") || theLine.equals(""))) {
                // Print comment and blank lines.
                System.out.println(theLine);
            } else {
                int c1 = theLine.indexOf(SEPARATOR);
                if (c1 > 0) {
                    String opcode = theLine.substring(0, c1);
                    int c2 = theLine.substring(c1 + 1).indexOf(SEPARATOR);
                    if (c2 > 0) {
                        String value = theLine.substring(c1 + 1, c1 + 8 + 1);
                        String info = theLine.substring(c1 + 8 + 1 + 1, theLine.length());
                        // System.out.println("+ opcode:" + opcode + ":" + value + ":" + info);
                        if (!info.equals("") && !info.startsWith("|")) {
                            String thePadding = "";
                            switch (opcode.length()) {
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
                            System.out.println("   " + value + " : " + opcode + thePadding + " : " + info);
                        }
                    }
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    private static void fileLoadOpcodes(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        String value;
        String info;
        String logic;
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
                // Line types:
                //  Ignore blank lines and comment lines,
                //      // Comment line
                //  Opcode information line,
                //      dad:00RP1001:DAD RP   00 RP1 001  1  16 bit add. Add register pair(RP: B:C or D:E) to H:L, into H:L. And set carry bit.
                //  Opcode logic short info line,
                //      dadb:00001001:|HL = HL + BC
                // System.out.println("+ " + theLine);
                opcodeDoc.add(theLine);
                if (!theLine.startsWith("//") && !theLine.equals("")) {
                    int c1 = theLine.indexOf(SEPARATOR);
                    if (c1 > 0) {
                        String opcode = theLine.substring(0, c1).trim();
                        int c2 = theLine.substring(c1 + 1).indexOf(SEPARATOR);
                        if (c2 > 0) {
                            value = theLine.substring(c1 + 1, c1 + 8 + 1).trim();
                            info = theLine.substring(c1 + 8 + 1 + 1, theLine.length()).trim();
                            logic = "";
                            if (info.startsWith("|")) {
                                logic = info.substring(1, info.length()).trim();
                                info = "";
                            }
                            // System.out.println("+ opcode:" + opcode + ":" + value + ":" + info + ":" + logic);
                            opcodeArray[opcodeCount++] = new asmOpcode(value, opcode, info, logic);
                        }
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

    // -------------------------------------------------------------------------
    private int getOpcodeCount(String theReadFilename) {
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
                    // String opcode = theLine.substring(0, c1);
                    int c2 = theLine.substring(c1 + 1).indexOf(SEPARATOR);
                    if (c2 > 0) {
                        opcodeCount++;
                        // String value = theLine.substring(c1 + 1, c1 + 8 + 1);
                        // String info = theLine.substring(c1 + 8 + 1 + 1, theLine.length());
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
        // Given an opcode, return the byte code.
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
        // Print a single opcode's info.
        byte returnValue = OpcodeNotFound;
        for (int i = 0; i < opcodeCount; i++) {
            if (opcodeArray[i].name.equals(theName)) {
                // System.out.println("++ " + opcodeArray[i].name + ":" + opcodeArray[i].value + ":" + opcodeArray[i].logic + ":" + opcodeArray[i].info);
                if (!opcodeArray[i].info.equals("")) {
                    System.out.println("Opcode   Binary   Cycles Description");
                    System.out.println("-------------------------------------");
                    System.out.println(opcodeArray[i].info);
                    returnValue = 1;
                } else if (!opcodeArray[i].logic.equals("")) {
                    String thePadding = "";
                    switch (opcodeArray[i].name.length()) {
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
                    System.out.println("Opcode  Binary    Short logic description");
                    System.out.println("-----------------------------------------");
                    System.out.println(opcodeArray[i].name + thePadding + "   " + opcodeArray[i].value + "  " + opcodeArray[i].logic);
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
            String description;
            if (!opcodeArray[i].logic.equals("")) {
                description = opcodeArray[i].logic;
            } else {
                description = opcodeArray[i].info;
            }
            System.out.println(
                    "++ " + counterPadding + iCounter
                    + ": " + opcodeArray[i].value
                    + " " + opcodeArray[i].name
                    + thePadding + " " + description
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
            String description;
            if (!opcodeArray[i].logic.equals("")) {
                description = opcodeArray[i].logic;
            } else {
                description = opcodeArray[i].info;
            }
            System.out.println(
                    "++ " + counterPadding + iCounter
                    + ": " + opcodeArray[i].value
                    + " " + opcodeArray[i].name
                    + thePadding + " " + description
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

        sOpcode = "dadb";
        bOpcode = theOpcodes.getOpcode(sOpcode);
        if (bOpcode == theOpcodes.OpcodeNotFound) {
            System.out.println("- Opcode, Not found: " + sOpcode + ".");
        } else {
            System.out.println("+ Opcode, " + sOpcode + " value: " + theOpcodes.byteToString(bOpcode));
        }
        System.out.println("\n-----------------------------------------------");
        theOpcodes.printOpcodeInfo("dad");
        System.out.println("");
        theOpcodes.printOpcodeInfo("dadb");

        System.out.println("\n-----------------------------------------------");
        System.out.println("");
        theOpcodes.opcodesList();
        // System.out.println("");
        // theOpcodes.opcodeInfoList();
        // System.out.println("");
        // theOpcodes.opcodesListByName();
        // System.out.println("");
        // theOpcodes.opcodesListByValue();

        System.out.println("\n+++ Exit.\n");
    }
}
