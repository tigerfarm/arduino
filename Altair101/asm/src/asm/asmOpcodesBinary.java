/*
    Retrieve and list opcode data from a text file.
    Can sort and select data before printing.

 */
package asm;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode data objects.
class asmOpcodeBinary {

    String value;
    String name;
    String info;
    String logic;

    // Constructor 
    public asmOpcodeBinary(String value, String name, String info, String logic) {
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode processing.
public class asmOpcodesBinary {

    private static asmOpcodes theOpcodes = new asmOpcodes(); // Use to get an opcode's binary value.

    private static String opcodeFilename = "asmOpcodesBinary.txt";
    static int opcodeCount = 3;
    static asmOpcode[] opcodeArray = new asmOpcode[opcodeCount];
    // private int errorCount = 0;
    public final byte OpcodeNotFound = (byte) 255;

    // Keep the file lines in memory for listing.
    private static final String SEPARATOR = ":";
    private int programTop = 0;
    private final static List<String> opcodeDoc = new ArrayList<>();

    // -------------------------------------------------------------------------
    // Constructor to initialize the opcode data.
    public asmOpcodesBinary() throws IOException {
        opcodeCount = 0;
        System.out.println("+ Assembler opcode file: " + opcodeFilename);
        System.out.println("+ Number of opcode byte values = " + opcodeCount);
        opcodeArray = new asmOpcode[opcodeCount];
        fileLoadOpcodes(opcodeFilename);
    }

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
    private static void fileLoadOpcodes(String theReadFilename) throws FileNotFoundException, IOException {
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
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            String opcode = "";
            opcodeCount = 0;
            while (theLine != null) {
                opcodeDoc.add(theLine);
                if (theLine.startsWith("0x")) {
                    // Line samples:
                    //            10        20
                    //  0123456789012345678901234567890123456789
                    //  0x02  1           STAX B     (BC) <- A
                    //  + opcode:0x02:0:STAX B     (BC) <- A:
                    //
                    //  0x03  1           INX B      BC <- BC+1 
                    //  0x04  1  Z,S,P,AC INR B      B <- B+1
                    //  0x08              -
                    value = theLine.substring(2, 4);
                    info = theLine.substring(18, theLine.length()).trim();
                    if (info.equals("-")) {
                        opcode = "-";
                    } else {
                        int c1 = info.indexOf(" ");
                        if (c1 > 0) {
                            opcode = info.substring(0, c1).trim();
                        } else {
                            opcode = info.trim();
                        }
                    }
                    logic = "";
                    String paddingD;
                    String padding = "";
                    String paddingV = "";
                    if (opcode.length() == 3) {
                        padding = " ";
                    } else if (opcode.length() == 2) {
                        padding = "  ";
                    }
                    if (opcode.equals("-")) {
                        System.out.println("++ " + value + " ---");
                    } else {
                        int decimal = Integer.parseInt(value, 16);
                        if (decimal < 10) {
                            paddingD = "00";
                        } else if (decimal < 100) {
                            paddingD = "0";
                        } else {
                            paddingD = "";
                        }
                        String theOpcodeValue = theOpcodes.getOpcodeValue(decimal);
                        if (theOpcodeValue.equals("")) {
                            theOpcodeValue = "---";
                        }
                        switch (theOpcodeValue.length()) {
                            case 4:
                                paddingV = " ";
                                break;
                            case 3:
                                paddingV = "  ";
                                break;
                            case 2:
                                paddingV = "   ";
                                break;
                            default:
                                break;
                        }
                        System.out.print("++ " + value + " "
                                + paddingD + decimal + " "
                                + byteToString((byte)decimal) + " "
                                + opcode + padding
                                + " "+ theOpcodeValue + paddingV
                                + " :" + info + ":"
                        );
                        System.out.print(":" + theOpcodes.getOpcodeValue(decimal) + ":");
                        System.out.println("");
                    }
                    // opcodeArray[opcodeCount++] = new asmOpcode(value, opcode, info, logic);
                }
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
        /*
        String opcode = "hlt";
        byte opcodeBinary;
        opcodeBinary = theOpcodes.getOpcode(opcode);
        System.out.println("++ " + opcode + " :" + opcodeBinary + ":" + theOpcodes.getOpcodeValue(opcodeBinary));
        // theOpcodes.getOpcodeValue(opcodeBinary);
        */
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) throws IOException {

        System.out.println("+++ Start.");
        asmOpcodesBinary theOpcodesBin = new asmOpcodesBinary();
        System.out.println("\n-----------------------------------------------");
        System.out.println("+ Find sample values.\n");

        System.out.println("");
        // theOpcodes.opcodesListByValue();
        System.out.println("\n+++ Exit.\n");
    }
}
