package asm;

import java.io.*;

public class fileProcess {
    
    opcodes8080 theOpcodes = new opcodes8080();

    private void parseLine(String orgLine) {
        String label;
        String opcode;
        String p1;
        String p2;
        String theRest;
        int si = 0;
        int ei = 0;
        int c1 = 0;
        //
        String theLine = orgLine.trim();
        if (theLine.length() == 0) {
            System.out.println("++");
            return;
        }
        if (theLine.startsWith(";")) {
            System.out.println("++ " + orgLine);
            return;
        }
        //
        ei = theLine.indexOf(";");
        if (ei > 1) {
            // Remove trailing comment.
            //    mvi a,1     ; Move 1 to A.
            //  > mvi a,1
            theLine = theLine.substring(0, ei).trim();
        }
        // ---------------------------------------------------------------------
        // Label line: "Start:"
        if (theLine.endsWith(":")) {
            label = theLine.substring(0, theLine.length() - 1).toLowerCase();
            System.out.println("++ label: " + label);
            return;
        }
        // ---------------------------------------------------------------------
        // Opcode line.
        // Opcodes have 0, 1, or 2 parameters.
        // 0) nop
        // 1) jmp Next
        // 2) mvi a,1

        c1 = theLine.indexOf(" ");
        if (c1 < 1) {
            // Opcode, no parameters, example: "nop".
            opcode = theLine.toLowerCase();
            System.out.println("++ Opcode, no parameters: " + opcode);
            return;
        }
        //
        opcode = theLine.substring(0, c1).toLowerCase();
        theRest = theLine.substring(c1 + 1).trim();
        //
        c1 = theRest.indexOf(",");
        if (c1 < 1) {
            // Opcode, Single parameter, example: "jmp Next".
            p1 = theRest;
            System.out.println("++ Opcode, Single parameter|" + opcode + "| p1|" + p1 + "|");
            return;
        }
        p1 = theRest.substring(0, c1).trim();
        if (theRest.length() <= c1 + 1) {
            // Error, example: "mvi e,".
            System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "| * Error, missing p2.");
            return;
        }
        p2 = theRest.substring(c1 + 1).trim();
        //
        System.out.println("++ Opcode|" + opcode + "| p1|" + p1 + "|" + " p2|" + p2 + "|");
    }

    public void parseFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
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
    }

    public void listFile(String theReadFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;
        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
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

    public static void transFile(String theReadFilename, String theWriteFilename) {
        File readFile;
        FileInputStream fin;
        DataInputStream pin;

        File writeFile;
        FileOutputStream fout;
        PrintStream pout;

        try {
            readFile = new File(theReadFilename);
            if (!readFile.exists()) {
                System.out.println("+ ** ERROR, theReadFilename does not exist.");
                return;
            }
            fin = new FileInputStream(readFile);
            pin = new DataInputStream(fin);

            writeFile = new File(theWriteFilename);
            fout = new FileOutputStream(writeFile);
            pout = new PrintStream(fout);

            int si = -1;
            int boldOpen;
            int boldClose;
            int bracketOpen;
            int bar;
            int bracketClose;
            String transLine;
            String theHREF;
            String theLine = pin.readLine();
            while (theLine != null) {
                transLine = theLine;
                if (theLine.startsWith("----")) {
                    transLine = "<div><hr id=\"Horizontal Line\"></div>";
                } else if (theLine.compareTo("") == 0) {
                    transLine = "<p>";
                } else if (theLine.compareTo("\\\\") == 0) {
                    transLine = "<div><br></div>";
                } else if (theLine.startsWith("{{{")) {
                    transLine = "<pre>";
                } else if (theLine.startsWith("}}}")) {
                    transLine = "</pre>";
                } else if (theLine.startsWith("!!")) {
                    transLine = "<h3>";
                    transLine = transLine + theLine.substring(1, theLine.length());
                    transLine = transLine + "</h3>";
                } else if (theLine.startsWith("!")) {
                    transLine = "<h4>";
                    transLine = transLine + theLine.substring(1, theLine.length());
                    transLine = transLine + "</h4>";
                } else {
                    if (theLine.startsWith("\\\\")) {
                        transLine = "<div>";
                        transLine = transLine + theLine.substring(2, theLine.length());
                        transLine = transLine + "</div>";
                        theLine = transLine;
                    }
                    si = -1;

                    // Bolding: __Education__
                    boldOpen = theLine.indexOf("__", si);
                    boldClose = theLine.indexOf("__", boldOpen + 2);
                    if ((boldOpen >= 0) && (boldClose > boldOpen)) {
                        transLine = "<b>";
                        if (boldOpen > 0) {
                            transLine = theLine.substring(0, boldOpen) + "<b>";
                        }
                        transLine = transLine + theLine.substring(boldOpen + 2, boldClose) + "</b>";
                        theLine = transLine + theLine.substring(boldClose + 2, theLine.length());
                    }

                    // Wiki external links [abc|http...] to HREF links
                    si = -1;
                    bracketOpen = theLine.indexOf("[", si);
                    bar = theLine.indexOf("|", si);
                    bracketClose = theLine.indexOf("]", si);
                    if ((bracketOpen >= 0) && (bar > bracketOpen) && (bracketClose > bar)) {
                        theHREF = "<a href=\"";
                        theHREF = theHREF + theLine.substring(bar + 1, bracketClose);
                        theHREF = theHREF + "\" target=\"_blank\">";
                        theHREF = theHREF + theLine.substring(bracketOpen + 1, bar);
                        theHREF = theHREF + "</a>";
                        if (bracketOpen == 0) {
                            transLine = "";
                        } else {
                            transLine = theLine.substring(0, bracketOpen);
                        }
                        transLine = transLine + theHREF;
                        si = transLine.length() + 1;
                        theLine = transLine + theLine.substring(bracketClose + 1, theLine.length());
                        transLine = theLine;
                    }
                    bracketOpen = theLine.indexOf("[", si);
                    si = bracketOpen + 1;
                    bar = theLine.indexOf("|", si);
                    bracketClose = theLine.indexOf("]", si);
                    if ((bracketOpen >= 0) && (bar > bracketOpen) && (bracketClose > bar)) {
                        theHREF = "<a href=\"";
                        theHREF = theHREF + theLine.substring(bar + 1, bracketClose);
                        theHREF = theHREF + "\" target=\"_blank\">";
                        theHREF = theHREF + theLine.substring(bracketOpen + 1, bar);
                        theHREF = theHREF + "</a>";
                        if (bracketOpen == 0) {
                            transLine = "";
                        } else {
                            transLine = theLine.substring(0, bracketOpen);
                        }
                        transLine = transLine + theHREF;
                        theLine = transLine + theLine.substring(bracketClose + 1, theLine.length());
                        transLine = theLine;
                    }

                    // Wiki external links [http...] to HREF links
                    si = -1;
                    bracketOpen = theLine.indexOf("[", si);
                    bar = theLine.indexOf("|", si);
                    bracketClose = theLine.indexOf("]", si);
                    if ((bracketOpen >= 0) && (bar < bracketOpen) && (bracketClose > bracketOpen)) {
                        theHREF = "<a href=\"";
                        theHREF = theHREF + theLine.substring(bracketOpen + 1, bracketClose);
                        theHREF = theHREF + "\" target=\"_blank\">";
                        theHREF = theHREF + theLine.substring(bracketOpen + 1, bracketClose);
                        theHREF = theHREF + "</a>";
                        if (bracketOpen == 0) {
                            transLine = "";
                        } else {
                            transLine = theLine.substring(0, bracketOpen);
                        }
                        transLine = transLine + theHREF;
                        si = transLine.length() + 1;
                        theLine = transLine + theLine.substring(bracketClose + 1, theLine.length());
                        transLine = theLine;
                    }
                }
                System.out.println(transLine);
                pout.println(transLine);
                theLine = pin.readLine();
            }
            pin.close();
            pout.close();
        } catch (IOException ioe) {
            System.out.print("+ *** IOException: ");
            System.out.println(ioe.toString());
        }

    }

    public static void main(String args[]) {
        System.out.println("++ Start.");
        fileProcess thisProcess = new fileProcess();

        thisProcess.parseFile("p1.asm");

        System.out.println("++ Exit.");
    }
}
