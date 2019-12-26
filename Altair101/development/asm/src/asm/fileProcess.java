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
    private String label;
    private int labelTop = 0;
    private List<String> labelName = new ArrayList<String>();
    private List<Integer> labelAddress = new ArrayList<Integer>();
    //
    // Use for storing program bytes and calculating label addresses.
    private int programTop = 0;
    private List<String> programBytes = new ArrayList<String>();

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
    public void listProgramBytes() {
        System.out.println("+ List Program Bytes:");
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            System.out.println("++ " + theValue);
        }
        System.out.println("+ End of list.");
    }

    public void setProgramByteLabels() {
        System.out.println("+ Set Program Labels:");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("p1:")) {
                String lableAddress = getLabelAddress(theValue.substring(3));
                System.out.println("++ Label: " + theValue + ":" + lableAddress);
                programBytes.set(i, theValue + ":" + lableAddress);
            } else {
                System.out.println("++ " + theValue);
            }
            i++;
        }
        System.out.println("+ End of list.");
    }

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
            System.out.println("++ " + orgLine);
            return;
        }
        //
        ei = theLine.indexOf(";");
        if (ei > 1) {
            // Remove trailing comment.
            //    mvi a,1     ; Move 1 to A.
            //  > mvi a,1
            theLine = theLine.substring(0, ei).trim();
        }
        // ---------------------------------------------------------------------
        // Label line: "Start:"
        if (theLine.endsWith(":")) {
            label = theLine.substring(0, theLine.length() - 1);
            labelName.add(label);
            labelAddress.add(programTop);
            System.out.println("++ Label Name: " + label + " Address: " + programTop);
            return;
        }
        // ---------------------------------------------------------------------
        // Opcode line.
        // Opcodes have 0, 1, or 2 parameters.
        // 0) nop
        // 1) jmp Next
        // 2) mvi a,1

        c1 = theLine.indexOf(" ");
        if (c1 < 1) {
            // Opcode, no parameters, example: "nop".
            opcode = theLine.toLowerCase();
            opcodeBinary = theOpcodes.getOpcode(opcode);
            if (opcodeBinary == 255) {
                System.out.println("-- Error, Opcode not valid: " + opcode);
                return;
            }
            programBytes.add("opcode:" + opcode + ":" + opcodeBinary);
            programTop++;
            System.out.println("++ Opcode: " + opcode + " " + printByte(opcodeBinary));
            return;
        }
        //
        opcode = theLine.substring(0, c1).toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        //
        c1 = theRest.indexOf(",");
        if (c1 < 1) {
            // Opcode, Single parameter, example: "jmp Next".
            p1 = theRest;
            opcodeBinary = theOpcodes.getOpcode(opcode);
            programBytes.add("opcode:" + opcode + ":" + printByte(opcodeBinary));
            programTop++;
            programBytes.add("p1:" + p1);
            programTop++;
            System.out.println("++ Opcode, Single parameter: "
                    + opcode + " " + printByte(opcodeBinary)
                    + " | p1|" + p1 + "|");
            return;
        }
        p1 = theRest.substring(0, c1).trim();
        if (theRest.length() <= c1 + 1) {
            // Error, example: "mvi e,".
            System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "| * Error, missing p2.");
            return;
        }
        p2 = theRest.substring(c1 + 1).trim();
        programBytes.add(p2);
        //
        if (opcode.equals("mvi")) {
            opcodeBinary = theOpcodes.getOpcode(opcode + p1);
            programBytes.add("opcode:" + opcode + p1 + ":" + printByte(opcodeBinary));
            programTop++;
            System.out.println("++ Opcode, Single parameter: "
                    + opcode + " " + printByte(opcodeBinary)
                    + " | p1|" + p1 + "|" + " p2|" + p2 + "|");
        } else {
            System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "|" + " p2|" + p2 + "|");
        }
    }

    public void checkLine() {
    }

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
                checkLine();
                theLine = pin.readLine();
            }
            pin.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
    }

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

    public static void main(String args[]) {
        System.out.println("++ Start.");
        fileProcess thisProcess = new fileProcess();

        thisProcess.parseFile("p2.asm");
        thisProcess.setProgramByteLabels();
        thisProcess.listLabels();
        thisProcess.listProgramBytes();

        System.out.println("++ Exit.");
    }
}
