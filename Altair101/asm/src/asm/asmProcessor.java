package asm;

/*
    ----------------------------------------------------------------------------
    Altair 101 Assembler Program

    The basic assembler works:
    + It can assemble programs from a set of tested Intl 8080/8085 opcodes and assembler directives.
    + It assemble the Kill the Bit program, which was run on the Altair 101 dev machine.

    +++ Ready to be updated so that it can assemble the Pong program.
    + Add directive, end, which tells the assemble to end processing the file its assembiling.
    + Add opcode, rlc: RLC       00000111          C       Rotate A left
    ++ Similar to:     rrc      00 001 111  1  Rotate accumulator right by shift right 1 bit, and wrapping the last bit to the first position. Need to handle carry bit.

    ---------------------------------------------
    Next assembler updates,

    + Add directive, "end".
    + Handle ":" (SEPARATOR) in a DB string.
++ Example:
++ msgSuccess  db      '+ Success: '
++ ++ ':',                 //   7: dbname: msgsuccess

    ++ When reading in the string, exchange SEPARATOR with SEPARATOR_TEMP.
    ++ When writing out each character (dbname), exchange SEPARATOR_TEMP with SEPARATOR.
    + Print the comment properly, when it contains a SEPARATOR in an EQU, example:
    ++ B00111110, 58,       //  50: mvi 00111110 ('^^')

    + Create more opcode test programs and samples,
    ++ looping, branching, sense switch interation.
    ++ Create subroutines such as print and println.

    Next opcodes to add/test,
    + Opcodes implemented in the assembler, but not tested with a program:
    lda a    00 110 010  3  Load register A with data from the address, a(hb:lb).
    sta a    00 110 010  3  Store register A to the address, a(hb:lb).
    inr D    00 DDD 100  1  Increment a register. To do, set flags: ZSPA.
    dcr D    00 DDD 101  1  Decrement a register. To do, set flags: ZSPA.
    inx RP   00 RP0 011  1  Increment a register pair(16 bit value): B:C, D:E, H:L. To do: increment the stack pointer.
    shld a   00 100 010  3  Store data value from memory location: a(address hb:lb), to register L. Store value at: a + 1, to register H.
    // shld a    00100010 lb hb    -  Store register L contents to memory address hb:lb. Store register H contents to hb:lb+1.

    ----------------------------------------------------------------------------
    Program sections:
    + Program byte output: listing and printing program bytes.
    + Program byte output: printing program bytes into an array for use in Processor.ino.
    + Address label name and value management.
    + Immediate variable name and value management.
    + Parse opcodes into program bytes.
    + Parse program lines.
    + File level process: read files, and parse or list the file lines.

    ----------------------------------------------------------------------------
    Standardize use of hex numbers.
    Improve error handling.

    Be consistent with label and name case sensitivity.
    + Currently, not case sensitive.
    + Match, getLabelAddress() with how the address names being added.
    + Match, getImmediateValue() with how the immediates being added.

    https://en.wikipedia.org/wiki/ASCII

    ----------------------------------------------------------------------------
 */
import static asm.opcodes8080.byteToString;
import java.io.*;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class asmProcessor {

    opcodes8080 theOpcodes = new opcodes8080();
    //
    private String opcode;
    private byte opcodeBinary;
    private String sOpcodeBinary;
    private String p1;
    private String p2;
    private final String SEPARATOR = ":";
    private final String SEPARATOR_TEMP = "^^";
    private final int NAME_NOT_FOUND = 256;
    private final int DB_STRING_TERMINATOR = 255;   // ffh = B11111111
    private int errorCount = 0;
    private static int ignoreFirstCharacters = 0;
    //
    // Use for storing program bytes and calculating label addresses.
    private int programCounter = 0;
    private int programTop = 0;
    private final List<String> programBytes = new ArrayList<>();

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Program byte output: Listing and printing bytes.
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
            String[] opcodeValues = theValue.split(SEPARATOR);
            opcode = opcodeValues[1];
            if (opcodeValues[0].equals("opcode")) {
                opcodeStatement = "B" + opcodeValues[2] + ",";    // B11000011,
            } else {
                opcode = opcodeValues[0];
            }
            // System.out.println("++ printProgramBytesArray, opcode|" + opcode + "| theValue |" + theValue + "|");
            switch (opcode) {
                // -------------------------------------------------------------
                // Assembler directives.
                case "dbname":
                    // ++ printProgramBytesArray, opcode|dbname| theValue |dbname:hello:H|
                    opcodeStatement = "'" + opcodeValues[2] + "'" + ",";
                    opcodeComment = opcode + ": " + opcodeValues[1];
                    break;

                case "dbstringterminator":
                    // ++ printProgramBytesArray, opcode|dbstringterminator| theValue|dbstringterminator:hello:255|
                    opcodeStatement = opcodeValues[2] + ",";
                    opcodeComment = opcode + ": " + opcodeValues[1];
                    break;
                case "dsname":
                    // ++ printProgramBytesArray, opcode|dsname| theValue|dsname:scorer:1|
                    opcodeStatement = opcodeValues[2] + ",";
                    opcodeComment = opcode + ": " + opcodeValues[1];
                    break;
                // -------------------------------------------------------------
                // Opcodes
                case "hlt":
                case "nop":
                case "ret":
                case "rlc":
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
                    // ++ immediate:39:39
                    // ++ immediate:TERMB:255
                    theValue = it.next();   // ++ immediate:39:39
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[2] + ",";
                    if (byteValues[1].equals(byteValues[2])) {
                        opcodeComment = opcode + " " + byteValues[1];
                    } else {
                        opcodeComment = opcode + " " + byteValues[2] + " (" + byteValues[1] + ")";
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
                    //------------------------------------------------
                    theValue = it.next();   // ++ lb:Loop:2
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[2] + ",";
                    //------------
                    theValue = it.next();   // ++ hb:x
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[1] + ",";
                    //------------------------------------------------
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
                    //------------------------------------------------
                    theValue = it.next();   // ++ lb:Loop:2
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[2] + ",";
                    //------------
                    theValue = it.next();   // ++ hb:x
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[1] + ",";
                    //------------------------------------------------
                    opcodeComment = opcode + " " + opcodeValues[3] + "," + opcodeValues[4];
                    break;
                // -----------------------------
                case "mvi":
                    // opcode <register>,<immediate>
                    // mvi a,1
                    // ++ opcode:mvi:00111110:a:1
                    // ++ immediate:1:1
                    // ++ immediate:TERMB:255
                    theValue = it.next();   // ++ immediate:1:1
                    programTop++;
                    byteValues = theValue.split(SEPARATOR);
                    opcodeStatement += " " + byteValues[2] + ",";
                    if (byteValues[1].equals(byteValues[2])) {
                        opcodeComment = opcode + " " + opcodeValues[1];
                    } else {
                        opcodeComment = opcode + " " + opcodeValues[2] + " (" + byteValues[1] + ")";
                    }
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
    // -------------------------------------------------------------------------
    // Address label name and value management.
    //  Parsing, listing, and setting address program byte values.
    private String label;
    private final List<String> labelName = new ArrayList<>();
    private final List<Integer> labelAddress = new ArrayList<>();

    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ Label Name: " + label + ", Address: " + programTop);
    }

    private void parseOrg(String theValue) {
        String theName = "org";
        System.out.println("++ Org address value: " + theValue);
        int intValue;
        if (theValue.endsWith("h")) {
            // Hex number. For example, change 0ffh or ffh to integer.
            // Samples: 0ffh, 0eh
            int si = 0;
            if (theValue.startsWith("0") && theValue.length() > 3) {
                si = 1;
            }
            theValue = theValue.substring(si, theValue.length() - 1);   // Hex string to integer. Remove the "h".
            intValue = Integer.parseInt(theValue, 16);
        } else {
            intValue = Integer.parseInt(theValue);
        }
        for (int i = programTop; programTop < intValue; programTop++) {
            programBytes.add("dsname:" + theName + SEPARATOR + 0);  // default value.
        }
    }

    private void parseDs(String theName, String theValue) {
        System.out.println("++ DS variable name: " + theName + ", number of bytes: " + theValue);
        labelName.add(theName);
        labelAddress.add(programTop + 1);        // Address to the bytes.
        for (int i = 0; i < Integer.parseInt(theValue); i++) {
            programBytes.add("dsname:" + theName + SEPARATOR + 0);  // default value.
            programTop++;
        }
    }

    public void listLabelAddresses() {
        System.out.println("\n+ List label Addresses:");
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
        System.out.println("+ findName: " + findName);
        int returnValue = 0;
        Iterator<String> lName = labelName.iterator();
        Iterator<Integer> lAddress = labelAddress.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theAddress = lAddress.next();
            if (theName.toLowerCase().equals(findName.toLowerCase())) {
                returnValue = theAddress;
                System.out.println("+ Found theAddress: " + returnValue);
                break;
            }
        }
        if (findName.endsWith("h")) {
            // Hex number. For example, change 0ffh or ffh to integer.
            // Samples: 0ffh, 0eh
            int si = 0;
            if (findName.startsWith("0") && findName.length() > 3) {
                si = 1;
            }
            findName = findName.substring(si, findName.length() - 1);   // Hex string to integer. Remove the "h".
            returnValue = Integer.parseInt(findName, 16);
        } else if (!findName.equals("0") && returnValue == 0) {
            try {
                returnValue = Integer.parseInt(findName);
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("\n- Error, address label not found: " + findName + ".\n");
                returnValue = 0;
            }
        }
        return returnValue;
    }

    private void setProgramByteAddresses() {
        System.out.println("\n+ Set Program Label address values...");
        // --------------
        // Set label address values, whether assembler labels or assembler directive labels.
        // ++ opcode:lxi:00100001:h:scoreL
        // ++ lb:scoreL:0
        // ++ hb:0
        // Variable name and address value:
        // ++ scorel: 40
        // Program bytes become:
        // ++ lb:scoreL:40
        // ++ hb:0
        // --------------
        // Similar to a label:
        // ++ opcode:call:11001101:PrintLoop
        // ++ lb:PrintLoop:19
        // ++ hb:0
        // Label data:
        // ++ Label, lb:PrintLoop:19
        // ++ Label, hb:0
        // --------------
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
                    System.out.println("++ Label, " + theValue + ":" + labelAddress);
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
                    System.out.println("++ Label, " + theValue + ":" + lb);
                    // hb:
                    it.next();
                    i++;
                    programBytes.set(i, "hb:" + hb);
                    System.out.println("++ Label, hb:" + hb);
                }
            }
            i++;
        }
        System.out.println("+ Label address values, set.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Immediate variable name and value management.
    // Assembler directive: DB and DS Variable name management:
    // + Parsing, listing, and setting label program byte values.
    private final List<String> variableName = new ArrayList<>();
    private final List<Integer> variableValue = new ArrayList<>();

    private void parseName(String theName, String theValue) {
        variableName.add(theName);
        if (theValue.endsWith("h")) {
            // Hex number. For example, change 0ffh or ffh to integer.
            // Samples: 0ffh, 0eh
            int si = 0;
            if (theValue.startsWith("0") && theValue.length() > 3) {
                si = 1;
            }
            theValue = theValue.substring(si, theValue.length() - 1);   // Hex string to integer. Remove the "h".
            variableValue.add(Integer.parseInt(theValue, 16));
        } else {
            variableValue.add(Integer.parseInt(theValue));
        }
        System.out.println("++ Variable name: " + theName + ", value: " + theValue);
    }

    private void parseDb(String theName, String theValue) {
        System.out.println("++ DB variable name: " + theName + ", string of bytes: " + theValue);
        labelName.add(theName);
        labelAddress.add(programTop);        // Address to the string of bytes.
        for (int i = 1; i < theValue.length() - 1; i++) {
            // Remove quotes, 'Hello' -> Hello
            programBytes.add("dbname:" + theName + SEPARATOR + theValue.substring(i, i + 1));
            programTop++;
        }
        programBytes.add("dbstringterminator:" + theName + SEPARATOR + DB_STRING_TERMINATOR);
        programTop++;

    }

    // ------------------------
    public void listImmediateValues() {
        System.out.println("\n+ List immediate values...");
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
    private String getImmediateValue(String findName) {
        // System.out.println("\n+ getImmediateValue, findName: " + findName);
        if (findName.equals("'^^'")) {
            return Integer.toString(SEPARATOR.charAt(0));
        }
        if (findName.startsWith("'") && findName.endsWith("'")) {
            // Need to handle characters, example: ":" return 58 (colon ascii value).
            return Integer.toString(findName.charAt(1));
        }
        String returnString = findName;
        int returnValue = NAME_NOT_FOUND;
        Iterator<String> lName = variableName.iterator();
        Iterator<Integer> lValue = variableValue.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theValue = lValue.next();
            if (theName.toLowerCase().equals(findName.toLowerCase())) {
                returnValue = theValue;
                // System.out.println("+ Found theValue: " + returnValue);
                break;
            }
        }
        if (returnValue == NAME_NOT_FOUND) {
            //System.out.println("+ getImmediateValue, not found: " + findName + ".");
            returnString = findName;
            try {
                Integer.parseInt(returnString);
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("\n- Error, immediate label not found: " + findName + ".\n");
            }
        } else {
            returnString = Integer.toString(returnValue);
        }
        // System.out.println("+ getImmediateValue, returnString: " + returnString);
        return returnString;
    }

    private void setProgramByteImmediates() {
        System.out.println("\n+ Set program immediate values...");
        // --------------
        // Set immediate values.
        // Program byte:
        // ++ immediate:TERMB
        // Variable name and value data:
        // ++ termb: 255
        // Set to:
        // ++ immediate:TERMB:255
        // --------------
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("immediate" + SEPARATOR)) {
                String sImmediate = theValue.substring("immediate:".length());
                // ++ immediate:TERMB
                // ++ immediate:42
                // ++ immediate:080h
                if (sImmediate.endsWith("h")) {
                    // Hex number. For example, change 0ffh or ffh to integer.
                    // Samples: 0ffh, 0eh
                    int si = 0;
                    if (sImmediate.startsWith("0") && sImmediate.length() > 3) {
                        si = 1;
                    }
                    sImmediate = sImmediate.substring(si, sImmediate.length() - 1);   // Hex string to integer. Remove the "h".
                    sImmediate = Integer.toString(Integer.parseInt(sImmediate, 16));
                }
                sImmediate = getImmediateValue(sImmediate);
                programBytes.set(i, theValue + SEPARATOR + sImmediate);
                System.out.println("++ " + theValue + SEPARATOR + sImmediate);
                // ++ immediate:TERMB:255
                // ++ immediate:42:42
            }
            i++;
        }
        System.out.println("+ Program immediate values, set.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse opcodes into program bytes.
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
        // Opcode with 2 parameters, example: mvi a,1
        switch (opcode) {
            case "lxi":
                // opcode <register>,<address label|number>
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
    // -------------------------------------------------------------------------
    // Parse program lines.
    private void parseLine(String orgLine) {
        String theLine;
        String theRest;
        int ei;
        int c1;
        //
        label = "";
        opcode = "";
        opcodeBinary = 0;
        p1 = "";
        p2 = "";

        if (ignoreFirstCharacters > 0 && orgLine.length() > ignoreFirstCharacters) {
            theLine = orgLine.substring(ignoreFirstCharacters, orgLine.length()).trim();
        } else {
            theLine = orgLine.trim();
        }
        //
        if (theLine.length() == 0) {
            // Ignore blank lines.
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
        //  1) Start:
        //  2) Halt1:      hlt
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
        //      4.1) org     0
        //      4.2) ds      2
        //      5) TERMB   equ     0ffh
        //      6) Hello   db      "Hello"
        //      7) scoreR  ds       1

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
        String part1asIs = theLine.substring(0, c1);
        if (part1asIs.endsWith(":")) {
            // Label line.
            //  2) Halt1:      hlt
            parseLabel(part1asIs.substring(0, part1asIs.length() - 1)); // Remove the ":".
            theLine = theLine.substring(c1 + 1).trim();
            c1 = theLine.indexOf(" ");
            if (c1 < 1) {
                //  1) Opcode, no parameters, example: "nop".
                opcode = theLine.toLowerCase();
                System.out.println("++ parseLine, Opcode|" + opcode + "|");
                parseOpcode(opcode);
                return;
            }
        }

        String part1 = theLine.substring(0, c1).toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        // System.out.println("++ parseLine, part1|" + part1 + "| theRest|" + theRest + "|");
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
        //      3.1) jmp Next
        //      3.2) jmp 42
        //  Or assembler directives:
        //      4.1) org     0
        //      4.2) ds      2
        c1 = theRest.indexOf(" ");
        if (c1 < 1) {
            if (theLine.startsWith("org")) {
                // Assembler directives:
                //  4.1) org     0
                System.out.println("++ parseLine, org directive, theRest: " + theRest);
                parseOrg(theRest);
                return;
            } else if (theLine.startsWith("ds")) {
                // Assembler directives:
                //  4.2) ds     2
                System.out.println("++ parseLine, ds directive without a label name, theRest|" + theRest);
                parseDs("", theRest);   // No label name required.
                return;
            }
            // Opcode, Single parameter, example: "jmp Next".
            System.out.println("++ parseLine, Opcode|" + part1 + "| theRest|" + theRest + "|");
            parseOpcode(part1, theRest);
            return;
        }
        // ---------------------------------------------------------------------
        // Parse assembler directives:
        //      5) TERMB   equ      0ffh
        //      6) Hello   db       "Hello"
        //      7) scoreR  ds       1
        //
        String part2 = theRest.substring(0, c1).toLowerCase();
        String part3 = theRest.substring(c1 + 1).trim();
        System.out.println("++ parseLine, Directive|" + part1 + "| part2|" + part2 + "| part3|" + part3 + "|");
        // ++ parseLine, Directive|termb| part2|equ| part3|0ffh|
        // ++ parseLine, Directive|hello| part2|db| part3|'Hello'|
        // ++ parseLine, ds directive: part1|scorer| part3|1|
        // ------------------------------------------
        if (part2.equals("equ")) {
            // Variable name and value.
            // 5) TERMB   equ      0ffh
            if (part3.equals("$")) {
                // stack   equ $    ; "$" is current address.
                part3 = Integer.toString(programTop);
                parseLabel(part3);
                return;
            }
            System.out.println("++ parseLine, equ directive: part1|" + part1 + "| part3|" + part3 + "|");
            parseName(part1, part3);
            return;
        }
        if (part2.equals("db")) {
            // String of bytes.
            // 6) Hello   db       "Hello"
            System.out.println("++ parseLine, db directive: part1|" + part1 + "| part3|" + part3 + "|");
            parseDb(part1, part3);
            return;
        }
        if (part2.equals("ds")) {
            // Number of uninitialezed bytes.
            // 7) scoreR  ds       1
            System.out.println("++ parseLine, ds directive: part1|" + part1 + "| part3|" + part3 + "|");
            if (part3.equals("1")) {
                parseDs(part1asIs, part3);
            }
            return;
        }
        System.out.print("-- Error parsing line: " + theLine);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // File level process: parse and listing.
    public void printProgramByteArray(asmProcessor thisProcess, String theReadFilename) {
        errorCount = 0;
        thisProcess.parseFile(theReadFilename);
        thisProcess.setProgramByteAddresses();
        thisProcess.setProgramByteImmediates();
        thisProcess.printProgramBytesArray();
        if (errorCount > 0) {
            System.out.println("-- Number of errors: " + errorCount);
        }
    }

    public void parseFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                errorCount++;
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
        if (errorCount > 0) {
            System.out.println("-- parseFile, Number of errors: " + errorCount);
        }
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
                errorCount++;
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
    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("++ Start.");
        asmProcessor thisProcess = new asmProcessor();

        System.out.println("\n+ Parse file lines.");
        //thisProcess.printProgramByteArray(thisProcess,
        //    "/Users/dthurston/Projects/arduino/Altair101/asm/programs/opCpi.asm");

        // Assemble the Pong program.
        // ignoreFirstCharacters = 12;     // Set to 12 for Pong program.
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pong.asm");
        //
        // Or other programs.
        // Required, starts the process:
        thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/opTemplate.asm");
        //
        // Optional, used for debugging:
        thisProcess.listLabelAddresses();
        thisProcess.listImmediateValues();
        //
        // Required, sets actual values:
        thisProcess.setProgramByteAddresses();
        thisProcess.setProgramByteImmediates();
        //
        // Optional, used for debugging:
        // thisProcess.listProgramBytes();
        //
        // Required, prints the output for use in Processor.ino:
        thisProcess.printProgramBytesArray();
        //
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
        }

        System.out.println("++ Exit.");
    }
}
