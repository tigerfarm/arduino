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

    static final String OPCODE_FILENAME = "asmOpcodesBinary.txt";
    static int opcodeCount = 3;
    //static asmOpcode[] opcodeArray = new asmOpcode[opcodeCount];
    // private int errorCount = 0;
    public final byte OpcodeNotFound = (byte) 255;

    // Keep the file lines in memory for listing.
    private static int opcodesTop = 0;
    private final static List<String> opcodes = new ArrayList<>();
    private final static List<String> opcodesType = new ArrayList<>();

    // ++ 006 00000110 MVI   mvib  b,d8   1           :B <- byte 2
    private static int opcodesDbTop = 0;

    // -------------------------------------------------------------------------
    // Constructor to initialize the opcode data.
    public asmOpcodesBinary() throws IOException {
        opcodeCount = 0;
        System.out.println("+ Assembler opcode file: " + OPCODE_FILENAME);
        System.out.println("+ Number of opcode byte values = " + opcodeCount);
        //opcodeArray = new asmOpcode[opcodeCount];
        fileLoadOpcodes(OPCODE_FILENAME);
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
        // String value;

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
            String opcodeOctal;
            String info;
            String opcode;
            String opcodeInfo;
            String opcodeSyntaxParameters;
            String opcodeParameterType;
            opcodeCount = 0;
            String doCheck2 = "  ";
            String paddingO = "";
            String paddingD;
            String paddingP = "";
            String paddingT = "";
            String paddingV = "";
            System.out.println("Decimal  Binary  Opcode    Parameters Type     Description");
            String theLine = pin.readLine().trim();
            while (theLine != null) {
                if (theLine.startsWith("0x")) {
                    theLine = theLine.trim();
                    //
                    // Process a Opcode syntax line from, asmOpcodesBinary.txt.
                    //
                    // Opcode syntax line samples with various parameter types:
                    //      0x08              -
                    //      0x07  1  CY       RLC        A = A << 1;          NA
                    //      0x02  1           STAX B     (BC) <- A            R
                    //      0x03  1           INX B      BC <- BC+1           R
                    //      0x04  1  Z,S,P,AC INR B      B <- B+1             R
                    //      0xc7  1           RST 0      CALL $0              D8
                    //      0x4a  1           MOV C,D    C <- D     
                    //      0x2e  2           MVI L, D8  L <- byte 2
                    //      0x31  3           LXI SP,D16 SP.hi <- byte 3, SP.lo <- byte 2
                    //
                    opcodeOctal = theLine.substring(2, 4);
                    int opcodeOctalDecimal = Integer.parseInt(opcodeOctal, 16);
                    if (opcodeOctalDecimal < 10) {
                        paddingD = "00";
                    } else if (opcodeOctalDecimal < 100) {
                        paddingD = "0";
                    } else {
                        paddingD = "";
                    }
                    if (theLine.length() < 20) {
                        // Special line type:
                        //      0x08              -
                        System.out.println("++ "
                                + paddingD + opcodeOctalDecimal + " "
                                + byteToString((byte) opcodeOctalDecimal) + " "
                                + "-"
                        );
                    } else {
                        String opcodeSyntax = theLine.substring(18, 28);
                        //
                        // ---------------------------------------------------------
                        // Sample:
                        //               10        20      28
                        //      0123456789012345678901234567890123456789
                        //      0x03  1           INX B      BC <- BC+1 
                        //      0x08              -
                        //      0x03  1           INX B      BC <- BC+1           R
                        //  Components:
                        //      opcodeOctal                 03
                        //      opcodeDecimal               3
                        //      opcodeSyntax =              INX B
                        //      opcode =                    INX
                        //      opcodeSyntaxParameters =    B
                        //      opcodeBytesFollowing        0
                        //      opcodeSyntaxValue           inxb (opcodeSyntax: lowercase, spaces and commas removed)
                        //      opcodeParameterType         R
                        //      opcodeInfo =                BC <- BC+1
                        // Output:
                        //      ++ 003 00000011 INX   inxb  b         R       :BC <- BC+1
                        //
                        //  MVI L, D8
                        //      opcodeSyntaxValue = mvild8
                        //
                        opcodeSyntaxParameters = "";
                        int opcodeBytesFollowing = 0;
                        String opcodeBytesStr = " ";
                        int c1 = opcodeSyntax.indexOf(" ");
                        if (c1 > 0) {
                            opcode = opcodeSyntax.substring(0, c1).trim();
                            opcodeSyntaxParameters = opcodeSyntax.substring(c1 + 1, 10).trim();
                            opcodeSyntaxParameters = opcodeSyntaxParameters.replaceAll(" ", "").toLowerCase();
                            if (opcodeSyntaxParameters.endsWith("adr") || opcodeSyntaxParameters.endsWith("d16")) {
                                opcodeBytesFollowing = 2;
                            } else if (opcodeSyntaxParameters.endsWith("d8")) {
                                opcodeBytesFollowing = 1;
                            }
                            if (opcodeBytesFollowing > 0) {
                                opcodeBytesStr = String.valueOf(opcodeBytesFollowing);
                            }
                        } else {
                            opcode = opcodeSyntax.trim();
                        }
                        String opcodeSyntaxValue = opcodeSyntax.replaceAll(" ", "").toLowerCase();
                        opcodeSyntaxValue = opcodeSyntaxValue.replaceAll(",", "");
                        opcodeParameterType = getOpcodeType(opcode.toLowerCase());
                        opcodeInfo = theLine.substring(29, theLine.length()).trim();
                        //
                        // ---------------------------------------------------------
                        switch (opcode.length()) {
                            case 4:
                                paddingO = " ";
                                break;
                            case 3:
                                paddingO = "  ";
                                break;
                            case 2:
                                paddingO = "   ";
                                break;
                            default:
                                break;
                        }
                        String theOpcodeValue = theOpcodes.getOpcodeValue(opcodeOctalDecimal);
                        //  For example, from asmOpcodesBinary.txt, 
                        //      0x04  1  Z,S,P,AC INR B      B <- B+1
                        //  Use 0x04 (00000100),to get "inrb" from asmOpcodes.txt:
                        //      inrb:00000100:|B+1 -> B
                        //  Result,
                        //      theOpcodeValue = "inrb"
                        //
                        if (opcodeParameterType.equals("NA") && !opcode.toLowerCase().equals(theOpcodeValue)) {
                            // 0x07  1  CY       RLC        A = A ...
                            // ++ 07 007 00000111 RLC  rlc     NA  :RLC        A = A ...
                            doCheck2 = "- ";
                        }
                        if (theOpcodeValue.equals("")) {
                            theOpcodeValue = "---";
                        }
                        switch (theOpcodeValue.length()) {
                            case 5:
                                paddingV = " ";
                                break;
                            case 4:
                                paddingV = "  ";
                                break;
                            case 3:
                                paddingV = "   ";
                                break;
                            case 2:
                                paddingV = "    ";
                                break;
                            default:
                                break;
                        }
                        String doCheck = " ";
                        if (!opcodeSyntaxValue.startsWith(theOpcodeValue)) {
                            if (!theOpcodeValue.equals("pusha") && !theOpcodeValue.equals("popa")) {
                                doCheck = "*";
                            }
                        }
                        if (theOpcodeValue.startsWith("mov")) {
                            // ++ 77 119 01110111 MOV  movma * :MOV M,A    (HL) <- A:
                        }
                        switch (opcodeParameterType.length()) {
                            case 0:
                                paddingT = "    ";
                                break;
                            case 1:
                                paddingT = "   ";
                                break;
                            case 2:
                                paddingT = "  ";
                                break;
                            case 3:
                                paddingT = " ";
                                break;
                            default:
                                break;
                        }
                        switch (opcodeSyntaxParameters.length()) {
                            case 6:
                                paddingP = " ";
                                break;
                            case 5:
                                paddingP = "  ";
                                break;
                            case 4:
                                paddingP = "   ";
                                break;
                            case 3:
                                paddingP = "    ";
                                break;
                            case 2:
                                paddingP = "     ";
                                break;
                            case 1:
                                paddingP = "      ";
                                break;
                            case 0:
                                paddingP = "       ";
                                break;
                            default:
                                break;
                        }
                        c1 = theLine.indexOf(" ", 7);
                        if (c1 > 0) {
                            // 0x04  1  Z,S,P,AC INR B      B <- B+1 
                            if (!opcodeParameterType.equals("R")) {
                                doCheck2 = "  ";
                                // doCheck2 = "-p";
                            }
                        } else {
                            // Error because type is NA.
                            doCheck2 = "--";
                        }
                        //  Components:
                        //      opcodeOctal                 03
                        //      opcodeOctalDecimal           3
                        //      opcodeSyntax =              INX B
                        //      opcode =                    INX
                        //      opcodeSyntaxParameters =    B
                        //      opcodeSyntaxValue           inxb
                        //      opcodeParameterType         R
                        //      opcodeInfo =                BC <- BC+1
                        System.out.print("++ "
                                + paddingD + opcodeOctalDecimal + " "
                                + byteToString((byte) opcodeOctalDecimal)
                                + " " + opcode + paddingO
                                + " " + theOpcodeValue + paddingV
                                + opcodeSyntaxParameters + paddingP + opcodeBytesStr
                                + " " + doCheck
                                + " " + opcodeParameterType + paddingT
                                + " " + doCheck2
                                + " :" + opcodeInfo
                        );
                        System.out.println("");
                        // Not storing the results.
                        // opcodeArray[opcodeCount++] = new asmOpcode(value, opcode, info, logic);
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
    public void showFileBytes(String theReadFilename) {
        System.out.println("++ Show binary file bytes: " + theReadFilename);
        int theLength = 0;
        byte bArray[] = null;
        try {
            File theFile = new File(theReadFilename);
            theLength = (int) theFile.length();
            bArray = new byte[(int) theLength];
            FileInputStream in = new FileInputStream(theReadFilename);
            in.read(bArray);
            in.close();
        } catch (IOException ioe) {
            System.out.print("IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("+ Show, " + theLength + " bytes from the file: " + theReadFilename);
        String paddingD;
        for (int lineCounter = 0; lineCounter < theLength; lineCounter++) {
            // int opcodeOctalDecimal = Integer.parseInt(opcodeOctal, 16);
            if (lineCounter < 10) {
                paddingD = "000";
            } else if (lineCounter < 100) {
                paddingD = "00";
            } else if (lineCounter < 1000) {
                paddingD = "0";
            } else {
                paddingD = "";
            }
            System.out.print(" + " + paddingD + lineCounter + " " + byteToString(bArray[lineCounter]) + " ");
            //  + 0309 01010011 
            //  + 0310 01010100 
            System.out.println("");
        }
        System.out.println("\n+ Binary display completed.");
        //
        // String theOpcodeValue = theOpcodes.getOpcodeValue(opcodeOctalDecimal);
        //  For example, from asmOpcodesBinary.txt, 
        //      0x04  1  Z,S,P,AC INR B      B <- B+1
        //  Use 0x04 (00000100),to get "inrb" from asmOpcodes.txt:
        //      inrb:00000100:|B+1 -> B
        //  Result,
        //      theOpcodeValue = "inrb"
        //
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) throws IOException {

        System.out.println("+++ Start.");
        asmOpcodesBinary theOpcodesBin = new asmOpcodesBinary();
        
        /*
        System.out.println("\n-----------------------------------------------");
        System.out.println("+ Find sample values.\n");

        String opcode = "hlt";
        byte opcodeBinary;
        opcodeBinary = theOpcodes.getOpcode(opcode);
        System.out.println("++ " + opcode + " :" + opcodeBinary + ":" + theOpcodes.getOpcodeValue(opcodeBinary));
        // theOpcodes.getOpcodeValue(opcodeBinary);
         */

        System.out.println("\n-----------------------------------------------");
        String fileName = "p1.bin";
        System.out.println("+++ fileName: " + fileName);
        // theOpcodesBin.showFileBytes(fileName);
        System.out.println("\n-----------------------------------------------");
        fileName = "pGalaxyBytesOrg.bin";
        System.out.println("+++ fileName: " + fileName);
        // theOpcodesBin.showFileBytes("pGalaxyBytesOrg.bin");
        System.out.println("\n-----------------------------------------------");
        // 
        System.out.println("");
        // theOpcodes.opcodesListByValue();
        System.out.println("\n+++ Exit.\n");
    }
}
/*
org
 + 0930 00100001 lxih
 + 0931 00100000 16672 4120
 + 0932 01000001 

 + 0777 00100001 
 + 0778 00100000 
 + 0779 01000001 


Org:
                 movam
 + 1691 00100001 lxih
 + 1692 11001001 1225 4C9
 + 1693 00000100 

 + 1691 00100001 lxih
 + 1692 11110000 1008 3F0
 + 1693 00000011

*/