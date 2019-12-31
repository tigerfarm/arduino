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
        System.out.println("\n+ Print a program array from the program data:");
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String opcodeStatement = "";
            String opcodeComment = "";
            String[] byteValues;
            //
            String theValue = it.next();
            // System.out.println("++ theValue |" + theValue + "|");
            String[] opcodeValues = theValue.split(":");
            opcode = opcodeValues[1];
            if (opcodeValues[0].equals("opcode")) {
                opcodeStatement = "B" + opcodeValues[2] + ",";    // B11000011,
            }
            switch (opcode) {
                case "hlt":
                case "nop":
                    // opcode (no parameters)
                    // ++ opcode:nop:00000000:
                    opcodeComment = opcode;
                    break;
                case "cmp":
                case "inr":
                    // opcode <register>
                    // ++ opcode:inr:00111101:a:
                    opcodeComment = opcode + " " + opcodeValues[3];
                    break;
                case "out":
                    // opcode <immediate>
                    // out 39
                    // ++ opcode:out:11100011:39
                    // ++ immediate:39
                    theValue = it.next();   // ++ immediate:39
                    byteValues = theValue.split(":");
                    opcodeStatement += " " + byteValues[1] + ",";
                    opcodeComment = opcode + " " + opcodeValues[3];
                    break;
                case "jmp":
                case "jnz":
                case "jz":
                case "jnc":
                case "jc":
                    // opcode <address label>
                    // ++ opcode:jmp:11000011:There:
                    // ++ lb:Loop:2
                    // ++ hb:0
                    theValue = it.next();   // ++ lb:Loop:2
                    byteValues = theValue.split(":");
                    opcodeStatement += " " + byteValues[2] + ",";
                    it.next();              // ++ hb:0
                    opcodeStatement += " 0,";
                    opcodeComment = opcode + " " + opcodeValues[3];
                    break;
                case "mvi":
                    // opcode <register>,<immediate>
                    // mvi a,1
                    // ++ opcode:mvi:00111110:a:1
                    // ++ immediate:1
                    theValue = it.next();   // ++ immediate:1
                    byteValues = theValue.split(":");
                    opcodeStatement += " " + byteValues[1] + ",";
                    opcodeComment = opcode + " " + opcodeValues[3] + "," + opcodeValues[4];
                    break;
                default:
                    break;
            }
            printProgramBytesArrayLine(opcodeStatement, opcodeComment);
        }
        printProgramBytesArrayLine("0", "End of program");
        System.out.println("\n+ End of array.");
    }

    private void printProgramBytesArrayLine(String opcodeStatement, String opcodeComment) {
        String marginPadding = "  ";
        opcodeStatement += marginPadding;
        for (int i = opcodeStatement.length(); i < 21; i++) {
            // 12345678901234567890123456789
            //   B00111110, 73,    // mvi a,73
            opcodeStatement += " ";
        }
        System.out.println(marginPadding + opcodeStatement + "// " + opcodeComment);
    }

    // -------------------------------------------------------------------------
    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ Label Name: " + label + ", Address: " + programTop);
    }

    private void parseOpcode(String opcode) {
        // opcode (no parameters)
        // hlt
        // nop
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
        switch (opcode) {
            case "cmp":
            case "inr":
                // opcode <register>
                // cmp c
                // inr a
                opcodeBinary = theOpcodes.getOpcode(opcode + p1);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary) + ":" + p1);
                programTop++;
                break;
            case "out":
                // opcode <immediate>
                // out 39
                opcodeBinary = theOpcodes.getOpcode(opcode);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary) + ":" + p1);
                programTop++;
                programBytes.add("immediate:" + p1);
                programTop++;
                break;
            case "jmp":
            case "jnz":
            case "jz":
            case "jnc":
            case "jc":
                // opcode <address label>
                // jmp There
                opcodeBinary = theOpcodes.getOpcode(opcode);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary) + ":" + p1);
                programTop++;
                programBytes.add("lb:" + p1);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                break;
            default:
                opcode = "INVALID-" + opcode;
                System.out.print("-- Error, ");
                break;
        }
        System.out.println("++ Opcode: "
                + opcode + " " + printByte(opcodeBinary)
                + " p1|" + p1 + "|");
    }

    private void parseOpcode(String opcode, String p1, String p2) {
        // Opcode, 2 parameters, example: mvi a,1
        switch (opcode) {
            case "mvi":
                // opcode <register>,<immediate>
                // mvi a,1
                opcodeBinary = theOpcodes.getOpcode(opcode + p1);
                programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary) + ":" + p1 + ":" + p2);
                programTop++;
                programBytes.add("immediate:" + p2);
                programTop++;
                break;
            default:
                opcode = "INVALID-" + opcode;
                System.out.print("-- Error, ");
                break;
        }
        System.out.println("++ Opcode: "
                + opcode + " " + printByte(opcodeBinary)
                + " p1|" + p1 + "|" + " p2|" + p2 + "|");
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
        // thisProcess.parseFile("p1.asm");
        thisProcess.parseFile("opcodeCmp.asm");
        //
        thisProcess.setProgramByteLabels();
        // thisProcess.listLabels();
        //
        // thisProcess.listProgramBytes();
        thisProcess.printProgramBytesArray();

        System.out.println("++ Exit.");
    }
}
