package asm;

import static asm.opcodes8080.printByte;
import java.io.*;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class fileProcess {

    opcodes8080 theOpcodes = new opcodes8080();
    //
    private String opcode;
    private byte opcodeBinary;
    private String p1;
    private String p2;
    //
    // Use for storing program bytes and calculating label addresses.
    private int programTop = 0;
    private List<String> programBytes = new ArrayList<String>();

    // -------------------------------------------------------------------------
    //
    private String label;
    private int labelTop = 0;
    private List<String> labelName = new ArrayList<String>();
    private List<Integer> labelAddress = new ArrayList<Integer>();

    public void listLabels() {
        System.out.println("+ List labels:");
        Iterator<String> lName = labelName.iterator();
        Iterator<Integer> lAddress = labelAddress.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theAddress = lAddress.next();
            System.out.println("++ " + theName + ": " + theAddress);
        }
        System.out.println("+ End of list.");
    }

    // -------------------------------------------------------------------------
    public String getLabelAddress(String findName) {
        // System.out.println("+ findName: " + findName);
        String returnValue = "";
        Iterator<String> lName = labelName.iterator();
        Iterator<Integer> lAddress = labelAddress.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theAddress = lAddress.next();
            if (theName.equals(findName)) {
                returnValue = Integer.toString(theAddress);
                // System.out.println("+ Found.");
                break;
            }
        }
        return returnValue;
    }

    public void setProgramByteLabels() {
        // System.out.println("+ Set Program Labels:");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("lb:")) {
                String lableAddress = getLabelAddress(theValue.substring(3));
                // System.out.println("++ Label: " + theValue + ":" + lableAddress);
                programBytes.set(i, theValue + ":" + lableAddress);
            }
            i++;
        }
    }

    // -------------------------------------------------------------------------
    //
    public void listProgramBytes() {
        System.out.println("\n+ List Program Bytes:");
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            System.out.println("++ " + theValue);
        }
        System.out.println("+ End of list.");
    }

    public void printProgramBytesArray() {
        String sOpcode = "opcode:";
        int sOpcodeLen = sOpcode.length();
        String sP1 = "p1:";
        int sP1Len = sP1.length();
        int c2;
        System.out.println("\n+ Print a program array from the program data:");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            //   B11000011, 9, 0,  // jmp Test
            if (theValue.startsWith("opcode:")) {
                if (i > 0) {
                    System.out.println("");
                }
                c2 = theValue.indexOf(":", sOpcodeLen + 1);
                opcode = theValue.substring(sOpcodeLen, c2);
                System.out.print("   "
                        + "B" + theValue.substring(c2 + 1)
                        + ",   // " + theValue.substring(sOpcodeLen, c2)
                ); // print: B11000011,
            } else if (theValue.startsWith("p1:")) {
                c2 = theValue.indexOf(":", sP1Len + 1);
                System.out.print("   "
                        + theValue.substring(c2 + 1)
                        + ",   // " + theValue.substring(sP1Len, c2)
                ); // print: 9, 0,
            }
            i++;
        }
        System.out.println("\n+ End of array.");
    }

    // -------------------------------------------------------------------------
    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ Label Name: " + label + ", Address: " + programTop);
    }

    private void parseOpcode(String opcode) {
        // Opcode, no parameters, example: "nop".
        opcodeBinary = theOpcodes.getOpcode(opcode);
        if (opcodeBinary == theOpcodes.OpcodeNotFound) {
            opcode = "INVALID-" + opcode;
            System.out.print("-- Error, ");
        }
        programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
        programTop++;
        System.out.println("++ Opcode: " + opcode + " " + printByte(opcodeBinary));
    }

    private void parseOpcode(String opcode, String p1) {
        // Opcode, single parameter, example: jmp Next
        System.out.println("++ Opcode: "
                + opcode + " " + printByte(opcodeBinary)
                + " p1|" + p1 + "|");
        switch (opcode) {
            case "inr":
                opcodeBinary = theOpcodes.getOpcode(opcode + p1);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
                programTop++;
                break;
            case "jmp":
                opcodeBinary = theOpcodes.getOpcode(opcode);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
                programTop++;
                programBytes.add("lb:" + p1);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                break;
            default:
                opcodeBinary = theOpcodes.getOpcode(opcode);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
                programTop++;
                programBytes.add("p1:" + p1);
                programTop++;
                break;
        }
    }

    private void parseOpcode(String opcode, String p1, String p2) {
        // Opcode, 2 parameters, example: mvi a,1
        switch (opcode) {
            case "mvi":
            case "cmp":
                opcodeBinary = theOpcodes.getOpcode(opcode + p1);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
                programTop++;
                programBytes.add("p1:" + p1 + ":p2:" + p2 );
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + printByte(opcodeBinary)
                        + " p1|" + p1 + "|" + " p2|" + p2 + "|");
                break;
            default:
                programBytes.add(p2);
                System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "|" + " p2|" + p2 + "|");
                break;
        }
    }
    // -------------------------------------------------------------------------

    private void parseLine(String orgLine) {
        String theRest;
        int ei;
        int c1;
        //
        label = "";
        opcode = "";
        opcodeBinary = 0;
        p1 = "";
        p2 = "";
        //
        String theLine = orgLine.trim();
        if (theLine.length() == 0) {
            System.out.println("++");
            return;
        }
        if (theLine.startsWith(";")) {
            System.out.println("++ " + orgLine.trim());
            return;
        }
        //
        ei = theLine.indexOf(";");
        if (ei > 1) {
            System.out.println("++ " + theLine.substring(ei).trim());
            // Remove trailing comment.
            //    mvi a,1     ; Move 1 to A.
            //  > mvi a,1
            theLine = theLine.substring(0, ei).trim();
        }
        // ---------------------------------------------------------------------
        // Label line: "Start:"
        if (theLine.endsWith(":")) {
            label = theLine.substring(0, theLine.length() - 1);
            parseLabel(label);
            return;
        }
        // ---------------------------------------------------------------------
        // Opcode line.
        // Opcodes have 0, 1, or 2 parameters. For example:
        //      0) nop
        //      1) jmp Next
        //      2) mvi a,1

        c1 = theLine.indexOf(" ");
        // ------------------------------------------
        if (c1 < 1) {
            // Opcode, no parameters, example: "nop".
            opcode = theLine.toLowerCase();
            parseOpcode(opcode);
            return;
        }
        //
        opcode = theLine.substring(0, c1).toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        //
        c1 = theRest.indexOf(",");
        // ------------------------------------------
        if (c1 < 1) {
            // Opcode, Single parameter, example: "jmp Next".
            p1 = theRest;
            parseOpcode(opcode, p1);
            return;
        }
        p1 = theRest.substring(0, c1).trim();
        //
        // ------------------------------------------
        if (theRest.length() <= c1 + 1) {
            // Error, example: "mvi e,".
            System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "| * Error, missing p2.");
            return;
        }
        // ------------------------------------------
        // Opcode, 2 parameters, example: "mvi a,3".
        p2 = theRest.substring(c1 + 1).trim();
        parseOpcode(opcode, p1, p2);
    }

    // -------------------------------------------------------------------------
    public void parseFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            while (theLine != null) {
                parseLine(theLine);
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("");
    }

    // -------------------------------------------------------------------------
    public void listFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            while (theLine != null) {
                System.out.println("+ " + theLine);
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
    }

    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("++ Start.");
        fileProcess thisProcess = new fileProcess();

        System.out.println("\n+ Parse file lines.");
        thisProcess.parseFile("p1.asm");
        //
        thisProcess.setProgramByteLabels();
        thisProcess.listLabels();
        //
        thisProcess.listProgramBytes();
        // thisProcess.printProgramBytesArray();

        System.out.println("++ Exit.");
    }
}
