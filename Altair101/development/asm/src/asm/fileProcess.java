package asm;

/*
    Add parsing for the following (see program p1.asm).
    This is based on the Pong assembler program.

    DB value, that is a label to a string of byte characters.
        TERMB   equ     0ffh        ; Name for a value. Similar to: TERMB = 0ffh;
        Hello   db      'Hello'     ; Start with a label address, followed by a number of bytes with a terminate byte (TERMB).

    Add logic for assembler directive, ds.
                lxi     h,scoreL    ; Increment left misses. "scoreL" is a memory address.
                ...
        scoreL  ds      1           ; Score for left paddle. "1" is the number of bytes.

    Add parsing for a label on a opcode line.
        Halt:   hlt
        ledOut: lxi     h,0         ;HL=16 bit counter

    Add logic for assembler directive, org.
                org 0
                org 80h
 */
import static asm.opcodes8080.byteToString;
import java.io.*;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class fileProcess {

    opcodes8080 theOpcodes = new opcodes8080();
    //
    private String opcode;
    private byte opcodeBinary;
    private String sOpcodeBinary;
    private String p1;
    private String p2;
    private final String SEPARATOR = ":";
    private final String SEPARATOR_TEMP = "^^";
    private String sImmediate = "";
    private final int NAME_NOT_FOUND = 256;
    //
    // Use for storing program bytes and calculating label addresses.
    private int programCounter = 0;
    private int programTop = 0;
    private final List<String> programBytes = new ArrayList<>();

    // -------------------------------------------------------------------------
    //
    private String label;
    private final List<String> labelName = new ArrayList<>();
    private final List<Integer> labelAddress = new ArrayList<>();

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

    // ------------------------
    private int getLabelAddress(String findName) {
        // System.out.println("+ findName: " + findName);
        int returnValue = 0;
        Iterator<String> lName = labelName.iterator();
        Iterator<Integer> lAddress = labelAddress.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theAddress = lAddress.next();
            if (theName.equals(findName)) {
                returnValue = theAddress;
                // System.out.println("+ Found theAddress: " + returnValue);
                break;
            }
        }
        return returnValue;
    }

    private void setProgramByteLabels() {
        // System.out.println("+ Set Program Labels:");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("lb:")) {
                //
                int intAddress = getLabelAddress(theValue.substring(3));
                // ++ Label: lb:okaym1:265
                // B11001010, 9, 1,   // 259: jz okaym1  265 in binary hb=00000001 lb=00001001
                //
                if (intAddress < 256) {
                    // lb:
                    String labelAddress = Integer.toString(intAddress);
                    programBytes.set(i, theValue + SEPARATOR + labelAddress);
                    System.out.println("++ Label, lb: " + theValue + ":" + labelAddress);
                    // hb:
                    // Already set to default of 0.
                    theValue = it.next();
                    i++;
                    System.out.println("++ Label, " + theValue);
                } else {
                    // Need to also set hb (high byte).
                    // Address: 265, in binary hb=00000001(digital=1) lb=00001001(digital=9)
                    int hb = intAddress / 256;
                    int lb = intAddress - (hb * 256);
                    //-------------------
                    // lb:
                    programBytes.set(i, theValue + SEPARATOR + lb);
                    System.out.println("++ Label, lb: " + theValue + ":" + lb);
                    // hb:
                    it.next();
                    i++;
                    programBytes.set(i, "hb:" + hb);
                    System.out.println("++ Label, hb:" + hb);
                }
            }
            i++;
        }
    }

    // -------------------------------------------------------------------------
    // Assembler directive variable names.
    private final List<String> variableName = new ArrayList<>();
    private final List<Integer> variableValue = new ArrayList<>();

    public void listNames() {
        System.out.println("+ List names:");
        Iterator<String> lName = variableName.iterator();
        Iterator<Integer> lValue = variableValue.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theValue = lValue.next();
            System.out.println("++ " + theName + ": " + theValue);
        }
        System.out.println("+ End of list.");
    }

    // ------------------------
    private String getNameValue(String findName) {
        // System.out.println("+ getNameValue, findName: " + findName);
        String returnString = findName;
        int returnValue = NAME_NOT_FOUND;
        Iterator<String> lName = variableName.iterator();
        Iterator<Integer> lValue = variableValue.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theValue = lValue.next();
            if (theName.equals(findName)) {
                returnValue = theValue;
                // System.out.println("+ Found theValue: " + returnValue);
                break;
            }
        }
        if (returnValue != NAME_NOT_FOUND) {
            returnString = Integer.toString(returnValue);
        }
        return returnString;
    }

    private void setProgramByteNames() {
        System.out.println("+ Set Program Names:");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("name:")) {
                int intAddress = getLabelAddress(theValue.substring(3));
                // ++ Name:265
                // B11001010, 9, 1,   // 259: jz okaym1  265 in binary hb=00000001 lb=00001001
                //
                String labelAddress = Integer.toString(intAddress);
                programBytes.set(i, theValue + SEPARATOR + labelAddress);
                System.out.println("++ Label, lb: " + theValue + ":" + labelAddress);
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

    private void printProgramBytesArray() {
        System.out.println("\n+ Print a program array from the program data:");
        programCounter = 0;
        programTop = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String opcodeStatement = "";
            String opcodeComment = "";
            String[] byteValues;
            //
            String theValue = it.next();
            programTop++;
            // System.out.println("++ theValue |" + theValue + "|");
            String[] opcodeValues = theValue.split(SEPARATOR);
            opcode = opcodeValues[1];
            if (opcodeValues[0].equals("opcode")) {
                opcodeStatement = "B" + opcodeValues[2] + ",";    // B11000011,
            }
            switch (opcode) {
                // -----------------------------
                case "hlt":
                case "nop":
                case "ret":
                case "rrc":
                    // opcode (no parameters)
                    // ++ opcode:nop:00000000:
                    opcodeComment = opcode;
                    break;
                // -----------------------------
                case "cmp":
                case "dad":
                case "dcr":
                case "inr":
                case "inx":
                case "ldax":
                case "ora":
                case "xra":
                    // opcode <register|RegisterPair>
                    // ++ opcode:inr:00111101:a:
                    opcodeComment = opcode + " " + opcodeValues[3];
                    break;
                // -----------------------------
                case "adi":
                case "ani":
                case "cpi":
                case "in":
                case "out":
                case "sui":
                    // opcode <immediate>
                    // out 39
                    // ++ opcode:out:11100011:39
                    // ++ immediate:39
                    theValue = it.next();   // ++ immediate:39
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    String nameValue = getNameValue(byteValues[1].toLowerCase());
                    opcodeStatement += " " + nameValue + ",";
                    if (nameValue.equals(byteValues[1])) {
                        opcodeComment = opcode + " " + opcodeValues[3];
                    } else {
                        opcodeComment = opcode + " " + nameValue + " (" + byteValues[1] + ")";
                    }
                    break;
                // -----------------------------
                case "call":
                case "jmp":
                case "jnz":
                case "jz":
                case "jnc":
                case "jc":
                case "lda":
                case "shld":
                case "sta":
                    // opcode <address label>
                    // ++ opcode:jmp:11000011:There:
                    // ++ lb:Loop:2
                    // ++ hb:0
                    //------------
                    theValue = it.next();   // ++ lb:Loop:2
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[2] + ",";
                    //------------
                    theValue = it.next();   // ++ hb:x
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[1] + ",";
                    //
                    opcodeComment = opcode + " " + opcodeValues[3];
                    break;
                // -----------------------------
                case "lxi":
                    // opcode <register>,<address>
                    // lxi b,5
                    // ++ opcode:lxi:00000001:b:5
                    // ++ lb:5:
                    // ++ hb:0
                    theValue = it.next();   // ++ lb:5:
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[1] + ",";
                    it.next();              // ++ hb:0
                    programTop++;
                    opcodeStatement += " 0,";
                    opcodeComment = opcode + " " + opcodeValues[3] + "," + opcodeValues[4];
                    break;
                // -----------------------------
                case "mvi":
                    // opcode <register>,<immediate>
                    // mvi a,1
                    // ++ opcode:mvi:00111110:a:1
                    // ++ immediate:1
                    theValue = it.next();   // ++ immediate:1
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    //
                    sImmediate = byteValues[1];
                    if (byteValues[1].equals("'" + SEPARATOR_TEMP + "'")) {
                        sImmediate = "'" + SEPARATOR + "'";
                    }
                    //
                    opcodeStatement += " " + sImmediate + ",";
                    opcodeComment = opcode + " " + opcodeValues[3] + "," + sImmediate;
                    break;
                // -----------------------------
                case "mov":
                    // opcode <register>,<register>
                    // mov a,b
                    // ++ opcode:mov:00111110:a:b
                    opcodeComment = opcode + " " + opcodeValues[3] + "," + opcodeValues[4];
                    break;
                // -----------------------------
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
        String programCounterPadding = "";
        if (programCounter < 10) {
            programCounterPadding = "  ";
        } else if (programCounter < 100) {
            programCounterPadding = " ";
        }
        System.out.println(marginPadding + opcodeStatement + "// " + programCounterPadding + programCounter + ": " + opcodeComment);
        programCounter = programTop;
    }

    // -------------------------------------------------------------------------
    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ Label Name: " + label + ", Address: " + programTop);
    }

    private void parseName(String theName, String theValue) {
        variableName.add(theName);
        if (theValue.endsWith("h")) {
            // Change 0ffh to integer.
            int si = 0;
            if (theValue.startsWith("0")) {
                si = 1;
            }
            theValue = theValue.substring(si, 3);   // Hex string to integer.
        }
        variableValue.add(Integer.parseInt(theValue, 16));
        System.out.println("++ Variable name: " + theName + ", value: " + theValue);
    }

    private String getOpcodeBinary(String opcode) {
        opcodeBinary = theOpcodes.getOpcode(opcode);
        if (opcodeBinary == theOpcodes.OpcodeNotFound) {
            opcode = "INVALID: " + opcode;
            System.out.print("-- Error, ");
            return ("INVALID: " + opcode);
        }
        return (byteToString(opcodeBinary));
    }

    private void parseOpcode(String opcode) {
        // opcode (no parameters)
        // hlt
        // nop
        sOpcodeBinary = getOpcodeBinary(opcode);
        programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary);
        programTop++;
        System.out.println("++ Opcode: " + opcode + " " + sOpcodeBinary);
    }

    private void parseOpcode(String opcode, String p1) {
        // Opcode, single parameter, example: jmp Next
        switch (opcode) {
            // -----------------------------
            case "cmp":
            case "dad":
            case "dcr":
            case "inr":
            case "inx":
            case "ldax":
            case "ora":
            case "xra":
                // opcode <register|RegisterPair>
                // cmp c
                p1 = p1.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                break;
            // -----------------------------
            case "adi":
            case "ani":
            case "cpi":
            case "in":
            case "out":
            case "sui":
                // opcode <immediate>
                // out 39
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                programBytes.add("immediate:" + p1);
                programTop++;
                break;
            // -----------------------------
            case "call":
            case "jmp":
            case "jnz":
            case "jz":
            case "jnc":
            case "jc":
            case "lda":
            case "shld":
            case "sta":
                // opcode <address label>
                // jmp There
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                programBytes.add("lb:" + p1);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                break;
            // -----------------------------
            default:
                opcode = "INVALID: " + opcode;
                System.out.print("-- Error, ");
                break;
        }
        System.out.println("++ Opcode: "
                + opcode + " " + sOpcodeBinary
                + " p1|" + p1 + "|");
    }

    private void parseOpcode(String opcode, String p1, String p2) {
        // Opcode, 2 parameters, example: mvi a,1
        switch (opcode) {
            case "lxi":
                // opcode <register>,<address label>
                // lxi b,5
                p1 = p1.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1 + SEPARATOR + p2);
                programTop++;
                programBytes.add("lb:" + p2);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                break;
            case "mvi":
                // opcode <register>,<immediate>
                // mvi a,1
                p1 = p1.toLowerCase();
                //
                // Case of the immediate equaling the separator.
                sOpcodeBinary = getOpcodeBinary(opcode + p1);
                sImmediate = p2;
                if (p2.equals("'" + SEPARATOR + "'")) {
                    p2 = "'" + SEPARATOR_TEMP + "'";
                }
                //
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1 + SEPARATOR + p2);
                programTop++;
                programBytes.add("immediate:" + p2);
                programTop++;
                break;
            case "mov":
                // opcode <register>,<register>
                // mov a,b
                p1 = p1.toLowerCase();
                p2 = p2.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1 + p2);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1 + SEPARATOR + p2);
                programTop++;
                break;
            default:
                opcode = "INVALID: " + opcode;
                System.out.print("-- Error, ");
                break;
        }
        System.out.println("++ Opcode: "
                + opcode + " " + sOpcodeBinary
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
            // Comment lines are not parsed farther.
            // System.out.println("++ " + orgLine.trim());
            return;
        }
        ei = theLine.indexOf(";");
        if (ei > 1) {
            // Remove trailing comment.
            //    mvi a,1     ; Move 1 to A.
            //  > mvi a,1
            // System.out.println("++ " + theLine.substring(ei).trim());
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
        // Opcode lines. Opcodes can have 0, 1, or 2 parameters. For example:
        //      1) nop
        //      2) mvi a,1
        //      3) jmp Next
        // Or assembler directives:
        //      4) org     0
        //      5) TERMB   equ     0ffh
        //      6) Hello   db      "Hello"

        // ---------------------------------------------------------------------
        c1 = theLine.indexOf(" ");
        if (c1 < 1) {
            //  1) Opcode, no parameters, example: "nop".
            opcode = theLine.toLowerCase();
            System.out.println("++ parseLine, Opcode|" + opcode + "|");
            parseOpcode(opcode);
            return;
        }
        // ------------------------------
        // Get the other line components.
        String part1 = theLine.substring(0, c1).toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        System.out.println("++ parseLine, part1|" + part1 + "| theRest|" + theRest + "|");
        //
        // ---------------------------------------------------------------------
        // Check for opcode with 2 parameters, example: mvi a,1
        c1 = theRest.indexOf(",");
        // ------------------------------------------
        if (c1 > 0) {
            // ------------------------------------------
            //  2) Opcode, 2 parameters, example: "mvi a,3".
            p1 = theRest.substring(0, c1).trim();
            p2 = theRest.substring(c1 + 1).trim();
            System.out.println("++ parseLine, Opcode|" + part1 + "| p1|" + p1 + "| p2|" + p2 + "|");
            parseOpcode(part1, p1, p2);
            return;
        }
        // ---------------------------------------------------------------------
        // Check for single parameter lines:
        //  Opcode lines. Opcodes can have 0, 1, or 2 parameters. For example:
        //      3) jmp Next
        //  Or assembler directives:
        //      4) org     0
        c1 = theRest.indexOf(" ");
        if (c1 < 1) {
            if (theLine.startsWith("org")) {
                // Assembler directives:
                //  4) org     0
                System.out.println("++ parseLine, Directive: org. For now, parse org line, as a NOP line: " + orgLine.trim());
                parseOpcode("nop");
                return;
            }
            // Opcode, Single parameter, example: "jmp Next".
            System.out.println("++ parseLine, Opcode|" + part1 + "| theRest|" + theRest + "|");
            parseOpcode(part1, theRest);
            return;
        }
        // ---------------------------------------------------------------------
        // Parse assembler directives:
        //      5) TERMB   equ     0ffh
        //      6) Hello   db      "Hello"
        //
        String part2 = theRest.substring(0, c1).toLowerCase();
        String part3 = theRest.substring(c1 + 1).trim();
        System.out.println("++ parseLine, Directive|" + part1 + "| part2|" + part2 + "| part3|" + part3 + "|");
        // ++ parseLine, Directive|termb| part2|equ| part3|0ffh|
        // ++ parseLine, Directive|hello| part2|db| part3|'Hello'|
        // ------------------------------------------
        if (part2.equals("equ")) {
            System.out.println("++ parseLine, equ directive: part3|" + part3 + "|");
            parseName(part1, part3);
            return;
        }
        if (part2.equals("db")) {
            System.out.println("++ parseLine, db directive: part3|" + part3 + "|");
            return;
        }
        System.out.print("-- Error parsing line: " + theLine);
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
    public void printProgramByteArray(fileProcess thisProcess, String theReadFilename) {
        thisProcess.parseFile(theReadFilename);
        thisProcess.setProgramByteLabels();
        thisProcess.printProgramBytesArray();
    }

    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("++ Start.");
        fileProcess thisProcess = new fileProcess();

        System.out.println("\n+ Parse file lines.");
        //thisProcess.printProgramByteArray(thisProcess,
        //    "/Users/dthurston/Projects/arduino/Altair101/development/asm/programs/opCpi.asm");

        // Or process in parts with optional, extra debug listings.
        // Required, starts the process:
        thisProcess.parseFile("p1.asm");
        thisProcess.listNames();
        //
        // Required, sets actual address byte values for the labels:
        thisProcess.setProgramByteLabels();
        //
        // Optional, used for debugging:
        // thisProcess.listLabels();
        // thisProcess.listProgramBytes();
        //
        // Required, prints the output for use in Processor.ino:
        thisProcess.printProgramBytesArray();
        System.out.println("++ Exit.");
    }
}
