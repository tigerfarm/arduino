/*
 * DirSynchCopy.java
 * Created on June 2016
 * Author: Stacy David Thurston
 * Synching directories and files from one device to another, example: from a hard drive to a USB drive.
 */
package mp3dfplayerFiles;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import static java.nio.file.StandardCopyOption.COPY_ATTRIBUTES;
import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DirSynchCopy {

    static Format formatter = new SimpleDateFormat("EEEE dd-MMM-yy HH:mm");
    static int countCopies=0;
    static int countDeletes=0;

    public static void doDeleteFile(File theFullPathNameTo) {
        try {
            Files.delete(theFullPathNameTo.toPath());
            System.out.println("+ Deleted: " + theFullPathNameTo);
            countDeletes++;
        } catch (IOException e) {
            System.err.println("--- doDelete, Error: " + e.toString());
        }
    }

    public static void doDeleteFolder(File theDirectoryTo) {
        String[] childrenTo = theDirectoryTo.list();
        System.out.println("> doDeleteFolder: " + "(" + childrenTo.length + ") " + theDirectoryTo);
        int i = 0;
        while (i < childrenTo.length) {
            String childname = childrenTo[i];
            // System.out.println("+ child to delete: " + childname);
            File theFullPathNameTo = new File(theDirectoryTo + "/" + childname);
            if (theFullPathNameTo.isFile()) {
                doDeleteFile(theFullPathNameTo);
            }
            if (theFullPathNameTo.isDirectory()) {
                // System.out.println("+ child directory to delete: " + childname);
                doDeleteFolder(theFullPathNameTo);
            }
            i++;
        }
        doDeleteFile(theDirectoryTo);
        System.out.println("< doDeleteFolder: " + theDirectoryTo);
    }

    public static void directorySynchDel(
            String theDirectoryNameFrom, File theDirectoryFrom,
            String theDirectoryNameTo, File theDirectoryTo) {

        // Get directory & file info into a list
        String[] childrenTo = theDirectoryTo.list();

        // System.out.println("+ remove directories, if any to remove.");
        int i = 0;
        while (i < childrenTo.length) {
            String childname = childrenTo[i];
            File theFullPathNameTo = new File(theDirectoryTo + "/" + childname);
            if (theFullPathNameTo.isDirectory()) {
                File theFullPathNameFrom = new File(theDirectoryFrom + "/" + childname);
                if (!theFullPathNameFrom.isDirectory()) {
                    // System.out.println("+ directorySynchDel, Not a 'from' directory: " + childname);
                    doDeleteFolder(theFullPathNameTo);
                }
            }
            i++;
        }

        // System.out.println("+ remove files, if any to remove.");
        i = 0;
        while (i < childrenTo.length) {
            String childname = childrenTo[i];
            if (childname.equalsIgnoreCase("thumbs.db")) {
                i++;
                continue;
            }
            File theFullPathNameTo = new File(theDirectoryTo + "/" + childname);
            if (theFullPathNameTo.isFile()) {
                File theFullPathNameFrom = new File(theDirectoryFrom + "/" + childname);
                if (!theFullPathNameFrom.exists()) {
                    System.out.print("+ in 'to' and not in 'from' ");
                    doDeleteFile(theFullPathNameTo);
                }
            }
            i++;
        }
    }

    public static void doCopyFile(File theFullPathNameFrom, File theFullPathNameTo) {
        try {
            Files.copy(theFullPathNameFrom.toPath(), theFullPathNameTo.toPath(), REPLACE_EXISTING, COPY_ATTRIBUTES);
            System.out.println("+ doCopyFile, copied from: " + theFullPathNameFrom + " to " + theFullPathNameTo);
            countCopies++;
        } catch (IOException e) {
            System.err.println("--- doCopyFile, Error: " + e.toString());
        }
    }

    public static void directorySynchUpd(
            String theDirectoryNameFrom, File theDirectoryFrom,
            String theDirectoryNameTo, File theDirectoryTo) {

        // Get directory & file info into a list and process the list.
        String[] childrenFrom = theDirectoryFrom.list();

        // Process Directories
        for (int i = 0; i < childrenFrom.length; i++) {
            String childname = childrenFrom[i];
            if (childname.equalsIgnoreCase("build")) {
                // ignore NetBeans project build directories
                continue;
            }
            if (childname.equalsIgnoreCase("private") && theDirectoryNameFrom.endsWith("nbproject")) {
                // ignore NetBeans project private directories
                continue;
            }
            File theFullPathNameTo = new File(theDirectoryTo + "/" + childname);
            File theFullPathNameFrom = new File(theDirectoryFrom + "/" + childname);
            if (theFullPathNameFrom.isDirectory()) {
                // System.out.println("+ directorySynchUpd, From directory: "+theDirectoryNameFrom+", child directory: " + childname);
                if (!theFullPathNameTo.exists()) {
                    doCopyFile(theFullPathNameFrom, theFullPathNameTo);
                }
                doSynch(
                        theDirectoryNameFrom + "/" + childname, theFullPathNameFrom,
                        theFullPathNameTo.toString().replace('\\', '/'), theFullPathNameTo);
            }
        }

        // Process Files
        for (int i = 0; i < childrenFrom.length; i++) {
            String childname = childrenFrom[i];
            if (childname.equalsIgnoreCase("thumbs.db")) {
                continue;
            }
            File theFullPathNameFrom = new File(theDirectoryFrom + "/" + childname);
            if (theFullPathNameFrom.isFile()) {
                // System.out.println("+ directorySynchUpd, From directory: "+theDirectoryNameFrom+", child file: " + childname);
                Date FromChildDate = new Date(theFullPathNameFrom.lastModified());
                // System.out.println("+ file child: " + childname + " " + formatter.format(FromChildDate));
                File theFullPathNameTo = new File(theDirectoryTo + "/" + childname);
                if (!theFullPathNameTo.exists()) {
                    System.out.print("+ 'to' file does not exist, ");
                } else {
                    Date ToChildDate = new Date(theFullPathNameTo.lastModified());
                    int difV = ToChildDate.compareTo(FromChildDate);
                    if (difV == 0) {
                        // System.out.println("+ 'from' and to' are the same.");
                        continue;
                    }
                    if (difV < 0) {
                        System.out.print("+ 'from' file is newer, ");
                    } else if (difV > 0) {
                        // Ensure 'to' is same as 'from'.
                        System.out.print("+ Overwrite newer 'to' file, ");
                    }
                }
                doCopyFile(theFullPathNameFrom, theFullPathNameTo);
            }
        }
    }

    public static void doSynch(
            String theDirectoryNameFrom, File dirFrom,
            String theDirectoryNameTo, File dirTo) {

        // System.out.println("+ doSynch, from: "+theDirectoryNameFrom+", to: "+theDirectoryNameTo);
        directorySynchUpd(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);
        
        if (!(dirFrom.list().length == dirTo.list().length)) {
            // Delete files in dirTo which are not in dirFrom.
            directorySynchDel(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);
        }
    }

    public static void main(String[] args) {
        String theClassName = "DirSynchCopy";
        System.out.println("+++ Start class: " + theClassName);
        Date theDate = new Date();
        System.out.println("+ Date Today: " + formatter.format(theDate));
        if (args.length != 2) {
            System.out.println("+ Syntax: "+theClassName+" <from directory> <to directory>");
            System.out.println("+ Synch the <from directory> to the <to directory>:");
            System.out.println("+    1. Copy files from the <from directory> that donot exist in the <to directory>.");
            System.out.println("+    2. Replace files in the <to directory> that have a different modified-date from the <from directory> file.");
            System.out.println("+    3. Remove files from the <to directory>, that donot exist in the <from directory>.");
            System.out.println("+ Upon completion, the <to directory> was made the same as the <from directory>.");
            return;
        }
        int argCounter = 0;
        String theDirectoryNameFrom = args[argCounter++].replace('\\', '/');
        if (theDirectoryNameFrom.endsWith("/")) {
            // System.out.print("\n+ theDirectoryNameFrom :"+theDirectoryNameFrom);
            theDirectoryNameFrom = theDirectoryNameFrom.substring(0, theDirectoryNameFrom.length()-1);
            // System.out.println(":"+theDirectoryNameFrom+".");
        }
        File dirFrom = new File(theDirectoryNameFrom);
        if (!dirFrom.isDirectory()) {
            System.out.println("--- datasynch, Error: the <from directory> is NOT a directory:" + theDirectoryNameFrom);
            System.out.println("\n+++ Exit class: "+theClassName+".");
            return;
        }
        String theDirectoryNameTo = args[argCounter++].replace('\\', '/');
        if (theDirectoryNameTo.endsWith("/")) {
            // System.out.print("\n+ theDirectoryNameTo :"+theDirectoryNameTo);
            theDirectoryNameTo = theDirectoryNameTo.substring(0, theDirectoryNameTo.length()-1);
            // System.out.println(":"+theDirectoryNameTo+".");
        }
        File dirTo = new File(theDirectoryNameTo);
        if (!dirTo.isDirectory()) {
            System.out.println("--- datasynch, Error: the <to directory> is NOT a directory:" + theDirectoryNameTo);
            System.out.println("\n+++ Exit class: "+theClassName+".");
            return;
        }
        System.out.print("++ Make:");
        System.out.print(" the <to directory> (" + theDirectoryNameTo + ")");
        System.out.println(" the same as the <from directory> ("+theDirectoryNameFrom+").\n");
        doSynch(theDirectoryNameFrom, dirFrom, theDirectoryNameTo, dirTo);
        if (countDeletes+countCopies==0) {
           System.out.println("\n+ Directories are the same, no changes.");
        }
        else {
           System.out.println("\n+ Number of copies = "+countCopies+", Number of deletes = "+countDeletes);
        }
        System.out.println("+++ Exit class: "+theClassName+".");
    }
}

// eof
