/*
 * List file directory information.
 */
package mp3player;

import java.io.File;
import java.text.SimpleDateFormat;
import java.text.Format;
import java.util.Date;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;

public class mp3playerList {

    public static void directoryListing(String theDirectoryName, File theDirectory) {
        Format formatter = new SimpleDateFormat("dd-MMM-yyyy HH:mm:ss EEEE");
        // Get directory & file info into a list
        String[] children = theDirectory.list();
        Arrays.sort(children);
        List fileDirList = new ArrayList();
        for (int i = 0; i < children.length; i++) {
            String filename = children[i];
            System.out.println("++ Directory filename: " + filename);
            File theName = new File(theDirectoryName + "/" + filename);
            if (!theName.isFile()) {
                // Process directories
                fileDirList.add(i, "+ Subdirectory: " + filename + " " + formatter.format(new Date(theName.lastModified())));
                // Use recursion to list subdirectories.
                directoryListing(theDirectoryName + "/" + filename, theName);
            } else {
                // Process Files
                fileDirList.add(i, "+ File: " + filename);
            }
        }
        System.out.println("++ Directory listing for: " + theDirectory);
        for (int i = 0; i < fileDirList.size(); i++) {
            String item = (String) fileDirList.get(i);
            if (item.startsWith("+")) {
                System.out.println("    " + item);
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
        System.out.println("+++ Start.");
        //String theDirectoryName = "../../../../../2020m/musicTop/mp3player1";
        String theDirectoryName = "/Users/dave/2021p/musicTop/mp3player0";
        File dir = new File(theDirectoryName);
        if (!dir.isDirectory()) {
            System.out.println("-- Error: " + theDirectoryName + " is not a directory...");
            return;
        }
        if (!dir.exists()) {
            System.out.println("-- Error: " + theDirectoryName + ", directory does not exist...");
            return;
        }
        System.out.println("+ Get information for the directory: " + theDirectoryName);
        String theParent = new File(theDirectoryName).getParent();
        if (theParent == null) {
            theDirectoryName = "";
        } else {
            System.out.println("+ Parent: " + theParent);
        }
        directoryListing(theDirectoryName, dir);
        System.out.println("\n+++ Exit.");
    }

}

// eof
