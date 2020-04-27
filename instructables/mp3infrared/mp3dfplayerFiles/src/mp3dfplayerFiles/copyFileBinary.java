package mp3dfplayerFiles;

import java.io.*;

public class copyFileBinary {

    public static void main(String[] args) {
        String theClassName = "copyFileBinary";
        System.out.println("+++ Start class: " + theClassName);
        if (args.length != 2) {
            System.out.println("+ Syntax:  copyFileBinary <file from> <file to>");
            System.out.println("+ Example: java -cp dist/fileio.jar fileio.copyFileBinary /opt/data/copyBinary.jpg /opt/data/copyBinaryOut.jpg");
            return;
        }
        String theFileNameFrom = args[0];
        String theFileNameTo = args[1];
        System.out.println("+ theFileNameFrom:" + theFileNameFrom);
        System.out.println("+ theFileNameTo:  " + theFileNameTo);

        File dirFrom = new File(theFileNameFrom);
        File dirTo = new File(theFileNameTo);
        try {
            InputStream in = new FileInputStream(dirFrom);
            OutputStream out = new FileOutputStream(dirTo);
            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            in.close();
            out.close();
        } catch (Exception e) {
            System.out.println("-- Error, readWriteCopy: " + e);
            System.exit(1);
        }

        System.out.println("\n+++ Exit class: " + theClassName + ".");
    }

}
