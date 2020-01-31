/*
    ----------------------------------------------------------------------------
    Altair 101 Assembler Program

    The basic assembler works:
    + It can convert assembly language opcodes and assembler directives into executable machine code.
    + It is designed for Intel 8080/8085 opcodes.
    + The converted, Kill the Bit program, run on the Altair 101 dev machine.

> file programs/opMvi.asm
> parsefile
> writebytes

    ---------------------------------------------
    + Pong program work notes.

    + I updated to have EQU as an immediate byte and a name-value label.
    + Need to test. For example,
    ++      SPEED   equ     0eh         ;higher value is faster
    ++      lxi     b,SPEED             ;BC=adder for speed

    + The following is assembled in this program, but not handled in Processor.ino.
    + Since Altair 101 has its own stack space, I can ignore the following.
    ++      lxi     sp,stack    ;init stack pointer 
    ++ Note, if I don't ignore, need to check the following.
    ++      stack   equ     $

    ---------------------------------------------
    Next assembler updates and issues,

    --------------
    +++ Testing: opLadSta.asm.

    + Incorrect address, lb should be 55:
++      30: 00100001 : 21 > opcode: lxi h,Addr1
++      31: 00111000 : 38 > lb: 56
++      32: 00000000 : 00 > hb: 0
    ...
++      52: 11000011 : C3 > opcode: jmp Halt
++      53: 00000011 : 03 > lb: 3
++      54: 00000000 : 00 > hb: 0
++      55: 00000000 : 00 > dsname: Addr1 : 0

    --------------
    + The following causes address error. printPrompt was set to the same address as prompt.
    ++ The processor calls address prompt, instead of address printPrompt.
    ...
    call printPrompt
    ...
    prompt      db      '+ Enter byte > '
    printPrompt:
                call printNL        ; Print prompt.
    --------------

    + Use TERMB EQU value, as this program's DB_STRING_TERMINATOR, and keep the default.
    ++ This allows an override.

    + Create more opcode test programs and samples,
    ++ looping, branching, sense switch interation.
    ++ Create subroutines such as print and println.

    Next opcodes to add/test,
    + Opcodes implemented in the assembler, but not tested with an assembler generated program:
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
    Space = 32
    ! = 33
    0 = 48
    A = 65
    a = 97
    Printable character in ascii decimal order:
        !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~

    ----------------------------------------------------------------------------
 */
package asm;

import java.io.*;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class asmProcessor {

    asmOpcodes theOpcodes = new asmOpcodes(); // Use to get an opcode's binary value.
    //
    private String opcode = "";
    private byte opcodeBinary;
    private String sOpcodeBinary;
    private String p1;
    private String p2;
    private final String SEPARATOR = ":";
    private final String SEPARATOR_TEMP = "^^";
    private final int NAME_NOT_FOUND = -1;
    private final int DB_STRING_TERMINATOR = 255;   // ffh = B11111111
    private int errorCount = 0;
    private final static int ignoreFirstCharacters = 0;
    //
    // Use for storing program bytes and calculating label addresses.
    private int programCounter = 0;
    private int programTop = 0;
    private final static List<String> programBytes = new ArrayList<>();

    private String label;
    private final static List<String> labelName = new ArrayList<>();
    private final static List<Integer> labelAddress = new ArrayList<>();

    private final static List<String> variableName = new ArrayList<>();
    private final static List<Integer> variableValue = new ArrayList<>();

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

    // -------------------------------------------------------------------------
    public int getErrorCount() {
        return this.errorCount;
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Program byte output: Listing byte information to screen
    //  and writing bytes to a file.
    //
    public void programListWrite(String theFileNameTo) {
        byte[] fileBytes = new byte[1024];    // Hold the bytes to be written.
        System.out.println("\n+ Print Program Bytes and description.");
        // ++ <count>: binary   :hex > description.
        System.out.println("++ Address:byte      databyte :hex > description");
        //                  ++       0:00000000: 11000011 : C3 > opcode: jmp Test
        programTop = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            // System.out.println("++ " + theValue);
            //
            String programCounterPadding = "";
            if (programTop < 10) {
                programCounterPadding = "  ";
            } else if (programTop < 100) {
                programCounterPadding = " ";
            }
            System.out.print("++     " + programCounterPadding + programTop + ":" + byteToString((byte)programTop) + ": ");
            String[] opcodeValues = theValue.split(SEPARATOR);
            String keyword = opcodeValues[0];
            switch (keyword) {
                case "opcode":
                    // opcode:jmp:11000011:Start
                    // opcode:call:11001101:PrintLoop
                    // opcode:ret:11001001
                    // pcode:mov:01111000:a:b
                    // System.out.print(opcodeValues[2] + " " + theValue + " > opcode: " + opcodeValues[1]);
                    System.out.print(opcodeValues[2] + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2], 2)));
                    System.out.print(" > opcode: " + opcodeValues[1]);
                    if (opcodeValues.length > 3) {
                        System.out.print(" " + opcodeValues[3]);
                    }
                    if (opcodeValues.length > 4) {
                        System.out.print("," + opcodeValues[4]);
                    }
                    System.out.println("");
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2], 2);
                    ;
                    break;
                case "lb":
                    // ++ lb:Start:14
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.println(" > lb: " + opcodeValues[2]); // byteToString(value[i])
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "hb":
                    // ++ hb:0
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[1])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[1])));
                    System.out.println(" > hb: " + opcodeValues[1]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[1]);
                    break;
                case "immediate":
                    // ++ immediate:'l':108
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.println(" > immediate: " + opcodeValues[1] + " : " + opcodeValues[2]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "databyte":
                    // ++ databyte:abc:k
                    char[] ch = new char[1];
                    ch[0] = opcodeValues[2].charAt(0);
                    System.out.print(byteToString((byte) (int) ch[0]) + " : ");
                    System.out.print(String.format("%02X", (int) ch[0]));
                    System.out.println(" > databyte: " + opcodeValues[2] + " : " + (int) ch[0]);
                    fileBytes[programTop] = (byte) (int) ch[0];
                    break;
                case "dbterm":
                    // dbterm:def:255
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.println(" > dbterm: " + opcodeValues[2]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "dsname":
                    // ++ dsname::0
                    // ++ dsname:org:0
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.println(" > dsname: " + opcodeValues[1] + " : " + opcodeValues[2]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                default:
                    System.out.println("- Error, unknown keyword: " + keyword + " at: " + programTop);
                    break;
            }
            programTop++;
        }
        System.out.println("+ End of list.");
        if (!theFileNameTo.equals("")) {
            // System.out.println("+ Write the bytes to the file:  " + theFileNameTo);
            File dirTo = new File(theFileNameTo);
            try {
                try (OutputStream out = new FileOutputStream(dirTo)) {
                    out.write(fileBytes, 0, programTop);   // programTop is the number of bytes to write.
                    System.out.println("+ Machine code file created:  " + theFileNameTo);
                }
            } catch (IOException e) {
                System.out.println("-- Error, writing to file: " + e);
                System.exit(1);
            }
        }

    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Address label name and value management.
    //  Parsing, listing, and setting address program byte values.
    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ Label Name: " + label + ", Address: " + programTop);
    }

    private void parseLabelValue(String theName, String theValue) {
        // Address label and value.
        // ++ Variable name: speed, value: 0e
        labelName.add(theName);
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
        labelAddress.add(intValue);
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
        labelAddress.add(programTop);        // Address to the bytes.
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
        System.out.println("+ getLabelAddress, findName: " + findName);
        int returnValue = NAME_NOT_FOUND;
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
        if (returnValue == NAME_NOT_FOUND && findName.endsWith("h")) {
            // Hex number. For example, change 0ffh or ffh to integer: 255.
            // Samples: 0ffh, 0eh, 500h(00000101 00000000)
            int si = 0;
            if (findName.startsWith("0") && findName.length() > 3) {
                si = 1;
            }
            findName = findName.substring(si, findName.length() - 1);   // Hex string to integer. Remove the "h".
            returnValue = Integer.parseInt(findName, 16);
        } else if (returnValue == NAME_NOT_FOUND) {
            try {
                returnValue = Integer.parseInt(findName);
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("\n- Error, programTop: " + programTop + ", invalid address value for: " + findName + ".\n");
                returnValue = NAME_NOT_FOUND;
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
            // Only use what is contained within the quotes, 'Hello' -> Hello
            if (theValue.substring(i, i + 1).equals(SEPARATOR)) {
                programBytes.add("databyte:" + theName + SEPARATOR + SEPARATOR_TEMP);
            } else {
                programBytes.add("databyte:" + theName + SEPARATOR + theValue.substring(i, i + 1));
            }
            programTop++;
        }
        programBytes.add("dbterm:" + theName + SEPARATOR + DB_STRING_TERMINATOR);
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
            // Need to handle characters, example: ":" return 58 (colon ascii value), or "\n" return 10.
            // Reference: https://en.wikipedia.org/wiki/ASCII
            if (findName.charAt(1) == '\\') {
                switch (findName.charAt(2)) {
                    case 'n':
                        // Line feed
                        return "10";
                    case 'b':
                        // Backspace
                        return "7";
                    case 'a':
                        // Bell
                        return "7";
                    case 't':
                        // Tab
                        return "9";
                    default:
                        errorCount++;
                        System.out.println("\n- Error, programTop: " + programTop + ", unhandled escape character: " + findName + ".\n");
                        return "0";
                }
            }
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
                System.out.println("\n- Error, programTop: " + programTop + ", immediate label not found: " + findName + ".\n");
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
            errorCount++;
            System.out.println("\n-- Error, programTop: " + programTop + ", invalid opode: " + opcode + "\n");
            opcode = "INVALID: " + opcode;
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
                System.out.print("-- Error, programTop: " + programTop + " ");
                errorCount++;
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
                System.out.print("-- Error, programTop: " + programTop + " ");
                errorCount++;
                break;
        }
        System.out.println("++ Opcode: "
                + opcode + " " + sOpcodeBinary
                + " p1|" + p1 + "|" + " p2|" + p2 + "|");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse program lines.
    //
    // ---------------------------------------------------------------------
    // Types of opcode lines:
    //      opcode                                          ret
    //      opcode <immediate>                              out 30
    //      opcode <address label>                          call print
    //      opcode <register>,<address label|16-bit number> 
    //      opcode <register|RegisterPair>                  inr m
    //      opcode <RegisterPair>,<address label>           lxi h,prompt
    //      opcode <register>,<immediate>                   mvi a,73
    //      opcode <register>,<register>                    mov b,a
    //
    // Others:
    //      
    // Types of directive lines:
    //                          org     <number>
    //                          ds      2
    //      <variable name>     equ     <immediate>
    //      <address label>     equ     <immediate>
    //      <address label>     db      '<characters>'
    //      <address label>     ds      1
    //
    // ---------------------------------------------------------------------
    // Lines with comments:
    //                      ; Comment line
    //      jmp Next        ; Comment after an assembler statement.
    // Labels:
    //      Start:          ; Label line
    //      Start: jmp Next ; Assembler statement after a label.
    // Number types:
    //      80
    //      80h
    //      080h
    // Immediate types:
    //      80
    //      80h
    //      080h
    //      'a'
    //      '\n'
    // Opcode lines. Opcodes can have 0, 1, or 2 parameters. For example:
    //      0)          nop
    //      1)          jmp Next
    //      1)          jmp <number>
    //      1)          cpi <immediate>
    //      2)          mvi a,<immediate>
    // Or assembler directives:
    //      4)          org     <number>
    //      4)          ds      2
    //      5) var1     equ     <immediate>
    //      6) Hello    db      'Hello, there: yes, there.'
    //      7) scoreR   ds      1
    //      7) scoreS   ds      2
    //
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
        c1 = theLine.indexOf(" ");
        if (c1 < 1) {
            //  1) Opcode, no parameters, example: "nop".
            opcode = theLine.toLowerCase();
            System.out.println("++ parseLine, Opcode|" + opcode + "|");
            if (opcode.equals("end")) {
                // End file processing
                return;
            }
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
        System.out.println("++ parseLine, part1|" + part1 + "| theRest|" + theRest + "|");
        //
        // ---------------------------------------------------------------------
        //
        // stacy
        // Works:
        //  mvi a,3
        // Error:
        //  abc     db     'okay, yes?'
        //      ++ parseLine, Opcode|abc| p1|db     'okay| p2|yes?'|
        //      -- Error, ++ Opcode: INVALID: abc 11000011 p1|db     'okay| p2|yes?'|
        //
        // Check for opcode with 2 parameters, example: mvi a,1
        c1 = theRest.indexOf("'");
        if (c1 > 0) {
            // Take care of the case where "," is inside a DB string.
            // ++ parseLine, part1|abc| theRest|db     'okay, yes?'|
            // ++ parseLine, DB string contains ','.
            int c2 = theRest.indexOf(" ");
            if (c2 > 1) {
                // If c2<i, then not a directive. Example: mvi a,'\n'
                String theDirective = theRest.substring(0, c2).toLowerCase();
                System.out.println("++ parseLine, DB string contains ','. theDirective = " + theDirective + ".");
                if (theDirective.equals("db")) {
                    // String of bytes.
                    // 6) Hello   db       "Hello"
                    String part3 = theRest.substring(c1, theRest.length());
                    System.out.println("++ parseLine, db directive: part1|" + part1 + "| part3|" + part3 + "|");
                    parseDb(part1, part3);
                    return;
                } else if (theDirective.endsWith("'")) {
                    //
                    // Case: mvi a,' '
                    // ++ parseLine, DB string contains ','. theDirective = a,'.
                    // -- Error, programTop: 15, line: mvi a,' '++ parseLine, Opcode|mvi| p1|a| p2|' '|
                    //
                } else {
                    errorCount++;
                    System.out.print("-- Error, programTop: " + programTop + ", line: " + theLine);
                }
            }
        }
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
            // EQU variable names and values, can either be an immediate byte, or an 2 byte address.
            // So, add both, an address label and a immediate name-value pair.
            // 5.1) TERMB   equ     0ffh
            // 5.2) stack   equ     $
            if (part3.equals("$")) {
                // stack   equ $    ; "$" is current address.
                part3 = Integer.toString(programTop);
            }
            System.out.println("++ parseLine, equ directive: part1|" + part1 + "| part3|" + part3 + "|");
            // Stacy, incorrect: ++ Label Name: 0eh, Address: 0
            parseLabelValue(part1, part3);
            // ++ Variable name: speed, value: 0e
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
            // 7) scoreR  ds       2
            System.out.println("++ parseLine, ds directive: part1|" + part1 + "| part3|" + part3 + "|");
            parseDs(part1asIs, part3);
            return;
        }
        System.out.print("-- Error parsing line: " + theLine);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // File level process: parse and listing.
    public void parseFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;

        errorCount = 0;
        programCounter = 0;
        programTop = 0;
        programBytes.clear();
        labelName.clear();
        labelName.clear();
        labelAddress.clear();
        variableName.clear();
        variableValue.clear();
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("-- ERROR, theReadFilename does not exist.");
                errorCount++;
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);
            String theLine = pin.readLine();
            opcode = "start";
            while (theLine != null && !opcode.equals("end")) {
                parseLine(theLine);
                theLine = pin.readLine();
            }
            pin.close();
            System.out.print("+ pin.close() ");
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("");
        if (errorCount > 0) {
            System.out.println("\n-- parseFile, Number of errors: " + errorCount);
            return;
        }
        setProgramByteAddresses();
        if (errorCount > 0) {
            System.out.println("\n-- parseFile, Number of errors: " + errorCount);
            return;
        }
        setProgramByteImmediates();
        if (errorCount > 0) {
            System.out.println("\n-- parseFile, Number of errors: " + errorCount);
            return;
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
    public void uploadFile(String theReadFilename) {
        System.out.println("++ Upload the binary file through the serial port: " + theReadFilename);
        System.out.println("++ Not available, yet.");
    }

    public void showFile(String theReadFilename) {
        // System.out.println("++ Show binary file: " + theReadFilename);
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
        int i;
        int tenCount = 0;
        for (i = 0; i < theLength; i++) {
            if (tenCount == 10) {
                tenCount = 0;
                System.out.println("");
            }
            tenCount++;
            System.out.print(String.format("%02X ", bArray[i]));
        }
        System.out.println("\n+ Hex display completed.");
        tenCount = 0;
        for (i = 0; i < theLength; i++) {
            if (tenCount == 10) {
                tenCount = 0;
                System.out.println("");
            }
            tenCount++;
            System.out.print(String.format("%03o ", bArray[i]));
        }
        System.out.println("\n+ Octal display completed.");
        tenCount = 0;
        for (i = 0; i < theLength; i++) {
            if (tenCount == 10) {
                tenCount = 0;
                System.out.println("");
            }
            tenCount++;
            System.out.print(byteToString(bArray[i]) + " ");
        }
        System.out.println("\n+ Binary display completed.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // For testing.
    public static void main(String args[]) {
        System.out.println("++ Start.");
        asmProcessor thisProcess = new asmProcessor();

        System.out.println("\n+ Parse file lines.");
        /*
        thisProcess.showFile("10000000.bin");
        if (1 == 1) {
            return;
        }
         */
        //thisProcess.printProgramByteArray(thisProcess,
        //    "/Users/dthurston/Projects/arduino/Altair101/asm/programs/opCpi.asm");
        // Assemble the Pong program.
        // ignoreFirstCharacters = 12;     // Set to 12 for Pong program.
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pong.asm");
        //
        // Or other programs.
        // Required, starts the process:
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pSenseSwitchInput.asm");
        thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/opJmp.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pKillTheBit.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/p1.asm");
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
            return;
        }
        //
        // Option: for debugging:
        // thisProcess.listLabelAddresses();
        // thisProcess.listImmediateValues();
        //
        // Required, sets actual values:
        //
        // Option: create a binary file of the program, which has a nice listing.
        // thisProcess.programListWrite("10000000.bin");
        // thisProcess.showFile("10000000.bin");
        //
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
        }

        System.out.println("++ Exit.");
    }
}
