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

    + List line for: -- 412: - Error, immediate label not found: c.
    + Line that I should list: mov a,c

    + Test if label and immediate names case sensitive.

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
    // Types of components and parameter:
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
    private String theLine;
    private String opcode = "";
    private byte opcodeBinary;
    private String sOpcodeBinary;
    private String p1;
    private String p2;
    private final String SEPARATOR = ":";
    private final String SEPARATOR_TEMP = "^^";
    private final String DB_STRING_TERMINATOR = "0";   // To have it automatically added, set to: 255 = ffh = B11111111
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
    private boolean debugMessage = false;    // false true

    // Use for storing error messages which are printed at the end, in a summary.
    private int errorCount = 0;

    public int getErrorCount() {
        return this.errorCount;
    }
    private final static List<Integer> errorLineNum = new ArrayList<>();
    private final static List<String> errorLines = new ArrayList<>();
    private final static List<String> errorMsgs = new ArrayList<>();

    private void printlnErrorMsg(int theProgramLine, String theMessage) {
        errorCount++;
        System.out.println("-- " + theProgramLine + ": " + theMessage);
        errorLineNum.add(theProgramLine);
        errorLines.add(theLine);
        errorMsgs.add(theMessage);
    }

    public void listErrorMsgs() {
        System.out.println("\n- Number of errors = " + errorCount);
        System.out.println("- List Error Messages:");
        Iterator<Integer> lErrorLineNums = errorLineNum.iterator();
        Iterator<String> lErrorLines = errorLines.iterator();
        Iterator<String> lErrorMsgs = errorMsgs.iterator();
        while (lErrorLines.hasNext()) {
            int theErrorLineNum = lErrorLineNums.next();
            String theErrorLine = lErrorLines.next();
            String theErrorMsg = lErrorMsgs.next();
            System.out.println("-- " + theErrorLineNum + ": " + theErrorLine + "\n-- " + theErrorMsg);
        }
        System.out.println("+ End of list.");
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
                    // System.out.println("++ " + theValue);
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
                case "dbbyte":
                    // dbbyte:def:255
                    System.out.print(byteToString((byte) Integer.parseInt(opcodeValues[2])) + " : ");
                    System.out.print(String.format("%02X", Integer.parseInt(opcodeValues[2])));
                    System.out.print(String.format(":%03o", Integer.parseInt(opcodeValues[2])));    // Octal
                    System.out.println(" > dbbyte: " + opcodeValues[2]);
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
                        printlnErrorMsg(programTop, "\n- getVariableValue, Error, programTop: " + programTop + ", unhandled escape character: " + sValue + ".");
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
                printlnErrorMsg(programTop, "- Error, invalid hex value: " + sValue + ":" + returnString + ", " + e.getMessage());
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
                printlnErrorMsg(programTop, "- Error, invalid octal value: " + sValue + ":" + returnString + ", " + e.getMessage());
            }
        } else if (sValue.endsWith("b")) {
            // Binary 
            returnString = sValue.substring(0, sValue.length() - 1);     // Remove the "b".
            try {
                returnString = Integer.toString(Integer.parseInt(returnString, 2));
            } catch (NumberFormatException e) {
                printlnErrorMsg(programTop, "- Error, invalid binary value: " + sValue + ":" + returnString + ", " + e.getMessage());
            }
        } else {
            // --------------
            // Since it's not a label, check if it's a valid integer.
            printlnDebug("+ Not found: " + sValue + ".");
            try {
                Integer.parseInt(sValue);   // If not a valid integer, this will fail.
                returnString = sValue;
            } catch (NumberFormatException e) {
                printlnErrorMsg(programTop, "- Error, immediate label not found: " + sValue + ".");
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
            programBytes.add("dsname:org" + SEPARATOR + "255");  // Default value is 255 (11111111), based on zasm sample.
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

    private void parseDbAdd(String theLabel, String theValue) {
        System.out.println("++ parseDbAdd theLabel:" + theLabel + ": theValue:" + theValue + ":");
        programBytes.add("databyte:" + theLabel + SEPARATOR + theValue);
        programTop++;
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
            // + Parse |db      0|
            programBytes.add("dbbyte:" + theLabel + SEPARATOR + theVarValue);
            programTop++;
            return;
        }
        System.out.println("++ DB, string of bytes: " + theValue);
        for (int i = 1; i < theValue.length() - 1; i++) {
            // Only use what is contained within the quotes, 'Hello' -> Hello
            if (theValue.substring(i, i + 1).equals(SEPARATOR)) {
                parseDbAdd(theLabel, SEPARATOR_TEMP);
            } else if (theValue.charAt(i) == '\\') {
                // Handle escape characters such as '\n'.
                String sValue = "'" + theValue.substring(i, i + 2) + "'";
                i++;    // Increment because of processing 2 characters instead of one.
                parseDbAdd(theLabel, sValue);
            } else {
                parseDbAdd(theLabel, theValue.substring(i, i + 1));
            }
        }
        if (!DB_STRING_TERMINATOR.equals("0")) {
            parseDbAdd(theLabel, DB_STRING_TERMINATOR);
        }
    }

    private void parseDb(String theLabel, String theValue) {
        System.out.println("++ parseDb( theLabel: " + theLabel + ", theValue: " + theValue + " )");
        labelName.add(theLabel);
        labelAddress.add(programTop);        // Address to the string of bytes.
        // parseDbValue(theLabel, theValue);

        // dave, should work for both "db" cases: with or without a label.
        // + Parse |DB CR,LF,' ',' ','1'|
        // ++ parseLine components part1asIs|DB| part1|db| part2|CR,LF,32,32,'1'| theDirective|| theRest||
        // If more than 3 "'", them multiple bytes.
        int cq = theValue.indexOf("'");     // index of the first single quote "'".
        int cc = theValue.indexOf(",");
        if (cq < 1 && cc < 1) {
            // No single quote and no comma.
            // Example, an immediate number or label:
            //      DB  6
            //      DB  CR
            parseDbValue("", theValue);        // No label.
            return;
        }
        if (cq == 0) {
            cq = theValue.indexOf("'", 1);     // index of the second single quote "'".
            if (cq == theValue.length()) {
                // Example string of bytes from Galaxy80.asm:
                //      DB  ' STARDATE  300'
                parseDbValue("", theValue);        // No label.
            }
            return;
        }
        // Example from Galaxy80.asm, and another example:
        //      DB CR,LF,' ',' ','1'
        int cn = 0;
        String theByte = theValue.substring(cn, cc);
        System.out.println("++ parseLine2, DB bytes, theByte|" + theByte + "|");
        parseDbValue("", theByte);
        while (cc < theValue.length()) {
            cn = cc + 1;
            cc = theValue.indexOf(",", cc + 1);
            if (cc < 1) {
                cc = theValue.length();
            }
            theByte = theValue.substring(cn, cc);
            System.out.println("++ parseLine2, DB bytes, theByte|" + theByte + "|");
            parseDbValue("", theByte);
        }
        System.out.println("++ parseLine, DB multiple comma separated bytes are parsed.");
    }

    // -----------
    // EQU can be an address value or an immediate value.
    private void parseEqu(String theName, String theValue) {
        int intValue = Integer.parseInt(convertValueToInt(theValue));
        // Name-value pair
        variableName.add(theName);
        variableValue.add(intValue);
        //
        // Example, address to stack of bytes:
        // ...
        //          LXI SP,STACK    ;Set stack pointer
        // ...
        //          DS  32          ;Stack Area
        // STACK:   EQU	$
        labelName.add(theName);
        labelAddress.add(programTop - 1);
        //
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
            printlnErrorMsg(programTop, "\n-- Error1, invalid opode: " + opcode + "\n");
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
            // opcode, no parameters
            //  Example: nop
            case "hlt":
            case "nop":
            case "ret":
            case "rlc":
            case "rrc":
            case "ei":
            case "di":
            case "cma":
            case "rar":
            case "ral":
            case "rz":
            case "rnz":
            case "rc":
            case "rnc":
            case "rpo":
            case "rpe":
            case "rp":
            case "rm":
            case "xthl":
            case "xchg":
            case "pchl":
            case "sphl":
            case "rim":
            case "sim":
            case "daa":
            case "cmc":
                sOpcodeBinary = getOpcodeBinary(opcode);
                programBytes.add("opcode:" + opcode + SEPARATOR + sOpcodeBinary);
                programTop++;
                System.out.println("++ Opcode: " + opcode + " " + sOpcodeBinary);
                break;
            // -----------------------------
            default:
                printlnErrorMsg(programTop, "-- Error2, INVALID, Opcode: " + opcode + " " + sOpcodeBinary);
                break;
        }
    }

    // -------------------------------------------------
    private void parseOpcode(String opcode, String p1) {
        // Opcode, single parameter
        switch (opcode) {
            // ------------------------------------------
            // opcode <address label>
            //  Example: jmp There
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
            case "stc":
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
            // opcode <immediate>
            //  Example: out 39
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
            // opcode <register|RegisterPair>
            // Example: cmp c
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
                printlnErrorMsg(programTop, "-- Error3, INVALID, Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + "  + p1|" + p1 + "|");
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
                printlnErrorMsg(programTop, "-- Error4, INVALID, Opcode: "
                        + opcode + " " + sOpcodeBinary
                        + " p1|" + p1 + "|" + " p2|" + p2 + "|"
                );
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
        // ++ parseLine components part1asIs|IOINI:| part1|ioini:| part2|MVI| theDirective|mvi| theRest|A,3|
        theRest = theRest.replaceAll("' '", "32");
        //
        // SUI ':'
        theRest = theRest.replace("'" + SEPARATOR + "'", "'" + SEPARATOR_TEMP + "'");
        //
        // --------------------
        // Next to add:
        // + Parse |DB CR,LF,' ',' ','1'|
        // ++ parseLine components part1asIs|DB| part1|db| part2|CR,LF,32,32,'1'| theDirective|| theRest||
        System.out.println("++ parseLine components theRest|" + theRest + "|");
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
        System.out.println("++ parseLine components part1asIs|" + part1asIs
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
            parseDb("", part2);
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
                    theRest = Integer.toString(programTop - 1);
                }
                System.out.println("++ parseLine, EQU directive, theLabel|" + theLabel + "| theValue|" + theRest + "|");
                parseEqu(theLabel, theRest);
                return;
            }
            if (theDirective.equals("db")) {
                // System.out.println("++ parseLine, DB directive, theLabel|" + theLabel + "| theValue|" + theRest + "| programTop=" + programTop);
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
            // ++ parseLine components part1asIs|IOST:| part1|iost:| part2|IN| theDirective|in| theRest|SIOCTL|
            //
            parseLabel(theLabel);
            //
            part1 = part2;
            part2 = theRest;
            theRest = "";
        }
        // ---------------------------------------------------------------------
        System.out.println("++ parseLine1 2 components, part1|" + part1 + "| part2|" + part2 + "|");
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
        // ++ parseLine1 2 components, part1|IN| part2|SIOCTL|
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
            theLine = pin.readLine();
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
            System.out.println("\n-- parseFile, Number of errors1: " + errorCount);
            return;
        }
        setProgramByteAddresses();
        if (errorCount > 0) {
            System.out.println("\n-- parseFile, Number of errors2: " + errorCount);
            return;
        }
        setProgramByteImmediates();
        if (errorCount > 0) {
            System.out.println("\n-- parseFile, Number of errors3: " + errorCount);
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
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pcli.asm");
        //
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pSyntax.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pGa.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pGa2.asm");
        // thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/pGalaxy80.asm");
        //
        thisProcess.parseFile("/Users/dthurston/Projects/arduino/Altair101/asm/programs/altairAdd.asm");
        //
        if (thisProcess.errorCount > 0) {
            thisProcess.listErrorMsgs();
            return;
        }
        //
        // Option: for debugging:
        thisProcess.listLabelAddresses();
        thisProcess.listImmediateValues();
        thisProcess.programBytesListAndWrite("p1.bin");

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
            thisProcess.listErrorMsgs();
        }

        System.out.println("++ Exit.");
    }
}

/* Confirmed: byte value to nemonic.
    And visual of parameter correctness.

++ 00 000 00000000 NOP  nop     NA  :NOP:
++ 01 001 00000001 LXI  lxib        :LXI B,D16  B <- byte 3, C <- byte 2:
++ 02 002 00000010 STAX staxb   R   :STAX B     (BC) <- A:
++ 03 003 00000011 INX  inxb    R   :INX B      BC <- BC+1:
++ 04 004 00000100 INR  inrb    R   :INR B      B <- B+1:
++ 05 005 00000101 DCR  dcrb    R   :DCR B      B <- B-1:
++ 06 006 00000110 MVI  mvib        :MVI B, D8  B <- byte 2:
++ 07 007 00000111 RLC  rlc     NA  :RLC        A = A << 1; bit 0 = prev bit 7; CY = prev bit 7:
++ 08 ---
++ 09 009 00001001 DAD  dadb    R   :DAD B      HL = HL + BC:
++ 0a 010 00001010 LDAX ldaxb   R   :LDAX B     A <- (BC):
++ 0b 011 00001011 DCX  dcxb    R   :DCX B      BC = BC-1:
++ 0c 012 00001100 INR  inrc    R   :INR C      C <- C+1:
++ 0d 013 00001101 DCR  dcrc    R   :DCR C      C <-C-1:
++ 0e 014 00001110 MVI  mvic        :MVI C,D8   C <- byte 2:
++ 0f 015 00001111 RRC  rrc     NA  :RRC        A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0:
++ 10 ---
++ 11 017 00010001 LXI  lxid        :LXI D,D16  D <- byte 3, E <- byte 2:
++ 12 018 00010010 STAX staxd   R   :STAX D     (DE) <- A:
++ 13 019 00010011 INX  inxd    R   :INX D      DE <- DE + 1:
++ 14 020 00010100 INR  inrd    R   :INR D      D <- D+1:
++ 15 021 00010101 DCR  dcrd    R   :DCR D      D <- D-1:
++ 16 022 00010110 MVI  mvid        :MVI D, D8  D <- byte 2:
++ 17 023 00010111 RAL  ral     NA  :RAL        A = A << 1; bit 0 = prev CY; CY = prev bit 7:
++ 18 ---
++ 19 025 00011001 DAD  dadd    R   :DAD D      HL = HL + DE:
++ 1a 026 00011010 LDAX ldaxd   R   :LDAX D     A <- (DE):
++ 1b 027 00011011 DCX  dcxd    R   :DCX D      DE = DE-1:
++ 1c 028 00011100 INR  inre    R   :INR E      E <-E+1:
++ 1d 029 00011101 DCR  dcre    R   :DCR E      E <- E-1:
++ 1e 030 00011110 MVI  mvie        :MVI E,D8   E <- byte 2:
++ 1f 031 00011111 RAR  rar     NA  :RAR        A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0:
++ 20 032 00100000 RIM  rim     NA  :RIM        special:
++ 21 033 00100001 LXI  lxih        :LXI H,D16  H <- byte 3, L <- byte 2:
++ 22 034 00100010 SHLD shld    adr :SHLD adr   (adr) <-L; (adr+1)<-H:
++ 23 035 00100011 INX  inxh    R   :INX H      HL <- HL + 1:
++ 24 036 00100100 INR  inrh    R   :INR H      H <- H+1:
++ 25 037 00100101 DCR  dcrh    R   :DCR H      H <- H-1:
++ 26 038 00100110 MVI  mvih        :MVI H,D8   H <- byte 2:
++ 27 039 00100111 DAA  daa     NA  :DAA        special:
++ 28 ---
++ 29 041 00101001 DAD  dadh    R   :DAD H      HL = HL + HI:
++ 2a 042 00101010 LHLD lhld    adr :LHLD adr   L <- (adr); H<-(adr+1):
++ 2b 043 00101011 DCX  dcxh    R   :DCX H      HL = HL-1:
++ 2c 044 00101100 INR  inrl    R   :INR L      L <- L+1:
++ 2d 045 00101101 DCR  dcrl    R   :DCR L      L <- L-1:
++ 2e 046 00101110 MVI  mvil        :MVI L, D8  L <- byte 2:
++ 2f 047 00101111 CMA  cma     NA  :CMA        A <- !A:
++ 30 048 00110000 SIM  sim     NA  :SIM        special:
++ 31 049 00110001 LXI  lxisp       :LXI SP,D16 SP.hi <- byte 3, SP.lo <- byte 2:
++ 32 050 00110010 STA  sta     adr :STA adr    (adr) <- A:
++ 33 051 00110011 INX  inxsp   R   :INX SP     SP = SP + 1:
++ 34 052 00110100 INR  inrm    R   :INR M      (HL) <- (HL)+1:
++ 35 053 00110101 DCR  dcrm    R   :DCR M      (HL) <- (HL)-1:
++ 36 054 00110110 MVI  mvim        :MVI M,D8   (HL) <- byte 2:
++ 37 055 00110111 STC  stc     adr :STC        CY = 1:
++ 38 ---
++ 39 057 00111001 DAD  dadsp   R   :DAD SP     HL = HL + SP:
++ 3a 058 00111010 LDA  lda     adr :LDA adr    A <- (adr):
++ 3b 059 00111011 DCX  dcxsp   R   :DCX SP     SP = SP-1:
++ 3c 060 00111100 INR  inra    R   :INR A      A <- A+1:
++ 3d 061 00111101 DCR  dcra    R   :DCR A      A <- A-1:
++ 3e 062 00111110 MVI  mvia        :MVI A,D8   A <- byte 2:
++ 3f 063 00111111 CMC  cmc     NA  :CMC        CY=!CY:
++ 40 064 01000000 MOV  movbb       :MOV B,B    B <- B:
++ 41 065 01000001 MOV  movbc       :MOV B,C    B <- C:
++ 42 066 01000010 MOV  movbd       :MOV B,D    B <- D:
++ 43 067 01000011 MOV  movbe       :MOV B,E    B <- E:
++ 44 068 01000100 MOV  movbh       :MOV B,H    B <- H:
++ 45 069 01000101 MOV  movbl       :MOV B,L    B <- L:
++ 46 070 01000110 MOV  movbm       :MOV B,M    B <- (HL):
++ 47 071 01000111 MOV  movba       :MOV B,A    B <- A:
++ 48 072 01001000 MOV  movcb       :MOV C,B    C <- B:
++ 49 073 01001001 MOV  movcc       :MOV C,C    C <- C:
++ 4a 074 01001010 MOV  movcd       :MOV C,D    C <- D:
++ 4b 075 01001011 MOV  movce       :MOV C,E    C <- E:
++ 4c 076 01001100 MOV  movch       :MOV C,H    C <- H:
++ 4d 077 01001101 MOV  movcl       :MOV C,L    C <- L:
++ 4e 078 01001110 MOV  movcm       :MOV C,M    C <- (HL):
++ 4f 079 01001111 MOV  movca       :MOV C,A    C <- A:
++ 50 080 01010000 MOV  movdb       :MOV D,B    D <- B:
++ 51 081 01010001 MOV  movdc       :MOV D,C    D <- C:
++ 52 082 01010010 MOV  movdd       :MOV D,D    D <- D:
++ 53 083 01010011 MOV  movde       :MOV D,E    D <- E:
++ 54 084 01010100 MOV  movdh       :MOV D,H    D <- H:
++ 55 085 01010101 MOV  movdl       :MOV D,L    D <- L:
++ 56 086 01010110 MOV  movdm       :MOV D,M    D <- (HL):
++ 57 087 01010111 MOV  movda       :MOV D,A    D <- A:
++ 58 088 01011000 MOV  moveb       :MOV E,B    E <- B:
++ 59 089 01011001 MOV  movec       :MOV E,C    E <- C:
++ 5a 090 01011010 MOV  moved       :MOV E,D    E <- D:
++ 5b 091 01011011 MOV  movee       :MOV E,E    E <- E:
++ 5c 092 01011100 MOV  moveh       :MOV E,H    E <- H:
++ 5d 093 01011101 MOV  movel       :MOV E,L    E <- L:
++ 5e 094 01011110 MOV  movem       :MOV E,M    E <- (HL):
++ 5f 095 01011111 MOV  movea       :MOV E,A    E <- A:
++ 60 096 01100000 MOV  movhb       :MOV H,B    H <- B:
++ 61 097 01100001 MOV  movhc       :MOV H,C    H <- C:
++ 62 098 01100010 MOV  movhd       :MOV H,D    H <- D:
++ 63 099 01100011 MOV  movhe       :MOV H,E    H <- E:
++ 64 100 01100100 MOV  movhh       :MOV H,H    H <- H:
++ 65 101 01100101 MOV  movhl       :MOV H,L    H <- L:
++ 66 102 01100110 MOV  movhm       :MOV H,M    H <- (HL):
++ 67 103 01100111 MOV  movha       :MOV H,A    H <- A:
++ 68 104 01101000 MOV  movlb       :MOV L,B    L <- B:
++ 69 105 01101001 MOV  movlc       :MOV L,C    L <- C:
++ 6a 106 01101010 MOV  movld       :MOV L,D    L <- D:
++ 6b 107 01101011 MOV  movle       :MOV L,E    L <- E:
++ 6c 108 01101100 MOV  movlh       :MOV L,H    L <- H:
++ 6d 109 01101101 MOV  movll       :MOV L,L    L <- L:
++ 6e 110 01101110 MOV  movlm       :MOV L,M    L <- (HL):
++ 6f 111 01101111 MOV  movla       :MOV L,A    L <- A:
++ 70 112 01110000 MOV  movmb       :MOV M,B    (HL) <- B:
++ 71 113 01110001 MOV  movmc       :MOV M,C    (HL) <- C:
++ 72 114 01110010 MOV  movmd       :MOV M,D    (HL) <- D:
++ 73 115 01110011 MOV  movme       :MOV M,E    (HL) <- E:
++ 74 116 01110100 MOV  movmh       :MOV M,H    (HL) <- H:
++ 75 117 01110101 MOV  movml       :MOV M,L    (HL) <- L:
++ 76 118 01110110 HLT  hlt     NA  :HLT        special:
++ 77 119 01110111 MOV  movma       :MOV M,A    (HL) <- A:
++ 78 120 01111000 MOV  movab       :MOV A,B    A <- B:
++ 79 121 01111001 MOV  movac       :MOV A,C    A <- C:
++ 7a 122 01111010 MOV  movad       :MOV A,D    A <- D:
++ 7b 123 01111011 MOV  movae       :MOV A,E    A <- E:
++ 7c 124 01111100 MOV  movah       :MOV A,H    A <- H:
++ 7d 125 01111101 MOV  moval       :MOV A,L    A <- L:
++ 7e 126 01111110 MOV  movam       :MOV A,M    A <- (HL):
++ 7f 127 01111111 MOV  movaa       :MOV A,A    A <- A:
++ 80 128 10000000 ADD  addb    R   :ADD B      A <- A + B:
++ 81 129 10000001 ADD  addc    R   :ADD C      A <- A + C:
++ 82 130 10000010 ADD  addd    R   :ADD D      A <- A + D:
++ 83 131 10000011 ADD  adde    R   :ADD E      A <- A + E:
++ 84 132 10000100 ADD  addh    R   :ADD H      A <- A + H:
++ 85 133 10000101 ADD  addl    R   :ADD L      A <- A + L:
++ 86 134 10000110 ADD  addm    R   :ADD M      A <- A + (HL):
++ 87 135 10000111 ADD  adda    R   :ADD A      A <- A + A:
++ 88 136 10001000 ADC  adcb    R   :ADC B      A <- A + B + CY:
++ 89 137 10001001 ADC  adcc    R   :ADC C      A <- A + C + CY:
++ 8a 138 10001010 ADC  adcd    R   :ADC D      A <- A + D + CY:
++ 8b 139 10001011 ADC  adce    R   :ADC E      A <- A + E + CY:
++ 8c 140 10001100 ADC  adch    R   :ADC H      A <- A + H + CY:
++ 8d 141 10001101 ADC  adcl    R   :ADC L      A <- A + L + CY:
++ 8e 142 10001110 ADC  adcm    R   :ADC M      A <- A + (HL) + CY:
++ 8f 143 10001111 ADC  adca    R   :ADC A      A <- A + A + CY:
++ 90 144 10010000 SUB  subb    R   :SUB B      A <- A - B:
++ 91 145 10010001 SUB  subc    R   :SUB C      A <- A - C:
++ 92 146 10010010 SUB  subd    R   :SUB D      A <- A + D:
++ 93 147 10010011 SUB  sube    R   :SUB E      A <- A - E:
++ 94 148 10010100 SUB  subh    R   :SUB H      A <- A + H:
++ 95 149 10010101 SUB  subl    R   :SUB L      A <- A - L:
++ 96 150 10010110 SUB  subm    R   :SUB M      A <- A + (HL):
++ 97 151 10010111 SUB  suba    R   :SUB A      A <- A - A - CY:
++ 98 152 10011000 SBB  sbbb    R   :SBB B      A <- A - B - CY:
++ 99 153 10011001 SBB  sbbc    R   :SBB C      A <- A - C - CY:
++ 9a 154 10011010 SBB  sbbd    R   :SBB D      A <- A - D - CY:
++ 9b 155 10011011 SBB  sbbe    R   :SBB E      A <- A - E - CY:
++ 9c 156 10011100 SBB  sbbh    R   :SBB H      A <- A - H - CY:
++ 9d 157 10011101 SBB  sbbl    R   :SBB L      A <- A - L - CY:
++ 9e 158 10011110 SBB  sbbm    R   :SBB M      A <- A - (HL) - CY:
++ 9f 159 10011111 SBB  sbba    R   :SBB A      A <- A - A - CY:
++ a0 160 10100000 ANA  anab    R   :ANA B      A <- A & B:
++ a1 161 10100001 ANA  anac    R   :ANA C      A <- A & C:
++ a2 162 10100010 ANA  anad    R   :ANA D      A <- A & D:
++ a3 163 10100011 ANA  anae    R   :ANA E      A <- A & E:
++ a4 164 10100100 ANA  anah    R   :ANA H      A <- A & H:
++ a5 165 10100101 ANA  anal    R   :ANA L      A <- A & L:
++ a6 166 10100110 ANA  anam    R   :ANA M      A <- A & (HL):
++ a7 167 10100111 ANA  anaa    R   :ANA A      A <- A & A:
++ a8 168 10101000 XRA  xrab    R   :XRA B      A <- A ^ B:
++ a9 169 10101001 XRA  xrac    R   :XRA C      A <- A ^ C:
++ aa 170 10101010 XRA  xrad    R   :XRA D      A <- A ^ D:
++ ab 171 10101011 XRA  xrae    R   :XRA E      A <- A ^ E:
++ ac 172 10101100 XRA  xrah    R   :XRA H      A <- A ^ H:
++ ad 173 10101101 XRA  xral    R   :XRA L      A <- A ^ L:
++ ae 174 10101110 XRA  xram    R   :XRA M      A <- A ^ (HL):
++ af 175 10101111 XRA  xraa    R   :XRA A      A <- A ^ A:
++ b0 176 10110000 ORA  orab    R   :ORA B      A <- A | B:
++ b1 177 10110001 ORA  orac    R   :ORA C      A <- A | C:
++ b2 178 10110010 ORA  orad    R   :ORA D      A <- A | D:
++ b3 179 10110011 ORA  orae    R   :ORA E      A <- A | E:
++ b4 180 10110100 ORA  orah    R   :ORA H      A <- A | H:
++ b5 181 10110101 ORA  oral    R   :ORA L      A <- A | L:
++ b6 182 10110110 ORA  oram    R   :ORA M      A <- A | (HL):
++ b7 183 10110111 ORA  oraa    R   :ORA A      A <- A | A:
++ b8 184 10111000 CMP  cmpb    R   :CMP B      A - B:
++ b9 185 10111001 CMP  cmpc    R   :CMP C      A - C:
++ ba 186 10111010 CMP  cmpd    R   :CMP D      A - D:
++ bb 187 10111011 CMP  cmpe    R   :CMP E      A - E:
++ bc 188 10111100 CMP  cmph    R   :CMP H      A - H:
++ bd 189 10111101 CMP  cmpl    R   :CMP L      A - L:
++ be 190 10111110 CMP  cmpm    R   :CMP M      A - (HL):
++ bf 191 10111111 CMP  cmpa    R   :CMP A      A - A:
++ c0 192 11000000 RNZ  rnz     NA  :RNZ        if NZ, RET:
++ c1 193 11000001 POP  popb    R   :POP B      C <- (sp); B <- (sp+1); sp <- sp+2:
++ c2 194 11000010 JNZ  jnz     adr :JNZ adr    if NZ, PC < adr:
++ c3 195 11000011 JMP  jmp     adr :JMP adr    PC <= adr:
++ c4 196 11000100 CNZ  cnz     adr :CNZ adr    if NZ, CALL adr:
++ c5 197 11000101 PUSH pushb   R   :PUSH B     (sp-2)<-C; (sp-1)<-B; sp <- sp - 2:
++ c6 198 11000110 ADI  adi     D8  :ADI D8     A <- A + byte:
++ c7 199 11000111 RST  rst0    D8  :RST 0      CALL $0:
++ c8 200 11001000 RZ   rz      NA  :RZ         if Z, RET:
++ c9 201 11001001 RET  ret     NA  :RET        PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2:
++ ca 202 11001010 JZ   jz      adr :JZ adr     if Z, PC <- adr:
++ cb ---
++ cc 204 11001100 CZ   cz      adr :CZ adr     if Z, CALL adr:
++ cd 205 11001101 CALL call    adr :CALL adr   (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP+2;PC=adr:
++ ce 206 11001110 ACI  aci     D8  :ACI D8     A <- A + data + CY:
++ cf 207 11001111 RST  rst1    D8  :RST 1      CALL $8:
++ d0 208 11010000 RNC  rnc     NA  :RNC        if NCY, RET:
++ d1 209 11010001 POP  popd    R   :POP D      E <- (sp); D <- (sp+1); sp <- sp+2:
++ d2 210 11010010 JNC  jnc     adr :JNC adr    if NCY, PC<-adr:
++ d3 211 11010011 OUT  out     D8  :OUT D8     special:
++ d4 212 11010100 CNC  cnc     adr :CNC adr    if NCY, CAL adr:
++ d5 213 11010101 PUSH pushd   R   :PUSH D     (sp-2)<-E; (sp-1)<-D; sp <- sp - 2:
++ d6 214 11010110 SUI  sui     D8  :SUI D8     A <- A - data:
++ d7 215 11010111 RST  rst2    D8  :RST 2      CALL $10:
++ d8 216 11011000 RC   rc      NA  :RC         if CY, RET:
++ d9 ---
++ da 218 11011010 JC   jc      adr :JC adr     if CY, PC<-adr:
++ db 219 11011011 IN   in      D8  :IN D8      special:
++ dc 220 11011100 CC   cc      adr :CC adr     if CY, CALL adr:
++ dd ---
++ de 222 11011110 SBI  sbi     D8  :SBI D8     A <- A - data - CY:
++ df 223 11011111 RST  rst3    D8  :RST 3      CALL $18:
++ e0 224 11100000 RPO  rpo     NA  :RPO        if PO, RET:
++ e1 225 11100001 POP  poph    R   :POP H      L <- (sp); H <- (sp+1); sp <- sp+2:
++ e2 226 11100010 JPO  jpo     adr :JPO adr    if PO, PC <- adr:
++ e3 227 11100011 XTHL xthl    NA  :XTHL       L <-> (SP); H <-> (SP+1) ... Set H:L to same value at the SP address and SP +1:
++ e4 228 11100100 CPO  cpo     adr :CPO adr    if PO, CALL adr:
++ e5 229 11100101 PUSH pushh   R   :PUSH H     (sp-2)<-L; (sp-1)<-H; sp <- sp - 2:
++ e6 230 11100110 ANI  ani     D8  :ANI D8     A <- A & data:
++ e7 231 11100111 RST  rst4    D8  :RST 4      CALL $20:
++ e8 232 11101000 RPE  rpe     NA  :RPE        if PE, RET:
++ e9 233 11101001 PCHL pchl    NA  :PCHL       PC.hi <- H; PC.lo <- L:
++ ea 234 11101010 JPE  jpe     adr :JPE adr    if PE, PC <- adr:
++ eb 235 11101011 XCHG xchg    NA  :XCHG       H <-> D; L <-> E:
++ ec 236 11101100 CPE  cpe     adr :CPE adr    if PE, CALL adr:
++ ed ---
++ ee 238 11101110 XRI  xri     D8  :XRI D8     A <- A ^ data:
++ ef 239 11101111 RST  rst5    D8  :RST 5      CALL $28:
++ f0 240 11110000 RP   rp      NA  :RP         if P, RET:
++ f1 241 11110001 POP  popa    R   :POP PSW    flags <- (sp); A <- (sp+1); sp <- sp+2:
++ f2 242 11110010 JP   jp      adr :JP adr     if P=1 PC <- adr:
++ f3 243 11110011 DI   di      NA  :DI         special:
++ f4 244 11110100 CP   cp      adr :CP adr     if P, PC <- adr:
++ f5 245 11110101 PUSH pusha   R   :PUSH PSW   (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2:
++ f6 246 11110110 ORI  ori     D8  :ORI D8     A <- A | data:
++ f7 247 11110111 RST  rst6    D8  :RST 6      CALL $30:
++ f8 248 11111000 RM   rm      NA  :RM         if M, RET:
++ f9 249 11111001 SPHL sphl    NA  :SPHL       SP=HL      ... Set SP to same value as H:L:
++ fa 250 11111010 JM   jm      adr :JM adr     if M, PC <- adr:
++ fb 251 11111011 EI   ei      NA  :EI         special:
++ fc 252 11111100 CM   cm      adr :CM adr     if M, CALL adr:
++ fd ---
++ fe 254 11111110 CPI  cpi     D8  :CPI D8     A - data:
++ ff 255 11111111 RST  rst7    D8  :RST 7      CALL $38:

-----------------------------------------------

*/
