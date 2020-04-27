/*
 * filedirInfo.java
 * Created on January 23, 2006, 9:30 AM
 * Author: Stacy David Thurston
 * Sample file directory information.
 */
package mp3dfplayerFiles;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import static java.nio.file.StandardCopyOption.COPY_ATTRIBUTES;
import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;
import java.text.SimpleDateFormat;
import java.text.Format;
import java.util.Date;

//  Linklist processing classes
import java.util.List;
import java.util.ArrayList;
import static mp3dfplayerFiles.DirSynchCopy.countCopies;

public class createDfplayerFiles {

    public static void doCopyFile(File theFullPathNameFrom, File theFullPathNameTo) {
        try {
            Files.copy(theFullPathNameFrom.toPath(), theFullPathNameTo.toPath(), REPLACE_EXISTING, COPY_ATTRIBUTES);
            System.out.println("+ doCopyFile, copied from: " + theFullPathNameFrom + " to " + theFullPathNameTo);
            countCopies++;
        } catch (IOException e) {
            System.err.println("--- doCopyFile, Error: " + e.toString());
        }
    }

    public static void directoryListing(String theDirectoryNameFrom, File dirFrom, String theDirectoryNameTo, File dirTo) {

        Format formatter = new SimpleDateFormat("dd-MMM-yy HH:mm:ss EEEE");

        // Get directory & file info into a list
        String[] children = dirFrom.list();
        List fileDirList = new ArrayList();
        for (int i = 0; i < children.length; i++) {
            String filename = children[i];
            File theName = new File(theDirectoryNameFrom + "/" + filename);
            if (!theName.isFile()) {
                // Process directories
                fileDirList.add(i, "+ Subdirectory: " + filename + " " + formatter.format(new Date(theName.lastModified())));
                // 
                // Use recursion to list subdirectories.
                File theNameTo = new File(theDirectoryNameTo + "/" + filename);
                // Create to-directory.
                System.out.println("++ Create to-directory: " + theDirectoryNameTo);
                theNameTo.mkdir();
                directoryListing(theDirectoryNameFrom + "/" + filename, theName, theDirectoryNameTo + "/" + filename, theNameTo);
            } else if (filename.compareTo("Thumbs.db") != 0) {
                // Process files, ignore the system file: Thumbs.db
                // fileDirList.add(i, "+ File: " + filename + " " + formatter.format(new Date(theName.lastModified())) + ", size: " + theName.length() + " bytes");
                fileDirList.add(i, "+ File: " + filename);
                // + File: Rush - Xanadu.mp3
                /*
                 */
                File theFullPathNameTo = new File(theDirectoryNameTo + "/" + filename);
                File theFullPathNameFrom = new File(theDirectoryNameFrom + "/" + filename);
                doCopyFile(theFullPathNameFrom, theFullPathNameTo);
            }
        }

        System.out.println("++ Directory listing for: " + dirFrom);
        // ++ Directory listing for: /Users/dthurston/2020m/musicTop/mp3dfplayerSongs1/00-new
        // Print List: directories then files
        for (int i = 0; i < fileDirList.size(); i++) {
            String item = (String) fileDirList.get(i);
            if (item.startsWith("+")) {
                System.out.println("    " + item);
                // "    + File: Rush - Xanadu.mp3"
            }
        }
        for (int i = 0; i < fileDirList.size(); i++) {
            String item = (String) fileDirList.get(i);
            if (item.startsWith("*")) {
                System.out.println("    " + item);
            }
        }
        System.out.println("eol...");

    }

    public static void main(String[] args) {

        String theClassName = "dirFilesInfo";
        System.out.println("+++ Start class: " + theClassName);

        Format formatter = new SimpleDateFormat("EEEE dd-MMM-yy HH:mm");
        Date date = new Date();
        System.out.println("+ Today's date: " + formatter.format(date));
        /*
        if (args.length != 1) {
            System.out.println("+ Syntax: dirFilesInfo <directory name>");
            return;
        }
        String theDirectoryName = args[0];
         */
        String theDirectoryNameFrom = "/Users/dthurston/2020m/musicTop/mp3dfplayerSongs1";
        File dirFrom = new File(theDirectoryNameFrom);
        if (!dirFrom.isDirectory()) {
            System.out.println("--- datasynch, Error: the <from directory> is NOT a directory:" + theDirectoryNameFrom);
            System.out.println("\n+++ Exit class: " + theClassName + ".");
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
            System.out.println("\n+++ Exit class: " + theClassName + ".");
            return;
        }
        System.out.print("++ Make:");
        System.out.print(" the <to directory> (" + theDirectoryNameTo + ")");
        System.out.println(" the same as the <from directory> (" + theDirectoryNameFrom + ").\n");
        directoryListing(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);

        System.out.println("\n+++ Exit class: " + theClassName + ".");
    }

}

// eof
