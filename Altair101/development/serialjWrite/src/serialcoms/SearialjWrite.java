/*
    For serial communications between my computer and the arduino of the Altair 101.
    Intension, is to use this to upload binary programs to run on the 101.

    The following is base of the video:
        https://www.youtube.com/watch?v=BdzzyEuUWYk
    Documentation:
        https://www.mschoeffler.de/2017/12/29/tutorial-serial-connection-between-java-application-and-arduino-uno/
    Library:
        I downloaded the JAR file, jSerialComm-2.5.3.jar, and added it to my project.
            http://fazecast.github.io/jSerialComm/
    Library Features:
        + Access standard serial ports.
        + Built for ease-of-use, supports timeouts, and abile to open multiple ports simultaneously.
        + Enumerates all available serial ports on a machine
        ++ Returns both a system port description and a friendly device description
        ++ User-specifiable port descriptors including symbolic links
        + Abile to read and write byte streams via Java's InputStream and OutputStream interfaces
    Samples:
        https://github.com/Fazecast/jSerialComm/wiki/Usage-Examples
    Java docs:
        http://fazecast.github.io/jSerialComm/javadoc/com/fazecast/jSerialComm/package-summary.html

  Strings for testing,
 abcdefghijklmnopqrstuvwxyz
          1         2         3         4         5         6         7         8         9         0         1         2         3
01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
00a0b000c0000000d000000000000000e0000000000000000000000000000000f000000000000000000000000000000000000000000000000000000000000000g00
 */
package serialcoms;

import java.io.IOException;
import com.fazecast.jSerialComm.SerialPort;
import java.io.File;
import java.io.FileInputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

public class SearialjWrite {

    // -------------------------------------------------------------------------
    public static String byteToString(byte aByte) {
        return toBinary(aByte, 8);
    }
    private static String toBinary(byte a, int bits) {
        if (--bits > 0) {
            return toBinary((byte) (a >> 1), bits) + ((a & 0x1) == 0 ? "0" : "1");
        } else {
            return (a & 0x1) == 0 ? "0" : "1";
        }
    }
    public static void sendFile(SerialPort sp, String theReadFilename) {
        // System.out.println("++ Write out binary file: " + theReadFilename);
        int theLength = 0;
        byte bArray[] = null;
        try {
            File theFile = new File(theReadFilename);
            theLength = (int) theFile.length();
            bArray = new byte[(int) theLength];
            FileInputStream in = new FileInputStream(theReadFilename);
            in.read(bArray);
            in.close();
        } catch (IOException ioe) {
            System.out.print("IOException: ");
            System.out.println(ioe.toString());
        }
        System.out.println("+ Write to serial port. Number of bytes: " + theLength + " in the file: " + theReadFilename);
        //
        Integer i;
        int tenCount = 0;
        try {
            for (i = 0; i < theLength; i++) {
                if (tenCount == 10) {
                    tenCount = 0;
                    System.out.println("");
                }
                tenCount++;
                // Print binary formatted output for viewing with Examine/Next.
                System.out.print(byteToString(bArray[i]) + " ");
                // Hex: System.out.print(String.format("%02X ", bArray[i]));
                sp.getOutputStream().write(i.byteValue());
                sp.getOutputStream().flush();
            }
        } catch (IOException ex) {
            Logger.getLogger(SearialjWrite.class.getName()).log(Level.SEVERE, null, ex);
        }
        //
        System.out.println("\n+ Write completed.");
    }

    public static void main(String[] args) throws IOException, InterruptedException {

        System.out.println("+++ Start.");

        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        //SerialPort sp = SerialPort.getCommPort("/dev/cu.usbmodem14120");
        SerialPort sp = SerialPort.getCommPort("/dev/cu.wchusbserial14120");
        // Connection settings must match Arduino program settings.
        // Baud rate, data bits, stop bits, and parity
        sp.setComPortParameters(9600, 8, 1, 0);
        // block until bytes can be written
        sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0);
        if (!sp.openPort()) {
            System.out.println("- Error, failed to open port.");
            return;
        }
        System.out.println("+ Port is open.");

        // ---------------------------------------------------------------------       
        System.out.println("+ Send bytes.");

        sendFile(sp, "/Users/dthurston/Projects/arduino/Altair101/asm/10000000.bin");
        // programs/pLoop.asm >> 10000000.bin:
        //  11000011 00000110 00000000 00000000 00000000 00000000 11000011 00000000 00000000
        /*
        for (Integer i = 0; i < 5; ++i) {
            sp.getOutputStream().write(i.byteValue());
            sp.getOutputStream().flush();
            System.out.println("++ Byte sent: " + i);
            // Thread.sleep(1000);
        }
        */
        
        // ---------------------------------------------------------------------
        if (sp.closePort()) {
            System.out.println("+ Port is closed.");
        } else {
            System.out.println("- Error: Failed to close port.");
        }

        System.out.println("+++ Exit.");
    }

}
