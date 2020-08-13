/*
    From a directory of MP3 files, create a directory of MP3 files
        using using directory and file names that work with the DFPlayer module.

    Program is courtesy of Tiger Farm Press.
    License: GPL.
 */
package mp3player;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import static java.nio.file.StandardCopyOption.COPY_ATTRIBUTES;
import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

public class mp3player {

    static private int countFiles = 1;
    static private int countDirs = 0;

    public static void dfPlayerCopy(String theDirectoryNameFrom, File dirFrom, String theDirectoryNameTo, File dirTo) {
        // Get directory & file info into a list
        String[] children = dirFrom.list();
        for (int i = 0; i < children.length; i++) {
            String filename = children[i];
            File theName = new File(theDirectoryNameFrom + "/" + filename);
            if (!theName.isFile()) {
                // Process directories
                // 
                // Create to-directory.
                countDirs++;
                String countDirName = String.valueOf(countDirs);
                if (countDirs < 10) {
                    countDirName = "0" + countDirName;
                }
                System.out.println("++ From directory: " + theDirectoryNameTo + ", Create to-directory: " + countDirName);
                File theNameTo = new File(theDirectoryNameTo + "/" + countDirName);
                theNameTo.mkdir();
                //
                // Use recursion to process subdirectories.
                dfPlayerCopy(theDirectoryNameFrom + "/" + filename, theName, theDirectoryNameTo + "/" + countDirName, theNameTo);
                //
            } else if (filename.toLowerCase().endsWith(".mp3")) {
                // Process MP3 files.
                File theFullPathNameFrom = new File(theDirectoryNameFrom + "/" + filename);
                String countFilesName = String.valueOf(countFiles);
                if (countFiles < 10) {
                    countFilesName = "000" + countFilesName;
                } else if (countFiles < 100) {
                    countFilesName = "00" + countFilesName;
                } else if (countFiles < 1000) {
                    countFilesName = "0" + countFilesName;
                }
                File theFullPathNameTo = new File(theDirectoryNameTo + "/" + countFilesName + ".mp3");
                // doCopyFile(theFullPathNameFrom, theFullPathNameTo);
                try {
                    Files.copy(theFullPathNameFrom.toPath(), theFullPathNameTo.toPath(), REPLACE_EXISTING, COPY_ATTRIBUTES);
                    System.out.println("+ doCopyFile, copied from: " + theFullPathNameFrom + " to " + theFullPathNameTo);
                    countFiles++;
                } catch (IOException e) {
                    System.err.println("--- doCopyFile, Error: " + e.toString());
                }

            }
        }
        System.out.println("eol...");

    }

    public static void main(String[] args) {
        System.out.println("+++ Start, DFPlayer module copy program.");
        if (args.length == 0) {
            System.out.println("Syntax:");
            System.out.println("   java -jar mp3player.jar copy [<IN: MP3 directory>  <OUT: DFPlayer formatted MP3 directory>]");
            System.out.println("");
            System.out.println("----------------------");
            System.out.println("This program copies a directory of MP3 files");
            System.out.println("   to create another directory of MP3 files");
            System.out.println("   using using directory and file names that work with a DFPlayer module.");
            System.out.println("");
            System.out.println("Before running this program,");
            System.out.println("+ Create a directory of your MP3 files.");
            System.out.println("+ Create a destination directory.");
            System.out.println("+ The destination directory is where the MP3 files will be copied to,");
            System.out.println("++ using digit number directory and file names.");
            System.out.println("+ Your destination directory should be empty.");
            System.out.println("+ If there are files in it, delete the files and directories.");
            System.out.println("----------------------");
            System.out.println("+ Run this program.");
            System.out.println("+ Syntax:");
            System.out.println("   java -jar mp3player.jar copy [<IN: MP3 directory>  <OUT: DFPlayer formatted MP3 directory>]");
            System.out.println("+ Syntax using defaults:");
            System.out.println("   java -jar mp3player.jar copy");
            System.out.println("+ Default directory names: mp3player1 and mp3player2.");
            System.out.println("+ Same as:");
            System.out.println("   java -jar mp3player.jar copy mp3player1 mp3player2.");
            System.out.println("----------------------");
            System.out.println("+ Insert the SD card into your computer.");
            System.out.println("+ Delete the directories and files from the SD card.");
            System.out.println("+ Empty trash because the files are still on the SD card and the DFPlayer module may play them.");
            System.out.println("+ Copy the new directories and files to the SD card.");
            System.out.println("+ Eject the card from the computer.");
            System.out.println("----------------------");
            System.out.println("+ Insert the card into the DFPlayer module.");
            System.out.println("+ The card is ready to play.");
            System.out.println("");
            return;
        }
        String theDirectoryNameFrom = "mp3player1";
        String theDirectoryNameTo = "mp3player2";
        if (args[0].compareToIgnoreCase("copy") == 0) {
            if (args.length == 3) {
                theDirectoryNameFrom = args[1];
                theDirectoryNameTo = args[2];
            }
            if (args.length == 2) {
                System.out.println("copy <IN: MP3 directory>  <OUT: DFPlayer formatted MP3 directory>");
                return;
            }
        }
        //
        File dirFrom = new File(theDirectoryNameFrom);
        if (!dirFrom.isDirectory()) {
            System.out.println("- Error: the <IN: directory> is NOT a directory:" + theDirectoryNameFrom);
            System.out.println("\n+++ Exit.");
            return;
        }
        if (theDirectoryNameTo.endsWith("/")) {
            // System.out.print("\n+ theDirectoryNameTo :"+theDirectoryNameTo);
            theDirectoryNameTo = theDirectoryNameTo.substring(0, theDirectoryNameTo.length() - 1);
            // System.out.println(":"+theDirectoryNameTo+".");
        }
        File dirTo = new File(theDirectoryNameTo);
        if (!dirTo.isDirectory()) {
            System.out.println("- Error: the <OUT: directory> is NOT a directory:" + theDirectoryNameTo);
            System.out.println("\n+++ Exit.");
            return;
        }
        System.out.print("++ Copy into DFPlayer formatted file and directory names,");
        System.out.print(" the <IN: directory> (" + theDirectoryNameTo + ")");
        System.out.println(" to the <OUT: directory> (" + theDirectoryNameFrom + ").\n");
        dfPlayerCopy(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);

        System.out.println("\n+++ Exit.");
    }
}

// eof
