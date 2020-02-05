/*
    ----------------------------------------------------------------------------
    Altair 101 Assembler Program

    The basic assembler works:
    + It can convert assembly language opcodes and assembler directives into executable machine code.
    + It is designed for Intel 8080/8085 opcodes.
    + The converted, Kill the Bit program, run on the Altair 101 dev machine.

    ---------------------------------------------
    +++ Next assembler updates and issues,

    + Make label and immediate names case sensitive.

    + Cleanup parseLine() code.

    Improve error handling by adding source line number to the error message.

    + Test, incorrect: ++ Label Name: 0eh, Address: 0

    + Use TERMB EQU value, as this program's DB_STRING_TERMINATOR, and keep the default.
    ++ This allows an override.

    ----------------------------------------------------------------------------
    Program sections:
    + Program byte output: listing and writing program bytes to file.
    + Address label name value pair management.
    + Immediate variable name value pair management.
    + Assembler directives: org, ds, db, equ.
    + Parse address label.
    + Parse opcodes into program bytes.
    + Parse program lines.
    + File level process: read files, and parse or list the file lines.

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse program lines.
    //
    // ---------------------------------------------------------------------
    // Lines with comments:
    //                      ; Comment line
    //      jmp Next        ; Comment after an assembler statement.
    //
    // Address label lines:
    //      Start:
    //      Start:          ; with a comment
    //      Start: jmp Next ; Assembler statement after a label.
    //
    // Label Address byte:
    //      ++ lb:Final:42
    //      ++ lb:<address>:<value>
    // Label address types:
    //                          Sample  Sample source
    //      Immediate type      source  Byte data           With value
    //      --------------      ------  ----------------    -------------
    //      Label               Final   lb:Final:           lb:Final:42
    //      Hex                 80h     lb:80h:             lb:80h:128
    //      Hex                 080h    lb:080h:            lb:080h:128
    //      Decimal             42      lb:42:              lb:42:42
    //      Unknown label       Fianl   lb:Fianl:           lb:Fianl:-1
    //
    // -----------------------------------------------------
    // Opcode lines. Opcodes can have 0, 1, or 2 parameters. For example:
    //      nop
    //      jmp Next
    //      cpi 73
    //      mvi a,var1
    //
    // Types of opcode lines:
    //      opcode                                          ret
    //      opcode <immediate>                              out 30
    //      opcode <address label>                          call print
    //      opcode <register>,<immediate>                   mvi a,73
    //      opcode <register>,<register>                    mov b,a
    //      opcode <register|RegisterPair>                  inr b
    //      opcode <RegisterPair>,<address label>           lxi h,prompt
    //      opcode <register>,<address label|16-bit number> 
    //
    // -----------------------------------------------------
    // Assembler directives line samples:
    //                org     0
    //                ds      2
    //       Final    equ     42
    //       Hello    db      'Hello, there: yes, there.'
    //       scoreR   ds      1
    //       scoreS   ds      2
    //
    // Types of assembler directive lines:
    //                          org     <number>
    //                          ds      <number>
    //      <address label>     ds      <number>
    //      <address label>     db      '<characters>'
    //      <address label>     equ     <number|$>
    //      <variable name>     equ     <immediate>
    //
    //                      Sample  Sample source
    // Immediate type       source  Byte data           With value
    // --------------       ------  ----------------    -------------
    // Separator character  ':'     immediate:'^^'      immediate:'^^':58
    // Escape character     '\n'    immediate:'\n'      immediate:'\n':10
    // Character            'a'     immediate:'a'       immediate:'a':97
    // Label                Final   immediate:Final     immediate:Final:42
    // Unknown label        Fianl   immediate:Fianl     immediate:Fianl:-1
    // Hex                  80h     immediate:80h       immediate:80h:128
    // Hex                  080h    immediate:080h      immediate:080h:128
    // Decimal              42      immediate:42        immediate:42:42
    //
    // ---------------------------------------------------------------------
    //
    ---------------------------------------------
    +++ Nice to show the actual line, but not required because I can search for the label, "Fianl".
- Error, immediate label not found: Fianl.
- Error, programTop byte# 53 : immediate:Fianl.

    ---------------------------------------------
    + Pong program work notes.

    + The following is assembled in this program, but not handled in Processor.ino.
    + Since Altair 101 has its own stack space, I can ignore the following.
    ++      lxi     sp,stack    ;init stack pointer 

    ---------------------------------------------
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
    private final int DB_STRING_TERMINATOR = 255;   // ffh = B11111111
    private final static int ignoreFirstCharacters = 0;
    //
    // Use for storing program bytes and calculating label addresses.
    private int programTop = 0;
    private final static List<String> programBytes = new ArrayList<>();

    private String label;
    private final static List<String> labelName = new ArrayList<>();
    private final static List<Integer> labelAddress = new ArrayList<>();

    private final static List<String> variableName = new ArrayList<>();
    private final static List<Integer> variableValue = new ArrayList<>();

    // -------------------------------------------------------------------------
    // Error handling
    //
    private final int NAME_NOT_FOUND = -1;
    private final String NAME_NOT_FOUND_STR = "-1";
    private boolean debugMessage = false;
    private int errorCount = 0;

    public int getErrorCount() {
        return this.errorCount;
    }

    private void printDebug(String theMessage) {
        if (debugMessage) {
            System.out.print(theMessage);
        }
    }

    private void printlnDebug(String theMessage) {
        if (debugMessage) {
            System.out.print(theMessage + "\n");
        }
    }

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
    // -------------------------------------------------------------------------
    // Program byte output: Listing byte information to screen
    //  and writing bytes to a file.
    //
    public void programBytesListAndWrite(String theFileNameTo) {
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
            System.out.print("++     " + programCounterPadding + programTop + ":" + byteToString((byte) programTop) + ": ");
            // Only works up to 255 byte address. Example:
            // ++     255:11111111: 00000001 : 01 > hb: 1
            // ++     256:00000000: 11000011 : C3 > opcode: ...
            //
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
    private String convertValueToInt(String sValue) {
        String returnString = NAME_NOT_FOUND_STR;
        //
        // Immediate type       Source Value    To integer value
        // --------------       ------------    ----------------
        // Separator character  '^^'            58 (Example, ":")
        // Escape character     '\n'            10
        // Character            'a'             97
        // Hex                  080h            128
        // Decimal              42              42
        //
        if (sValue.equals("'^^'")) {
            // If SEPARATOR is ":", set to 58 (colon ascii value).
            returnString = Integer.toString(SEPARATOR.charAt(0));
        } else if (sValue.startsWith("'") && sValue.endsWith("'")) {
            // Reference: https://en.wikipedia.org/wiki/ASCII
            if (sValue.charAt(1) == '\\') {
                // Handle escape characters, example: "\n" return 10.
                switch (sValue.charAt(2)) {
                    case 'n':
                        // Line feed
                        returnString = "10";
                        break;
                    case 'b':
                        // Backspace
                        returnString = "7";
                        break;
                    case 'a':
                        // Bell
                        returnString = "7";
                        break;
                    case 't':
                        // Tab
                        returnString = "9";
                        break;
                    default:
                        errorCount++;
                        System.out.println("\n- getImmediateValue, Error, programTop: " + programTop + ", unhandled escape character: " + sValue + ".\n");
                        returnString = NAME_NOT_FOUND_STR;
                }
            } else {
                // Non-escape character converted to an integer, then to a string.
                returnString = Integer.toString(sValue.charAt(1));
            }
        } else if (sValue.endsWith("h")) {
            // Hex number. For example, change 0ffh or ffh to an integer.
            // Other samples: 0ffh, 0eh
            int si = 0;
            if (sValue.startsWith("0") && sValue.length() > 3) {
                si = 1;
            }
            returnString = sValue.substring(si, sValue.length() - 1);     // Hex string to integer. Remove the "h".
            try {
                Integer.parseInt(returnString);
                returnString = Integer.toString(Integer.parseInt(returnString, 16));
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("");
                System.out.println("- Error, invalid value: " + sValue + ".");
                System.out.println("");
            }
        } else {
            // --------------
            // Since it's not a label, check if it's a valid integer.
            printlnDebug("+ Not found: " + sValue + ".");
            try {
                Integer.parseInt(sValue);
                returnString = sValue;
            } catch (NumberFormatException e) {
                // sImmediate = "*** Error, label not found.";
                errorCount++;
                System.out.println("");
                System.out.println("- Error, immediate label not found: " + sValue + ".");
                System.out.println("");
            }
        }
        return returnString;
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Label address name value pair management.
    //
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
        printlnDebug("+ getLabelAddress, findName: " + findName);
        int returnValue = NAME_NOT_FOUND;
        Iterator<String> lName = labelName.iterator();
        Iterator<Integer> lAddress = labelAddress.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theAddress = lAddress.next();
            if (theName.toLowerCase().equals(findName.toLowerCase())) {
                returnValue = theAddress;
                printlnDebug("+ Found theAddress: " + returnValue);
                break;
            }
        }
        if (returnValue == NAME_NOT_FOUND) {
            returnValue = Integer.parseInt(convertValueToInt(findName));
        }
        return returnValue;
    }

    // ------------------------
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
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            if (theValue.startsWith("lb:")) {
                //
                int intAddress = getLabelAddress(theValue.substring(3));
                //
                // Label Address byte:
                //      ++ lb:Final:42
                //      ++ lb:<address>:<value>
                // Label address types:
                //                          Sample  Sample source
                //      Immediate type      source  Byte data           With value
                //      --------------      ------  ----------------    -------------
                //      Label               Final   lb:Final:           lb:Final:42
                //      Hex                 80h     lb:80h:             lb:80h:128
                //      Hex                 080h    lb:080h:            lb:080h:128
                //      Decimal             42      lb:42:              lb:42:42
                //      Unknown label       Fianl   lb:Fianl:           lb:Fianl:-1
                //
                if (intAddress == NAME_NOT_FOUND) {
                    printlnDebug("- Label address not found for program byte: " + theValue);
                } else if (intAddress < 256) {
                    // lb:
                    String theLabelAddress = Integer.toString(intAddress);
                    programBytes.set(i, theValue + SEPARATOR + theLabelAddress);
                    printlnDebug("++ Label, " + theValue + ":" + theLabelAddress);
                    // hb:
                    // Already set to default of 0.
                    it.next();
                    printlnDebug("++ Label, " + theValue);
                    i++;
                } else {
                    // Need to also set high byte(hb).
                    // Address: 265, in binary hb=00000001(digital=1) lb=00001001(digital=9)
                    int hb = intAddress / 256;
                    int lb = intAddress - (hb * 256);
                    //-------------------
                    // lb:
                    programBytes.set(i, theValue + SEPARATOR + lb);
                    printlnDebug("++ Label, " + theValue + ":" + lb);
                    // hb:
                    it.next();
                    i++;
                    programBytes.set(i, "hb:" + hb);
                    printlnDebug("++ Label, hb:" + hb);
                }
            }
            i++;
        }
        System.out.println("+ Finished setting label address values.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Immediate name value pair management.
    //
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
        // Return either a numeric value as a string,
        //  or NAME_NOT_FOUND_STR.
        printlnDebug("\n+ getImmediateValue, findName: " + findName);
        String returnString = NAME_NOT_FOUND_STR;
        Iterator<String> lName = variableName.iterator();
        Iterator<Integer> lValue = variableValue.iterator();
        while (lName.hasNext()) {
            String theName = lName.next();
            int theValue = lValue.next();
            if (theName.toLowerCase().equals(findName.toLowerCase())) {
                returnString = Integer.toString(theValue);
                printlnDebug("+ Found: " + returnString);
                break;
            }
        }
        printlnDebug("+ getImmediateValue, returnString: " + returnString);
        return returnString;
    }

    private void setProgramByteImmediates() {
        System.out.println("\n+ Set program immediate values...");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theSource = it.next();
            if (theSource.startsWith("immediate" + SEPARATOR)) {
                String theValue = theSource.substring("immediate:".length());
                String nameValue = getImmediateValue(theValue);
                printlnDebug("+ getImmediateValue returned, theValue=" + theValue + " nameValue=" + nameValue);
                if (nameValue.equals(NAME_NOT_FOUND_STR)) {
                    // Since it's not a label, convert it to an integer.
                    printlnDebug("+ Not found: " + theValue + ".");
                    theValue = convertValueToInt(theValue);
                } else {
                    theValue = nameValue;
                }
                //
                // Immediate type       Sample source       With value
                // --------------       ----------------    -------------
                // Separator character  immediate:'^^'      immediate:'^^':58 (Example, ":")
                // Escape character     immediate:'\n'      immediate:'\n':10
                // Character            immediate:'a'       immediate:'a':97
                // Label                immediate:Final     immediate:Final:42
                // Unknown label        immediate:Fianl     immediate:Fianl:-1
                // Hex                  immediate:080h      immediate:080h:128
                // Decimal              immediate:42        immediate:42:42
                //
                programBytes.set(i, theSource + SEPARATOR + theValue);
                printlnDebug("++ " + theSource + SEPARATOR + theValue);
                // ++ immediate:Final:42
                // ++ immediate:42:42
            }
            i++;
        }
        System.out.println("+ Finished setting immediate values.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Assembler directives
    //
    //      org
    //      ds
    //      db
    //      equ
    // ----------------------------------------
    private void parseOrg(String theValue) {
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
            programBytes.add("dsname:org" + SEPARATOR + "0");  // Default value is "0".
        }
    }

    private void parseDs(String theName, String theValue) {
        System.out.println("++ DS variable name: " + theName + ", number of bytes: " + theValue);
        if (!theName.equals("")) {
            // Case, there is a label name, for example:
            //      stack ds 6
            // Add an address to the bytes.
            labelName.add(theName);
            labelAddress.add(programTop);
        }
        for (int i = 0; i < Integer.parseInt(theValue); i++) {
            programBytes.add("dsname:" + theName + SEPARATOR + 0);  // default value.
            programTop++;
        }
    }

    private void parseDb(String theName, String theValue) {
        System.out.println("++ DB variable name: " + theName + ", string of bytes: " + theValue);
        // Add an address to the bytes. For example,
        //      Hello db 'Hello there.'
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

    // -----------
    // EQU can be an address value or an immediate value.
    private void parseEquLabelValue(String theName, String theValue) {
        // Address label value pair.
        labelName.add(theName);
        int intValue = Integer.parseInt(convertValueToInt(theValue));
        labelAddress.add(intValue);
        System.out.println("++ parseEquLabelValue, Name: " + theName + ", Value: " + intValue);
    }

    private void parseEquNameValue(String theName, String theValue) {
        // Name value pair
        variableName.add(theName);
        int intValue = Integer.parseInt(convertValueToInt(theValue));
        variableValue.add(intValue);
        System.out.println("++ parseEquNameValue, Name: " + theName + ", Value: " + intValue);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse opcodes into program bytes.
    //
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
        //  For example,
        //      hlt
        //      nop
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
    //
    // Address label name and value management.
    //  For example,
    //      Start:
    //
    private void parseLabel(String label) {
        // Address label
        labelName.add(label);
        labelAddress.add(programTop);
        System.out.println("++ parseLabel, Name: " + label + ", Address: " + programTop);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse each program source line.
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
                } else if (theDirective.endsWith("'") || theDirective.equals("equ")) {
                    //
                    // Case: mvi a,' '
                    // ++ parseLine, DB string contains ','. theDirective = a,'.
                    // -- Error, programTop: 15, line: mvi a,' '++ parseLine, Opcode|mvi| p1|a| p2|' '|
                    //
                } else {
                    errorCount++;
                    System.out.println("-- Error, programTop: " + programTop + ", line: " + theLine);
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
            // EQU variable names and values, can either be an immediate byte, or a 2 byte address.
            // So, add both, an address label and a immediate name-value pair.
            //      TERMB   equ     0ffh
            //      stack   equ     $
            if (part3.equals("$")) {
                // stack   equ $    ; "$" is current address.
                part3 = Integer.toString(programTop);
            }
            System.out.println("++ parseLine, equ directive: part1|" + part1 + "| part3|" + part3 + "|");
            parseEquLabelValue(part1, part3);
            parseEquNameValue(part1, part3);
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
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/opImmediate.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pKillTheBit.asm");
        thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/p1.asm");
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
            return;
        }
        //
        // Option: for debugging:
        thisProcess.listLabelAddresses();
        thisProcess.listImmediateValues();
        thisProcess.programBytesListAndWrite("");
        //
        // Required, sets actual values:
        //
        // Option: create a binary file of the program, which has a nice listing.
        // thisProcess.programBytesListAndWrite("10000000.bin");
        // thisProcess.showFile("10000000.bin");
        //
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
        }

        System.out.println("++ Exit.");
    }
}
