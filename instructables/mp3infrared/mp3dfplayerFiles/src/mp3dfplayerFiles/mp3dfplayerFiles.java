/*
    From a directory of MP3 files, create a directory of MP3 files
        using using directory and file names that work with the DFPlayer module.

    Before running this program:
    + Set the directory names in the main method.
    + The delete files and directories in the to-directory.
    + Run this program.
    ----------------------
    + Insert the SD card into your computer.
    + Delete the file from the SD card.
    + Empty trash because the files are still on the SD card adn the DFPlayer module may play them.
    + Copy the new directories and files to the SD card.
    + Eject the card from the computer.
    ----------------------
    + Insert the card into the DFPlayer module.

    Program is courtesy of Tiger Farm Press.
    License: GPL.
 */
package mp3dfplayerFiles;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import static java.nio.file.StandardCopyOption.COPY_ATTRIBUTES;
import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

public class mp3dfplayerFiles {

    static private int countFiles = 0;
    static private int countDirs = 0;

    public static void directoryProcess(String theDirectoryNameFrom, File dirFrom, String theDirectoryNameTo, File dirTo) {
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
                directoryProcess(theDirectoryNameFrom + "/" + filename, theName, theDirectoryNameTo + "/" + countDirName, theNameTo);
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
        System.out.println("+++ Start...");
        String theDirectoryNameFrom = "/Users/dthurston/2020m/musicTop/mp3dfplayerSongs1";
        File dirFrom = new File(theDirectoryNameFrom);
        if (!dirFrom.isDirectory()) {
            System.out.println("--- datasynch, Error: the <from directory> is NOT a directory:" + theDirectoryNameFrom);
            System.out.println("\n+++ Exit.");
            return;
        }
        String theDirectoryNameTo = "/Users/dthurston/2020m/musicTop/mp3dfplayerSongs2";
        if (theDirectoryNameTo.endsWith("/")) {
            // System.out.print("\n+ theDirectoryNameTo :"+theDirectoryNameTo);
            theDirectoryNameTo = theDirectoryNameTo.substring(0, theDirectoryNameTo.length() - 1);
            // System.out.println(":"+theDirectoryNameTo+".");
        }
        File dirTo = new File(theDirectoryNameTo);
        if (!dirTo.isDirectory()) {
            System.out.println("--- datasynch, Error: the <to directory> is NOT a directory:" + theDirectoryNameTo);
            System.out.println("\n+++ Exit.");
            return;
        }
        System.out.print("++ Make:");
        System.out.print(" the <to directory> (" + theDirectoryNameTo + ")");
        System.out.println(" the same as the <from directory> (" + theDirectoryNameFrom + ").\n");
        directoryProcess(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);

        System.out.println("\n+++ Exit.");
    }
}

// eof
