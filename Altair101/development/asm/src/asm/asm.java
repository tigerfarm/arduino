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
        list doList = new list();
        String theFilename = "p1.asm";

        String thePrompt = "> ";
        System.out.print("+ Enter 'exit' to exit. 'help' to get a command listing.");
        String consoleInLine = "";
        System.out.println("+ Again, enter 'exit' to end the input loop.");
        while (!(consoleInLine.equals("exit"))) {
            System.out.print(thePrompt);
            try {
                consoleInLine = this.br.readLine();
            } catch (Exception e) {
                System.out.print("--- Error exception." + e.getMessage());
            }
            /*
                if (!consoleInLine.equalsIgnoreCase("")) {
                    System.out.println("+ Echo input string :" + consoleInLine + ":");
                }
             */
            if (consoleInLine.equalsIgnoreCase("file")) {
                System.out.println("+ File name to use : " + theFilename + ".");
            } else if (consoleInLine.equalsIgnoreCase("list")) {
                System.out.println("+ List file: " + theFilename + ":");
                doList.listFile(theFilename,"p1.txt");
            }
            if (consoleInLine.equalsIgnoreCase("help")) {
                System.out.println("Help");
                System.out.println("+ file <filename> : set the assembler program file name to use in other commands.");
                System.out.println("+ exit : Exit program.");
            }
        }
    }
}
