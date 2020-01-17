package asm;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class asm {

    asmProcessor processFile = new asmProcessor();
    opcodes8080 theOpcodes = new opcodes8080();

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

    public void run() {
        // asmProcessor doList = new asmProcessor();
        String theFilename = "p1.asm";
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
                case "print":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print and parse the program: " + theFilename + ":");
                    processFile.parseFile(theFilename);
                    break;
                case "printarray":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print the byte array for the program: " + theFilename + ":");
                    processFile.printProgramByteArray(processFile, theFilename);
                    break;
                case "printfile":
                    System.out.println("+ -------------------------------------");
                    System.out.println("+ Print the byte array for the program: " + theFilename + ":");
                    processFile.printProgramBytesToFile(processFile, theFilename, outFilename);
                    break;
                case "file":
                    // > file this.asm
                    if (theRest.length() > 0) {
                        theFilename = theRest;
                    }
                    System.out.println("+ File name to use for input: " + theFilename + ".");
                    break;
                case "fileout":
                    // > fileout this.bin
                    if (theRest.length() > 0) {
                        theFilename = theRest;
                    }
                    System.out.println("+ File name to use for output: " + theFilename + ".");
                    break;
                case "list":
                    switch (theRest) {
                        case "":
                        case "file":
                            System.out.println("+ -------------------------------------");
                            System.out.println("+ List file: " + theFilename + ":");
                            processFile.listFile(theFilename);
                            break;
                        default:
                            System.out.println("- Invalid list option." + theRest);
                            break;
                    }
                    break;
                case "opcode":
                    theOpcodes.getOpcodeInfo(theRest);
                    break;
                case "opcodes":
                    System.out.println("+ -------------------------------------");
                    theOpcodes.opcodesList();
                    break;
                case "upload":
                    System.out.println("++ Not available, yet.");
                    break;
                case "help":
                    System.out.println("---------------------------------------");
                    System.out.println("Help");
                    System.out.println("+ file <filename>    : set the input file name to use in other commands.");
                    System.out.println("+ fileout <filename> : set the output file name to use in other commands.");
                    System.out.println("+ list [file]        : list the input file to the screen.");
                    System.out.println("");
                    System.out.println("+ print              : Parse the file, output to screen.");
                    System.out.println("+ printarray         : Print the byte array for the program to screen.");
                    System.out.println("+ printfile          : Print binary file, and output info to screen.");
                    System.out.println("");
                    System.out.println("+ opcode <opcode>    : list the opcode information to the screen.");
                    System.out.println("+ opcodes            : list the opcodes to screen.");
                    System.out.println("");
                    System.out.println("+ upload             : Serial upload the program bytes to the Arduino.");
                    System.out.println("+ exit               : Exit this program.");
                    break;
                default:
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
