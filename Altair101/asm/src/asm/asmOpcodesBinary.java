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
import java.util.Iterator;
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
    private static int opcodesTop = 0;
    private final static List<String> opcodes = new ArrayList<>();
    private final static List<String> opcodesType = new ArrayList<>();

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
    /*
    opcodesTop

    ++;
    opcodes.add (opcode);

    opcodesType.add (opcodeType);
     */
    public static String getOpcodeType(String theOpcode) {
        // Given an opcode, return the byte code.
        String returnValue = "";
        Iterator<String> iTypes = opcodesType.iterator();
        for (Iterator<String> it = opcodes.iterator(); it.hasNext();) {
            String theOpcodes = it.next();
            String theType = iTypes.next();
            if (theOpcodes.equals(theOpcode)) {
                returnValue = theType;
                break;
            }
        }
        return returnValue;
    }

    private static void fileLoadOpcodeSyntax(String theReadFilename) throws FileNotFoundException, IOException {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        String value;
        String info;
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
            String opcode;
            String opcodeType = "";
            opcodeCount = 0;
            // System.out.println("\n+ List opcode syntax.");
            while (theLine != null) {
                theLine = theLine.trim();
                if (theLine.startsWith("// opcode, no parameters")) {
                    opcodeType = "NA";
                    // System.out.println("++ opcode, no parameters");
                }
                if (theLine.startsWith("// opcode <address label>")) {
                    opcodeType = "adr";
                    // System.out.println("++ opcode <address label>");
                }
                if (theLine.startsWith("// opcode <immediate>")) {
                    opcodeType = "D8";
                    // System.out.println("++ opcode <immediate>");
                }
                if (theLine.startsWith("// opcode <register|RegisterPair>")) {
                    opcodeType = "R";
                    // System.out.prinlnt("++ opcode <register|RegisterPair>");
                }
                /*
                if (theLine.startsWith("case \"mvi\":")) {
                    System.out.print("++ opcode MVI");
                    theLine = pin.readLine();
                    System.out.print(" : " + theLine.trim());
                    System.out.println("");
                }
                if (theLine.startsWith("case \"mov\":")) {
                    System.out.print("++ opcode MOV");
                    theLine = pin.readLine();
                    System.out.print(" : " + theLine.trim());
                    System.out.println("");
                }
                if (theLine.startsWith("case \"lxi\":")) {
                    System.out.print("++ opcode LXI");
                    theLine = pin.readLine();
                    System.out.print(" : " + theLine.trim());
                    System.out.println("");
                }
                 */
                if (theLine.equals("break;")) {
                    opcodeType = "";
                }
                if (theLine.startsWith("case \"") && !opcodeType.equals("")) {
                    // Get the opcode.
                    // 01234567890
                    // case "hlt":
                    int c1 = theLine.indexOf("\"", 7);
                    if (c1 > 7) {
                        opcode = theLine.substring(6, c1);
                        opcodesTop++;
                        opcodes.add(opcode);
                        opcodesType.add(opcodeType);
                    } else {
                        opcode = "---";
                    }
                    // System.out.println("++ opcode :" + opcode + ":" + opcodeType);
                }
                //
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("+++ Opcode types loaded, count = " + opcodesTop);
        // System.out.println("+++ getOpcodeType(\"hlt\") = " + getOpcodeType("hlt"));
        // System.out.println("+++ getOpcodeType(\"jmp\") = " + getOpcodeType("jmp"));
        // System.out.println("+++ getOpcodeType(\"out\") = " + getOpcodeType("out"));
        // System.out.println("+++ getOpcodeType(\"cmp\") = " + getOpcodeType("cmp"));
    }

    // -------------------------------------------------------------------------
    private static void fileLoadOpcodes(String theReadFilename) throws FileNotFoundException, IOException {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        String value;
        String info;
        String logic;

        fileLoadOpcodeSyntax("src/asm/asmProcessor.java");

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
                    String opcodeSyntax = "";
                    String paddingD;
                    String padding = "";
                    String paddingV = "";
                    String paddingT = "";
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
                        String infoLow = info.replaceAll(" ", "").toLowerCase();
                        infoLow = infoLow.replaceAll(",", "").toLowerCase();
                        String doCheck = " ";
                        if (!infoLow.startsWith(theOpcodeValue)) {
                            if (!theOpcodeValue.equals("pusha") && !theOpcodeValue.equals("popa")) {
                                doCheck = "*";
                            }
                        }
                        if (theOpcodeValue.startsWith("mov")) {
                            // ++ 77 119 01110111 MOV  movma * :MOV M,A    (HL) <- A:
                        }
                        String theType = getOpcodeType(opcode.toLowerCase());
                        switch (theType.length()) {
                            case 0:
                                paddingT = "   ";
                                break;
                            case 1:
                                paddingT = "  ";
                                break;
                            case 2:
                                paddingT = " ";
                                break;
                            default:
                                break;
                        }
                        // Confirm the parameter type.
                        //  0123456789012345678901234567890123456789
                        //  0x04  1  Z,S,P,AC INR B      B <- B+1
                        String doCheck2 = "  ";
                        opcodeSyntax = theLine.substring(18, 28).trim();
                        if (theType.equals("NA") && !opcode.toLowerCase().equals(theOpcodeValue)) {
                            // 0x07  1  CY       RLC        A = A ...
                            // ++ 07 007 00000111 RLC  rlc     NA  :RLC        A = A ...
                            doCheck2 = "- ";
                        }
                        int c1 = theLine.indexOf(" ", 7);
                        if (c1 > 0) {
                            // 0x04  1  Z,S,P,AC INR B      B <- B+1 
                            String param = theLine.substring(c1, 28);
                            if (!theType.equals("R")) {
                                doCheck2 = "  ";
                                // doCheck2 = "-p";
                            }
                        } else {
                            // Error because type is NA.
                            doCheck2 = "--";
                        }
                        System.out.print("++ " + value + " "
                                + paddingD + decimal + " "
                                + byteToString((byte) decimal) + " "
                                + opcode + padding
                                + " " + theOpcodeValue + paddingV
                                + " " + doCheck
                                + " " + theType + paddingT
                                + " " + doCheck2
                                + " :" + info + ":"
                        );
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
