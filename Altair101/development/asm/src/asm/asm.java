package asm;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class asm {

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

    public static void main(String[] args) {

        System.out.println("+++ Start 8080/8085 assembler.");
        asm dataioObj = new asm();    // none static data input
        dataioObj.run();

        System.out.println("\n++ Exit.");
    }

    public void run() {
        fileProcess doList = new fileProcess();
        String theFilename = "p1.asm";
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
                theRest = consoleInLine.substring(c1+1).trim();
            } else {
                cmd = consoleInLine.toLowerCase();
                theRest = consoleInLine.substring(c1+1);
            }
            //
            if (cmd.equals("file")) {
                // > file this.asm
                ei = theRest.indexOf(" ");
                if (ei > c1) {
                    theFilename = theRest.substring(0, ei);
                } else {
                    theFilename = theRest;
                }
                System.out.println("+ File name to use : " + theFilename + ".");
            } else if (cmd.equals("list")) {
                System.out.println("+ List file: " + theFilename + ":");
                doList.listFile(theFilename);
                // doList.listFile(theFilename, "p1.txt");
            }
            if (consoleInLine.equalsIgnoreCase("help")) {
                System.out.println("Help");
                System.out.println("+ file <filename> : set the assembler program file name to use in other commands.");
                System.out.println("+ list : list the file to screen.");
                System.out.println("+ exit : Exit program.");
            }
        }
    }
}
