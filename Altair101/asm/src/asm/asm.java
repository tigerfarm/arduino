/*
    Interactive program to assemble programs into byte code for Processor.ini to run.

    From a command prompt, call various Altair 101 assembly functions.    
    To run:
        $ java -jar asm.jar
 */
package asm;

import static asm.asmUpload.getSerialPortName;
import static asm.asmUpload.listSerialPorts;
import static asm.asmUpload.sendFile;
import static asm.asmUpload.setSerialPortName;
import java.io.BufferedReader;
import java.io.InputStreamReader;

// Looks like I need to add the new serial port module to get this to work.
// It maybe possible to disable the USB serial port reset using to following link.
//  https://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection/
//  Stick a 120 ohm resistor in the headers between 5v and reset
public class asm {

    asmProcessor processFile = new asmProcessor();
    asmOpcodes theOpcodes = new asmOpcodes();

    private static final String asmVersion = "0.91c";

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

    // -------------------------------------------------------------------------
    public void run() {
        // asmProcessor doList = new asmProcessor();
        String inFilename = "p1.asm";
        String outFilename = "p1.bin";
        String cmd;
        String theRest;
        int si = 0;
        int ei = 0;

        String thePrompt = "> ";
        System.out.print("+ Enter 'exit' to exit. 'help' to get a command listing.");
        String consoleInLine = "";
        System.out.println("+ Again, enter 'exit' to end the input loop.");
        while (!(consoleInLine.equals("exit"))) {
            System.out.print(thePrompt);
            try {
                consoleInLine = this.br.readLine().trim();
            } catch (Exception e) {
                System.out.print("--- Error exception." + e.getMessage());
            }
            int c1 = consoleInLine.indexOf(" ", si);
            if (c1 > 0) {
                cmd = consoleInLine.substring(si, c1).toLowerCase();
                theRest = consoleInLine.substring(c1 + 1).trim();
            } else {
                cmd = consoleInLine.toLowerCase();
                theRest = "";
            }
            //
            // System.out.println("+ cmd : " + cmd + ":" + theRest + ".");
            switch (cmd) {
                case "asm":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print and parse the program: " + inFilename + ":");
                    processFile.parseFile(inFilename);
                    System.out.println("+ -------------------------------------");
                    if (processFile.getErrorCount() > 0) {
                        break;
                    }
                    System.out.println("+ Write the program byte array to the file: " + outFilename + ":");
                    processFile.programBytesListAndWrite(outFilename);
                    break;
                case "parse":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Parse the program file: " + inFilename + ":");
                    processFile.parseFile(inFilename);
                    break;
                case "write":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Write the program byte array to the file: " + outFilename + ":");
                    processFile.programBytesListAndWrite(outFilename);
                    break;
                case "show":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print binary file bytes to screen: " + outFilename + ":");
                    processFile.showFile(outFilename);
                    break;
                case "file":
                    // > file this.asm
                    if (theRest.length() > 0) {
                        inFilename = theRest;
                    }
                    System.out.println("+ Program source file name: " + inFilename + ".");
                    break;
                case "fileout":
                    // > fileout this.bin
                    if (theRest.length() > 0) {
                        outFilename = theRest;
                    }
                    System.out.println("+ File name to use for output: " + outFilename + ".");
                    break;
                // -------------------------------------------------------------
                case "list":
                    switch (theRest) {
                        case "":
                            System.out.println("+ -------------------------------------");
                            System.out.println("> list <file|bytes|opcode|ports>");
                            System.out.println("");
                        case "file":
                            System.out.println("+ -------------------------------------");
                            System.out.println("+ List program source file: " + inFilename + ":");
                            processFile.listFile(inFilename);
                            break;
                        case "bytes":
                            System.out.println("+ -------------------------------------");
                            System.out.println("+ List the parsed byte array to screen.");
                            processFile.programBytesListAndWrite("");
                            break;
                        case "opcodes":
                            System.out.println("+ -------------------------------------");
                            theOpcodes.opcodeInfoList();
                            break;
                        case "ports":
                            listSerialPorts();
                            break;
                        default:
                            System.out.println("- Invalid list option." + theRest);
                            break;
                    }
                    break;
                // -------------------------------------------------------------
                case "opcode":
                    theOpcodes.printOpcodeInfo(theRest);
                    break;
                case "opcodes":
                    theOpcodes.opcodesList();
                    break;
                case "opcodenames":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesListByName();
                    break;
                case "opcodebytes":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesListByValue();
                    break;
                // -------------------------------------------------------------
                case "upload":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Write to the serail port, the program file: " + outFilename + ":");
                    sendFile(outFilename);
                    break;
                case "uploadset":
                    if (theRest.length() == 0) {
                        System.out.println("+ Serial port name set to: uploadset: " + getSerialPortName());
                    } else {
                        System.out.println("+ Set the serial port name: " + theRest);
                        setSerialPortName(theRest);
                    }
                    break;
                // -------------------------------------------------------------
                case "exit":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+++ Exit.");
                    break;
                // -------------------------------------------------------------
                // -------------------------------------------------------------
                case "help":
                    System.out.println("---------------------------------------");
                    System.out.println("Altair 101 8080/8085 assembler, version " + asmVersion);
                    System.out.println("");
                    System.out.println("Help document,");
                    System.out.println("----------------------");
                    System.out.println("+ file <source>      : Set the input file name to use in other commands.");
                    if (!inFilename.equals("")) {
                        System.out.println("++ Source file   : " + inFilename);
                    }
                    System.out.println("+ fileout <filename> : Set the machine code file name.");
                    if (!inFilename.equals("")) {
                        System.out.println("++ Machine code file: " + outFilename);
                    }
                    System.out.println("");
                    System.out.println("----------------------");
                    System.out.println("+ asm                : Parse the input file and write the bytes to a file.");
                    System.out.println("+ parse              : Parse the input file.");
                    System.out.println("+ write              : Write the bytes to a binary file.");
                    System.out.println("+ list bytes         : List the parsed bytes and info.");
                    System.out.println("+ list [file]        : List the program source file.");
                    System.out.println("+ show               : Print machine code file bytes to screen.");
                    System.out.println("");
                    System.out.println("+ list opcodes       : list the opcode information, ordered by the name.");
                    System.out.println("+ opcodes            : list the opcode information, ordered the same as in the file.");
                    System.out.println("+ opcode <opcode>    : list an opcode's information.");
                    System.out.println("+ opcodebytes        : list the opcode data, sorted by value.");
                    System.out.println("+ opcodenames        : list the opcode data, sorted by name.");
                    System.out.println("");
                    System.out.println("+ upload             : Serial upload the program bytes to the Arduino.");
                    System.out.println("++ Serial port name: " + getSerialPortName());
                    System.out.println("+ list ports         : List available serial ports.");
                    System.out.println("+ uploadset          : Set serial port to use for uploading.");
                    System.out.println("----------------------");
                    System.out.println("");
                    System.out.println("+ exit               : Exit this program.");
                    System.out.println("");
                    System.out.println("> list <file|bytes|opcodes|ports>");
                    System.out.println("");
                    break;
                default:
                    if (!cmd.equals("")) {
                        System.out.println("- Invalid command: " + cmd);
                    }
                    break;
            }
        }
    }

    public static void main(String[] args) {
        System.out.println("+++ Start 8080/8085 assembler, version " + asmVersion);
        System.out.println("");
        asm asmProcess = new asm();
        asmProcess.run();
        System.out.println("\n++ Exit.");
    }
}
