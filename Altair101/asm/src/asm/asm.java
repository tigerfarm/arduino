package asm;

import com.fazecast.jSerialComm.SerialPort;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.logging.Level;
import java.util.logging.Logger;

// Looks like I need to add the new serial port module to get this to work.
// It maybe possible to disable the USB serial port reset using to following link.
//  https://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection/
//  Stick a 120 ohm resistor in the headers between 5v and reset

public class asm {

    asmProcessor processFile = new asmProcessor();
    asmOpcodes theOpcodes = new asmOpcodes();

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // To view serial ports:
    //  $ ls /dev/tty.*

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
    public static void sendFile(String theReadFilename) {
        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        //SerialPort sp = SerialPort.getCommPort("/dev/cu.usbmodem14120");
        SerialPort sp = SerialPort.getCommPort("/dev/cu.wchusbserial14120");
        // Connection settings must match Arduino program settings.
        // Baud rate, data bits, stop bits, and parity
        sp.setComPortParameters(9600, 8, 1, 0);
        // block until bytes can be written
        sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0);
        if (!sp.openPort()) {
            System.out.println("- Error, failed to open port.");
            return;
        }
        System.out.println("+ Port is open.");
        // System.out.println("++ Write out binary file: " + theReadFilename);
        int theLength = 0;
        byte bArray[] = null;
        try {
            // File theFile = new File(theReadFilename);
            File theFile = new File("/Users/dthurston/Projects/arduino/Altair101/asm/10000000.bin");
            theLength = (int) theFile.length();
            bArray = new byte[(int) theLength];
            FileInputStream in = new FileInputStream(theReadFilename);
            in.read(bArray);
            in.close();
        } catch (IOException ioe) {
            System.out.print("IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("+ Write to serial port. Number of bytes: " + theLength + " in the file: " + theReadFilename);
        //
        Integer i;
        int tenCount = 0;
        try {
            for (i = 0; i < theLength; i++) {
                if (tenCount == 10) {
                    tenCount = 0;
                    System.out.println("");
                }
                tenCount++;
                // Print binary formatted output for viewing with Examine/Next.
                System.out.print(byteToString(bArray[i]) + " ");
                // Hex: System.out.print(String.format("%02X ", bArray[i]));
                sp.getOutputStream().write(i.byteValue());
                sp.getOutputStream().flush();
            }
        } catch (IOException ex) {
            Logger.getLogger(asm.class.getName()).log(Level.SEVERE, null, ex);
        }
        // ---------------------------------------------------------------------
        if (sp.closePort()) {
            System.out.println("+ Serial port is closed.");
        } else {
            System.out.println("- Error: Failed to close serial port.");
        }
        //
        System.out.println("\n+ Write completed.");
    }

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
                case "parsefile":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print and parse the program: " + inFilename + ":");
                    processFile.parseFile(inFilename);
                    break;
                case "listbytes":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ List the parsed byte array to screen.");
                    processFile.listProgramBytes();
                    break;
                case "writebytes":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Write the program byte array to the file: " + outFilename + ":");
                    processFile.printProgramBytesToFile(outFilename);
                    break;
                case "showfile":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print binary file bytes to screen: " + outFilename + ":");
                    processFile.showFile(outFilename);
                    break;
                case "printarray":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print the byte array for the program: " + inFilename + ":");
                    processFile.printProgramBytesArray();
                    break;
                case "file":
                    // > file this.asm
                    if (theRest.length() > 0) {
                        inFilename = theRest;
                    }
                    System.out.println("+ File name to use for input: " + inFilename + ".");
                    break;
                case "fileout":
                    // > fileout this.bin
                    if (theRest.length() > 0) {
                        outFilename = theRest;
                    }
                    System.out.println("+ File name to use for output: " + outFilename + ".");
                    break;
                case "list":
                    switch (theRest) {
                        case "":
                        case "file":
                            System.out.println("+ -------------------------------------");
                            System.out.println("+ List file: " + inFilename + ":");
                            processFile.listFile(inFilename);
                            break;
                        default:
                            System.out.println("- Invalid list option." + theRest);
                            break;
                    }
                    break;
                case "opcode":
                    theOpcodes.printOpcodeInfo(theRest);
                    break;
                case "opcodes":
                    theOpcodes.opcodeInfoList();
                    break;
                case "opcodelist":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesList();
                    break;
                case "opcodebytes":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesListByValue();
                    break;
                case "opcodenames":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesListByName();
                    break;
                case "upload":
                    // processFile.uploadFile(outFilename);
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Write to the serail port, the program file: " + outFilename + ":");
                    sendFile(outFilename);
                    break;
                case "help":
                    System.out.println("---------------------------------------");
                    System.out.println("Help");
                    System.out.println("+ file <filename>    : set the input file name to use in other commands.");
                    if (!inFilename.equals("")) {
                        System.out.println("++ Input file name: " + inFilename);
                    }
                    System.out.println("+ fileout <filename> : set the output file name to use in other commands.");
                    if (!inFilename.equals("")) {
                        System.out.println("++ Output file name: " + outFilename);
                    }
                    System.out.println("+ list [file]        : list the input file to the screen.");
                    System.out.println("");
                    System.out.println("+ parsefile          : Parse the file, output to screen.");
                    System.out.println("+ listbytes          : List the parsed bytes to screen.");
                    System.out.println("+ writebytes         : Write bytes to a binary file, and output info to screen.");
                    System.out.println("+ showfile           : Print binary file bytes to screen.");
                    System.out.println("+ printarray         : Print the byte array for the program to screen.");
                    System.out.println("");
                    System.out.println("+ opcode <opcode>    : list an opcode's information.");
                    System.out.println("+ opcodes            : list opcode information.");
                    System.out.println("+ opcodelist         : list the opcode data, ordered the same as in the file.");
                    System.out.println("+ opcodebytes        : list the opcode data, sorted by value.");
                    System.out.println("+ opcodenames        : list the opcode data, sorted by name.");
                    System.out.println("");
                    System.out.println("+ upload             : Serial upload the program bytes to the Arduino.");
                    System.out.println("+ exit               : Exit this program.");
                    break;
                case "exit":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+++ Exit.");
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
        System.out.println("+++ Start 8080/8085 assembler.");

        asm asmProcess = new asm();    // none static data input
        asmProcess.run();

        System.out.println("\n++ Exit.");
    }
}
