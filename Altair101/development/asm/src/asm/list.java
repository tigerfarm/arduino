package asm;

import java.io.*;

public class list {

    public static void listFile(String theReadFilename, String theWriteFilename) {
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

        System.out.println("++ Start Class: transWiki2Links");

        if (args.length != 2) {
            System.out.println("+ Syntax: transWiki2Links <read filename> <write filename>");
            System.out.println("+ Example: transWiki2Links /java2012/opt/wikiDataTiger/Travel.txt /java2012/opt/write/Travel.txt");
            return;
        }
        String theReadFilename = args[0];
        System.out.println("+ theReadFilename: " + theReadFilename);
        String theWriteFilename = args[1];
        System.out.println("+ theWriteFilename: " + theWriteFilename);

        listFile(theReadFilename, theWriteFilename);

        System.out.println("++ Exit...");
    }
}
