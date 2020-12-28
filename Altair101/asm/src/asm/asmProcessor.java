/*
    ----------------------------------------------------------------------------
    Altair 101 Assembler Program

    The basic assembler works:
    + It can convert assembly language opcodes and assembler directives into executable machine code.
    + It is designed for Intel 8080/8085 opcodes.
    + The converted, Kill the Bit program, runs on the Altair 101 dev machine.

    Binary files:
    https://www.autometer.de/unix4fun/z80pack/
    https://www.autometer.de/unix4fun/z80pack/ftp/

    Run CPM: 
    https://schorn.ch/altair.html
    For the ladder.com game, but how to get them on the disk?
    http://www.classiccmp.org/cpmarchives/cpm/Software/rlee/C/COMPUPRO/GAMES/

    The PCGET program uploads one file at a time. Is there a way to upload multiple files, or to create a .dsk file on the PC itself?
    sim> attach dsk0 cpm48.dsk
    sim> boot dsk

    ---------------------------------------------
    +++ Next assembler updates and issues,

    + Make label and immediate names case sensitive.

    + Cleanup parseLine() code.

    Improve error handling by adding source line number to the error message.

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

    Assembler directives:
    + org :
    //                org     0
    + ds  :  
    //                ds      2
    //       scoreR   ds      1
    + db  :
    //       Hello    db      'Hello, there: yes, there.'
    + equ :
    //       Final    equ     42

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
    // Label Address, 1byte + 1 byte = 16-bit address:
    //      ++ lb:Final:42
    //      ++ lb:<address>:<value>
    // Label address types:
    //                          Sample      Sample source
    //      Immediate type      source      Byte data           With value
    //      --------------      ------      ----------------    -------------
    //      Label               Final       lb:Final:           lb:Final:42
    //      Hex                 80h         lb:80h:             lb:80h:128
    //      Hex                 080h        lb:080h:            lb:080h:128
    //      Binary              00001000b   lb:8:               lb:00001000b:8
    //      Decimal             42          lb:42:              lb:42:42
    //      Unknown label       Fianl       lb:Fianl:           lb:Fianl:-1
    //
    // -----------------------------------------------------
    // Opcode lines. Opcodes can have 0, 1, or 2 parameters. For example:
    //      nop
    //      jmp Next
    //      cpi 73
    //      mvi a,var1
    //
    // Types of componets and parameter:
    //      <immediate> = <number>|'<character>'|<variable name>
    //      <number> = <digits>|<digits>H|<digits>B
    //      <register> = A|B|C|D|E|H|L|M
    //      <RegisterPair> = B|D|H
    //      <label> = String with no spaces
    //
    // Types of opcode lines:
    //      <label>: <opcode>
    //      <opcode>                                          ret
    //      <opcode> <immediate>                              out 30
    //      <opcode> <label>                                  call print
    //      <opcode> <register>,<immediate>                   mvi a,73
    //      <opcode> <register>,<register>                    mov b,a
    //      <opcode> <register|RegisterPair>                  inr b
    //      <opcode> <RegisterPair>,<label>                   lxi h,prompt
    //      <opcode> <register>,<label|16-bit number> 
    //
    // Types of Directive lines:
    //      org <number>
    //      DB <number|'<string>'>
    //      <label>[:] DB <number|'<string>'>
    //      DS <number>
    //      <label>[:] DS <number>
    //      <variable name>[:] EQU <$|number>
    //
    // label: all labels are 16-bt address labels.
    // DB : Reserve 1 byte memory with the value of <number>
    // DS : Number of bytes to reserve.
    // EQU : Variable value for replacement in opcode statements for immediates.
    //
    // -----------------------------------------------------
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
    // The EQU directive is used for defining constants.

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

    private final int MEMSIZE = 65536;   // 1024 4096 65536(64K).

    asmOpcodes theOpcodes = new asmOpcodes(); // Use to get an opcode's binary value.
    //
    private String opcode = "";
    private byte opcodeBinary;
    private String sOpcodeBinary;
    private String p1;
    private String p2;
    private final String SEPARATOR = ":";
    private final String SEPARATOR_TEMP = "^^";
    private final int DB_STRING_TERMINATOR = 0;   // To have it automatically added, set to: 255 = ffh = B11111111
    private static Integer ignoreFirstCharacters = 0;
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

    public static Integer getIgnoreFirstCharacters() {
        return ignoreFirstCharacters;
    }

    public static void setIgnoreFirstCharacters(String ignoreFirstCharactersNum) {
        try {
            ignoreFirstCharacters = Integer.parseInt(ignoreFirstCharactersNum);
        } catch (NumberFormatException e) {
            System.out.println("");
            System.out.println("- Error, invalid value: " + ignoreFirstCharactersNum + ", needs to be an integer.");
            System.out.println("");
        }
//        ignoreFirstCharacters = ignoreFirstCharactersNum;
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
    public void programBytesListCode() {
        // byte[] fileBytes = new byte[1024];    // Hold the bytes to be written.
        System.out.println("\n+ List Program byte code.");
        System.out.println("++ MWRITE(<Counter>, B<data byte> & 0xff);  // <byte description>");
        // MWRITE(<Counter>, B<data byte> & 0xff);  // opcode: mvi a,6
        // MWRITE(0, B00111110 & 0xff);  // opcode: mvi a,6
        // MWRITE(1, B00000110 & 0xff);  // immediate: 6 : 6
        //
        //                  ++       0:00000000 00000000: 11000011 : C3:303 > opcode: jmp Start
        //                  ...
        //                  ++     256:00000001 00000000: 00100000 : 20:040 > databyte: orstr :   : 32
        //                  ++     257:00000001 00000001: 01001111 : 4F:117 > databyte: orstr : O : 79
        //
        programTop = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            // System.out.println("++ " + theValue);
            String[] opcodeValues = theValue.split(SEPARATOR);
            System.out.print("MWRITE( cnt++" + ", B");
            if (opcodeValues.length > 2) {
                // System.out.print(opcodeValues[1] + ":" + opcodeValues.length);
                if (opcodeValues[2].length() == 8) {
                    System.out.print(opcodeValues[2]);
                } else {
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])));
                }
            } else {
                System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[1])));
            }
            System.out.print(" & 0xff);  // ");
            System.out.println("++ " + theValue);
            programTop++;
        }
        System.out.println("+ End of list.");
    }

    // -------------------------------------------------------------------------
    public void programBytesListHex() {
        System.out.println("\n+ List Program byte code.");
        System.out.print(":");
        programTop = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            String[] opcodeValues = theValue.split(SEPARATOR);
            if (opcodeValues.length > 2) {
                if (opcodeValues[2].length() == 8) {
                    // System.out.print("(" + opcodeValues[2] + ":");
                    System.out.print(String.format("%02X:", Integer.parseInt(opcodeValues[2], 2)));
                } else {
                    // System.out.print("{" + opcodeValues[2] + ":");
                    System.out.print(String.format("%02X:", Integer.parseInt(opcodeValues[2])));
                }
            } else {
                // System.out.print("[" + opcodeValues[1] + ":");
                System.out.print(String.format("%02X:", Integer.parseInt(opcodeValues[1], 2)));
            }
            programTop++;
        }
        System.out.println("\n+ End of list.");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Program byte output: Listing byte information to screen
    //  and writing bytes to a file.
    //
    public void programBytesListAndWrite(String theFileNameTo) {
        byte[] fileBytes = new byte[MEMSIZE];    // Hold the bytes to be written.
        System.out.println("\n+ Print Program Bytes and description.");
        System.out.println("++ Address:16-bit bytes       databyte :hex:oct > description");
        //                  ++       0:00000000 00000000: 11000011 : C3:303 > opcode: jmp Start
        //                  ...
        //                  ++     256:00000001 00000000: 00100000 : 20:040 > databyte: orstr :   : 32
        //                  ++     257:00000001 00000001: 01001111 : 4F:117 > databyte: orstr : O : 79
        //
        programTop = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theValue = it.next();
            // System.out.println("++ " + theValue);
            //
            String programCounterPadding = "";
            if (programTop < 10) {
                programCounterPadding = "   ";
            } else if (programTop < 100) {
                programCounterPadding = "  ";
            } else if (programTop < 1000) {
                programCounterPadding = " ";
            }
            // 16-bit address listing.
            int lb;
            int hb = 0;
            if (programTop < 256) {
                // 8-bit address
                lb = programTop;
            } else {
                // 16-bit address
                lb = programTop - 256;
                hb = programTop / 256;
            }
            System.out.print("++    " + programCounterPadding + programTop + ":" + byteToString((byte) hb) + " " + byteToString((byte) lb) + ": ");
            //
            // Print of the address only works up to 255 byte address. Example:
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
                    // opcode:sui:11001001:'^^'
                    // System.out.print(opcodeValues[2] + " " + theValue + " > opcode: " + opcodeValues[1]);
                    System.out.print(opcodeValues[2] + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2], 2)));      // Hex
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2], 2)));     // Octal
                    System.out.print(" > opcode: " + opcodeValues[1]);
                    if (opcodeValues.length > 3) {
                        if (opcodeValues[3].equals("'" + SEPARATOR_TEMP + "'")) {
                            System.out.print(" " + "'" + SEPARATOR + "'");
                        } else {
                            System.out.print(" " + opcodeValues[3]);
                        }
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
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2])));    // Octal
                    System.out.println(" > lb: " + opcodeValues[2]); // byteToString(value[i])
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "hb":
                    // ++ hb:0
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[1])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[1])));
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[1])));    // Octal
                    System.out.println(" > hb: " + opcodeValues[1]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[1]);
                    break;
                case "immediate":
                    // ++ immediate:'l':108
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2])));    // Octal
                    //
                    System.out.print(" > immediate: ");
                    if (opcodeValues[1].equals("'" + SEPARATOR_TEMP + "'")) {
                        System.out.print(" " + "'" + SEPARATOR + "'");
                    } else {
                        System.out.print(" " + opcodeValues[1]);
                    }
                    System.out.println(" : " + opcodeValues[2]);
                    //
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "databyte":
                    // ++ databyte:abc:k
                    // ++       6:00000110: 01110100 : 74     > databyte: testnl : t : 116
                    // ++       7:00000111: 00100111 : 27     > databyte: testnl : '\n' : 10
                    // ++      18:00010010: 00001010 : 0A:012 > databyte: p00000000 : '\n' : 10
                    // System.out.println(" > databyte: " + opcodeValues[1] + " : " + opcodeValues[2]);
                    if (opcodeValues[2].charAt(0) == '\'') {
                        // Case, escape character.
                        String eChar = convertValueToInt(opcodeValues[2]);
                        System.out.print(byteToString((byte) Integer.parseInt(eChar)) + " : ");
                        System.out.print(String.format("%02X", Integer.parseInt(eChar)));
                        System.out.print(String.format(":%03o", Integer.parseInt(eChar)));    // Octal
                        System.out.println(" > databyte: " + opcodeValues[1] + " : " + opcodeValues[2] + " : " + eChar);
                        fileBytes[programTop] = (byte) Integer.parseInt(eChar);
                    } else if (opcodeValues[2].equals(SEPARATOR_TEMP)) {
                        // Case, separator character, i.e. separator temp character string which maps to the separator character.
                        // ++ databyte:echo:^^
                        // ++      31:00011111: 00111010 : 3A:072 > databyte: echo : : : 58
                        char[] ch = new char[1];
                        ch[0] = SEPARATOR.charAt(0);
                        System.out.print(byteToString((byte) (int) ch[0]) + " : ");
                        System.out.print(String.format("%02X", (int) ch[0]));
                        System.out.print(String.format(":%03o", (int) ch[0]));    // Octal
                        System.out.println(" > databyte: " + opcodeValues[1] + " : " + SEPARATOR + " : " + (int) ch[0]);
                        fileBytes[programTop] = (byte) (int) ch[0];
                    } else {
                        // Case, non-escape character.
                        char[] ch = new char[1];
                        ch[0] = opcodeValues[2].charAt(0);
                        System.out.print(byteToString((byte) (int) ch[0]) + " : ");
                        System.out.print(String.format("%02X", (int) ch[0]));
                        System.out.print(String.format(":%03o", (int) ch[0]));    // Octal
                        System.out.println(" > databyte: " + opcodeValues[1] + " : " + opcodeValues[2] + " : " + (int) ch[0]);
                        fileBytes[programTop] = (byte) (int) ch[0];
                    }
                    break;
                case "dbterm":
                    // dbterm:def:255
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2])));    // Octal
                    System.out.println(" > dbterm: " + opcodeValues[2]);
                    fileBytes[programTop] = (byte) Integer.parseInt(opcodeValues[2]);
                    break;
                case "dsname":
                    // ++ dsname::0
                    // ++ dsname:org:0
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2])));    // Octal
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
    public String convertValueToInt(String sValue) {
        String returnString = NAME_NOT_FOUND_STR;
        //
        // Immediate type       Source Value    To integer value
        // --------------       ------------    ----------------
        // Separator character  '^^'            58 (Example, ":")
        // Escape character     '\n'            10
        // Character            'a'             97
        // Hex,     base 16     080H            128
        // Hex,     base 16     080h            128
        // Octal,   base 8      012h            10
        // Binary,  base 2      00001000b       8
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
                    case 'r':
                        // Carriage return
                        returnString = "13";
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
                        System.out.println("\n- getVariableValue, Error, programTop: " + programTop + ", unhandled escape character: " + sValue + ".\n");
                        returnString = NAME_NOT_FOUND_STR;
                }
            } else {
                // Non-escape character converted to an integer, then to a string.
                returnString = Integer.toString(sValue.charAt(1));
            }
        } else if (sValue.endsWith("h") || sValue.endsWith("H")) {
            // Hex number. For example, change 0ffh or ffh to an integer.
            // Other samples: 0ffh, 0eh
            int si = 0;
            if (sValue.startsWith("0") && sValue.length() > 3) {
                si = 1;
            }
            returnString = sValue.substring(si, sValue.length() - 1);     // Hex string to integer. Remove the "h".
            try {
                returnString = Integer.toString(Integer.parseInt(returnString, 16));
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("");
                System.out.println("- Error, invalid hex value: " + sValue + ":" + returnString + ", " + e.getMessage());
                System.out.println("");
            }
        } else if (sValue.endsWith("o")) {
            // Octal number. For example, change 012o or 12o to an integer.
            int si = 0;
            // if (sValue.startsWith("0") && sValue.length() > 3) {
            //     si = 1;
            // }
            returnString = sValue.substring(si, sValue.length() - 1);     // Hex string to integer. Remove the "h".
            try {
                returnString = Integer.toString(Integer.parseInt(returnString, 8));
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("");
                System.out.println("- Error, invalid octal value: " + sValue + ":" + returnString + ", " + e.getMessage());
                System.out.println("");
            }
        } else if (sValue.endsWith("b")) {
            // Binary 
            returnString = sValue.substring(0, sValue.length() - 1);     // Remove the "b".
            try {
                returnString = Integer.toString(Integer.parseInt(returnString, 2));
            } catch (NumberFormatException e) {
                errorCount++;
                System.out.println("");
                System.out.println("- Error, invalid binary value: " + sValue + ":" + returnString + ", " + e.getMessage());
                System.out.println("");
            }
        } else {
            // --------------
            // Since it's not a label, check if it's a valid integer.
            printlnDebug("+ Not found: " + sValue + ".");
            try {
                Integer.parseInt(sValue);   // If not a valid integer, this will fail.
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
    private String getVariableValue(String findName) {
        // Return either a numeric value as a string,
        //  or NAME_NOT_FOUND_STR.
        printlnDebug("\n+ getVariableValue, findName: " + findName);
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
        printlnDebug("+ getVariableValue, returnString: " + returnString);
        return returnString;
    }

    private void setProgramByteImmediates() {
        System.out.println("\n+ Set program immediate values...");
        int i = 0;
        for (Iterator<String> it = programBytes.iterator(); it.hasNext();) {
            String theSource = it.next();
            if (theSource.startsWith("immediate" + SEPARATOR)) {
                String theValue = theSource.substring("immediate:".length());
                String nameValue = getVariableValue(theValue);
                printlnDebug("+ getVariableValue returned, theValue=" + theValue + " nameValue=" + nameValue);
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
        if (theValue.endsWith("h") || theValue.endsWith("H")) {
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

    private void parseDbValue(String theLabel, String theValue) {
        if (!theValue.startsWith("'")) {
            String theVarValue = getVariableValue(theValue);
            if (theVarValue.equals(NAME_NOT_FOUND_STR)) {
                theVarValue = convertValueToInt(theValue);
            }
            System.out.println("++ DB variable name: '" + theLabel
                    + "', single byte with a value of: " + theValue
                    + " = " + theVarValue
                    + "."
            );
            programBytes.add("dbterm:" + theLabel + SEPARATOR + theVarValue);
            programTop++;
            return;
        }
        System.out.println("++ DB, string of bytes: " + theValue);
        for (int i = 1; i < theValue.length() - 1; i++) {
            // Only use what is contained within the quotes, 'Hello' -> Hello
            if (theValue.substring(i, i + 1).equals(SEPARATOR)) {
                programBytes.add("databyte:" + theLabel + SEPARATOR + SEPARATOR_TEMP);
            } else if (theValue.charAt(i) == '\\') {
                // theValue.substring(i, i + 1).equals("\\")
                // Handle escape characters such as '\n'.
                // String sValue = "'" + theValue.substring(i, i + 2) + "'";
                String sValue = theValue.substring(i, i + 2);
                i++;    // Increment because of processing 2 characters instead of one.
                // sValue = convertValueToInt("'" + sValue + "'");
                System.out.println("++ parseDb sValue: " + sValue);
                // '\n' -> ++ parseDb sValue: 10
                // programBytes.add("databyte:" + theName + SEPARATOR + Integer.parseInt(sValue));
                // "39" should be "10".
                // ++       7:00000111: 00100111 : 27 > databyte: '10' : 39
                programBytes.add("databyte:" + theLabel + SEPARATOR + "'" + sValue + "'");
            } else {
                programBytes.add("databyte:" + theLabel + SEPARATOR + theValue.substring(i, i + 1));
            }
            programTop++;
        }
        if (DB_STRING_TERMINATOR != 0) {
            programBytes.add("dbterm:" + theLabel + SEPARATOR + DB_STRING_TERMINATOR);
            programTop++;
        }
    }

    private void parseDb(String theLabel, String theValue) {
        // Add an address label to the DB value. For example,
        //              db  6
        //      six     db  6
        //      six:    DB  6
        labelName.add(theLabel);
        labelAddress.add(programTop);        // Address to the string of bytes.
        parseDbValue(theLabel, theValue);
    }

    // -----------
    // EQU can be an address value or an immediate value.
    private void parseEqu(String theName, String theValue) {
        int intValue = Integer.parseInt(convertValueToInt(theValue));
        // Name-value pair
        variableName.add(theName);
        variableValue.add(intValue);
        System.out.println("++ parseEqu, Variable Name: " + theName + ", Value: " + intValue);
    }

    /* Likely not required.
    private void parseEquNV(String theName, String theValue) {
        int intValue = Integer.parseInt(convertValueToInt(theValue));
        // Address label value pair.
        labelName.add(theName);
        labelAddress.add(intValue);
        System.out.println("++ parseEqu, Label Name: " + theName + ", Value: " + intValue);
        // Name value pair
        variableName.add(theName);
        variableValue.add(intValue);
        System.out.println("++ parseEqu, Variable Name: " + theName + ", Value: " + intValue);
    }
     */
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Parse opcodes into program bytes.
    //
    private String getOpcodeBinary(String opcode) {
        opcodeBinary = theOpcodes.getOpcode(opcode);
        if (opcodeBinary == theOpcodes.OpcodeNotFound) {
            errorCount++;
            System.out.println("\n-- Error1, programTop: " + programTop + ", invalid opode: " + opcode + "\n");
            opcode = "INVALID: " + opcode;
            return ("INVALID: " + opcode);
        }
        return (byteToString(opcodeBinary));
    }

    // -----------------------------------------
    private void parseOpcode(String opcode) {
        // opcode (no parameters).
        switch (opcode) {
            // ------------------------------------------
            // opcode, example: nop
            case "hlt":
            case "nop":
            case "ret":
            case "rlc":
            case "rrc":
            case "ei":
            case "di":
            case "rar":
            case "rz":
            case "rnz":
            case "rc":
            case "rnc":
            case "rpo":
            case "rpe":
            case "rp":
            case "rm":
            case "xthl":
            case "sphl":
            case "xchg":
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary);
                programTop++;
                System.out.println("++ Opcode: " + opcode + " " + sOpcodeBinary);
                break;
            // -----------------------------
            default:
                System.out.print("-- Error2, programTop: " + programTop + " ");
                System.out.println("-- INVALID, Opcode: " + opcode + " " + sOpcodeBinary);
                errorCount++;
                break;
        }
    }

    // -------------------------------------------------
    private void parseOpcode(String opcode, String p1) {
        // Opcode, single parameter
        switch (opcode) {
            // ------------------------------------------
            // opcode <address label>, example: jmp There
            case "call":
            case "jmp":
            case "jnz":
            case "jz":
            case "jnc":
            case "jc":
            case "lda":
            case "sta":
            case "lhld":
            case "shld":
            case "jm":
            case "jp":
            case "jpe":
            case "jpo":
            case "cnz":
            case "cz":
            case "cnc":
            case "cc":
            case "cpo":
            case "cpe":
            case "cp":
            case "cm":
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                programBytes.add("lb:" + p1);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + "  + label|" + p1 + "|");
                break;
            // ------------------------------------------
            // opcode <immediate>, example: out 39
            case "adi":
            case "ani":
            case "cpi":
            case "in":
            case "out":
            case "sui":
            case "aci":
            case "sbi":
            case "xri":
            case "ori":
            case "pchl":
            case "daa":
            case "ral":
            case "cma":
            case "stc":
            case "cmc":
            case "sphl":
            case "rst":
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                programBytes.add("immediate:" + p1);
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + "  + immediate|" + p1 + "|");
                break;
            // ------------------------------------------
            // opcode <register|RegisterPair>, example: cmp c
            case "cmp":
            case "dad":
            case "dcr":
            case "inr":
            case "inx":
            case "dcx":
            case "ldax":
            case "ora":
            case "pop":
            case "push":
            case "xra":
            case "add":
            case "adc":
            case "sub":
            case "sbb":
            case "ana":
            case "stax":
                p1 = p1.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1);
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + "  + Register or RegisterPair|" + p1 + "|");
                break;
            // ------------------------------------------
            default:
                System.out.print("-- Error3, programTop: " + programTop + " ");
                System.out.println("-- INVALID, Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + "  + p1|" + p1 + "|");
                errorCount++;
                break;
        }
    }

    private void parseOpcode(String opcode, String p1, String p2) {
        // Opcode with 2 parameters, example: mvi a,1
        switch (opcode) {
            case "mvi":
                // opcode <register>,<immediate>, example: mvi a,1
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
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + " register|" + p1 + "|" + " immediate|" + p2 + "|");
                break;
            case "mov":
                // opcode <register>,<register>, example: mov a,b
                p1 = p1.toLowerCase();
                p2 = p2.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1 + p2);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1 + SEPARATOR + p2);
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + " register|" + p1 + "|" + " register|" + p2 + "|");
                break;
            case "lxi":
                // opcode <register>,<address label|address number>, example: lxi b,5
                p1 = p1.toLowerCase();
                sOpcodeBinary = getOpcodeBinary(opcode + p1);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary + SEPARATOR + p1 + SEPARATOR + p2);
                programTop++;
                programBytes.add("lb:" + p2);
                programTop++;
                programBytes.add("hb:" + 0);
                programTop++;
                System.out.println("++ Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + " register|" + p1 + "|" + " addressLabel|addressNumber|" + p2 + "|");
                break;
            default:
                System.out.print("-- Error4, programTop: " + programTop + " ");
                System.out.println("-- INVALID, Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + " p1|" + p1 + "|" + " p2|" + p2 + "|");
                errorCount++;
                break;
        }
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

        theRest = orgLine.replaceAll("\t", " ");        // Convert tab to space.

        if (ignoreFirstCharacters > 0 && theRest.length() > ignoreFirstCharacters) {
            // This allows parsing files where the address is included in the listing.
            theLine = theRest.substring(ignoreFirstCharacters, theRest.length()).trim();
        } else {
            theLine = theRest.trim();
        }
        if (theLine.length() == 0) {
            // Ignore blank lines.
            return;
        }
        if (theLine.startsWith(";")) {
            // Ignore comment lines.
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
        System.out.println("\n+ Parse |" + theLine + "|");

        // ---------------------------------------------------------------------
        // Single component.
        c1 = theLine.indexOf(" ");
        if (c1 < 1) {
            String cOne = theLine.toLowerCase();
            // -----------------------------------------------------------------
            if (theLine.endsWith(":")) {
                label = theLine.substring(0, theLine.length() - 1);
                System.out.println("++ parseLine, label line|" + cOne + "|");
                // Example:     "Start:"
                parseLabel(label);
                return;
            }
            if (cOne.equals("end")) {
                // Special case: end file processing
                opcode = "end"; // Tells the calling function to end processing.
                return;
            }
            System.out.println("++ parseLine, no parameters, opcode|" + cOne + "|");
            // Example:     hlt
            parseOpcode(cOne);
            return;
        }
        // ---------------------------------------------------------------------
        // Multi component line parsing into 2 or 3 component parts.
        //
        // c1 = index of blank after first component.
        String part1asIs = theLine.substring(0, c1);
        String part1 = part1asIs.toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        //
        // + Parse |IOINI: MVI A,3|
        // ++ parseLine componets part1asIs|IOINI:| part1|ioini:| part2|MVI| theDirective|mvi| theRest|A,3|
        theRest = theRest.replaceAll("' '", "32");
        //
        // SUI ':'
        theRest = theRest.replace("'" + SEPARATOR + "'", "'" + SEPARATOR_TEMP + "'");
        //
        // --------------------
        // Next to add:
        // + Parse |DB CR,LF,' ',' ','1'|
        // ++ parseLine componets part1asIs|DB| part1|db| part2|CR,LF,32,32,'1'| theDirective|| theRest||
        System.out.println("++ parseLine componets theRest|" + theRest + "|");
        //
        // Parse |DB ' STARDATE  300'|
        // theRest = ' STARDATE  300';
        //
        int c2 = theRest.indexOf(" ");
        String part2;
        String theDirective = "";
        if (c2 < 1 || theRest.indexOf("'") == 0) {
            // No blanks for theRest is a string that starts with "'".
            part2 = theRest;
            theRest = "";
        } else {
            part2 = theRest.substring(0, c2).trim();
            theDirective = part2.toLowerCase();
            theRest = theRest.substring(c2 + 1).trim();
        }
        System.out.println("++ parseLine componets part1asIs|" + part1asIs
                + "| part1|" + part1
                + "| part2|" + part2
                + "| theDirective|" + theDirective
                + "| theRest|" + theRest + "|");
        //
        // ---------------------------------------------------------------------
        // Multi component line parsing order.
        //
        //      part1           part2                       theRest
        //      DB              <number>|'<string>'
        //      DS              <number>
        //      <label>[:]      DB                          <number>
        //      <label>[:]      DB                          '<string>'
        //      <label>[:]      DS                          <number>
        //      <label>[:]      EQU                         <$|number>
        //      org             <number>
        //      <label>:        <opcode>
        //      <label>:        <opcode>                    <parameters>
        //      <opcode>        <parameter>
        //      <opcode>        <parameter>,<parameter>
        //
        if (part1.equals("db")) {
            // dave
            // + Parse |DB CR,LF,' ',' ','1'|
            // ++ parseLine componets part1asIs|DB| part1|db| part2|CR,LF,32,32,'1'| theDirective|| theRest||
            // If more than 3 "'", them multiple bytes.
            int cq = part2.indexOf("'");
            int cc = part2.indexOf(",");
            if (cq < 1 && cc < 1) {
                // Example, an immediate number or label:
                //      DB  6
                //      DB  CR
                parseDbValue("", part2);        // No label.
                return;
            }
            if (cq == 0) {
                cq = part2.indexOf("'", 1);
                if (cq == part2.length()) {
                    // Example string of bytes from Galaxy80.asm:
                    //      DB  ' STARDATE  300'
                    parseDbValue("", part2);        // No label.
                }
                return;
            }
            // Example from Galaxy80.asm, and another example:
            //      DB CR,LF,' ',' ','1'
            int cn = 0;
            String theByte = part2.substring(cn, cc);
            System.out.println("++ parseLine2, DB bytes, theByte|" + theByte + "|");
            parseDbValue("", theByte);
            while (cc < part2.length()) {
                cn = cc + 1;
                cc = part2.indexOf(",", cc + 1);
                if (cc < 1) {
                    cc = part2.length();
                }
                theByte = part2.substring(cn, cc);
                System.out.println("++ parseLine2, DB bytes, theByte|" + theByte + "|");
                parseDbValue("", theByte);
            }
            System.out.println("++ parseLine, DB multiple comma separated bytes are parsed.");
            return;
        }
        if (part1.equals("ds")) {
            //      DS  2
            parseDs("", part2);             // No label.
            return;
        }
        if (!theRest.equals("")) {
            // Statement has a label, example:
            // thePrompt   db      '> '
            //
            // Set the label name.
            String theLabel = part1asIs;
            if (part1asIs.endsWith(":")) {
                // Remove the ":".
                theLabel = part1asIs.substring(0, part1asIs.length() - 1);
            }
            if (theDirective.equals("equ")) {
                // EQU variable names and values, can either be an immediate byte, or a 2 byte address.
                // So, add both, an address label and a immediate name-value pair.
                //      TERMB:  equ     0ffh
                //      stack:  equ     $
                if (theRest.equals("$")) {
                    //  stack   equ $    ; "$" is current address.
                    theRest = Integer.toString(programTop);
                }
                System.out.println("++ parseLine, EQU directive, theLabel|" + theLabel + "| theValue|" + theRest + "|");
                parseEqu(theLabel, theRest);
                return;
            }
            if (theDirective.equals("db")) {
                // + Parse |MSGYJD: DB CR,LF|
                // ++ parseLine componets part1asIs|MSGYJD:| part1|msgyjd:| part2|DB| theRest|CR,LF|
                //
                // String of bytes.
                //      MSGSDP: DB  '0'
                //      Hello:  db  'Hello there'
                //
                // + Parse |thePrompt   db      '> '|
                // ++ parseLine componets theRest|db      '> '|
                // ++ parseLine componets part1asIs|thePrompt| part1|theprompt| part2|db| theDirective|db| theRest|'> '|
                // ++ parseLabel, Name: thePrompt, Address: 338
                //
                System.out.println("++ parseLine, DB directive, theLabel|" + theLabel + "| theValue|" + theRest + "| programTop=" + programTop);
                parseDb(theLabel, theRest);
                return;
            }
            if (theDirective.equals("ds")) {
                // Assembler directives:
                //      Data0:  ds  3
                System.out.println("++ parseLine, ds directive without a label name, theRest|" + theRest);
                parseDs(theLabel, theRest);
                return;
            }
            // Else, labeled opcode statement.
            //      <label>:        <opcode>                    <parameters>
            // The label was added above, now alter the components for use in the following opcode processing.
            //
            // + Parse |IOST:   IN SIOCTL|
            // ++ parseLine componets part1asIs|IOST:| part1|iost:| part2|IN| theDirective|in| theRest|SIOCTL|
            //
            parseLabel(theLabel);
            //
            part1 = part2;
            part2 = theRest;
            theRest = "";
        }
        // ---------------------------------------------------------------------
        System.out.println("++ parseLine1 2 componets, part1|" + part1 + "| part2|" + part2 + "|");
        //
        //      org             <number>
        if (part1.equals("org")) {
            parseOrg(part2);
            return;
        }
        //      part1           part2
        //      <label>:        <opcode>
        if (part1.endsWith(":")) {
            // Remove the ":", and add the label.
            String theLabel = part1.substring(0, part1.length() - 1);
            parseLabel(theLabel);
            if (theRest.equals("")) {
                parseOpcode(part2);
                return;
            }
            parseOpcode(opcode);
            return;
        }
        // ++ parseLine1 2 componets, part1|IN| part2|SIOCTL|
        //      <opcode>        <parameter>
        //      <opcode>        <parameter>,<parameter>
        opcode = part1.toLowerCase();
        c1 = part2.indexOf(",");
        if (c1 > 0) {
            p1 = part2.substring(0, c1);
            p2 = part2.substring(c1 + 1);
            System.out.println("++ parseLine, opcode|" + opcode + "| p1|" + p1 + "| p2|" + p2 + "|");
            parseOpcode(opcode, p1, p2);
            return;
        }
        System.out.println("++ parseLine, opcode|" + opcode + "| part2|" + part2 + "|");
        parseOpcode(opcode, part2);
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
        for (int i = 0; i < theLength; i++) {
            System.out.print(byteToString(bArray[i]) + " ");
            System.out.println("");
        }
        System.out.println("\n+ Binary display completed.");
    }

    public void showFile(String theReadFilename) {
        System.out.println("++ Show binary file: " + theReadFilename);
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
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/programList.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pstatuslights.asm");
        //
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pSyntax.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pGalaxy80.asm");
        thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pcli.asm");
        //
        // Option: for debugging:
        thisProcess.listLabelAddresses();
        // thisProcess.listImmediateValues();
        thisProcess.programBytesListAndWrite("");

        // thisProcess.showFileBytes("p1.bin");
        // thisProcess.showFileBytes("pGalaxyBytesOrg.bin");
        // 
        // thisProcess.programBytesListCode();
        // thisProcess.programBytesListHex();
        //
        // Required, sets actual values:
        //
        // Option: create a binary file of the program, which has a nice listing.
        // thisProcess.programBytesListAndWrite("10000000.bin");
        // thisProcess.showFile("10000000.bin");
        // thisProcess.showFile("pG.asm");
        //
        if (thisProcess.errorCount > 0) {
            System.out.println("\n-- Number of errors: " + thisProcess.errorCount + "\n");
        }

        System.out.println("++ Exit.");
    }
}
