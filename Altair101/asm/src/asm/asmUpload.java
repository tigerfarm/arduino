/*
    Upload binary files to the Altair 101 through a serial port.
 */
package asm;

import com.fazecast.jSerialComm.SerialPort;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode processing.
public class asmUpload {

    private static final String SERIALPORT = "/dev/tty.SLAB_USBtoUART";

    // -------------------------------------------------------------------------
    // To view serial ports:
    //  $ ls /dev/tty.*
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

    public static void sendFile(String theReadFilename) {
        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        
        SerialPort sp = SerialPort.getCommPort(SERIALPORT);
        // Connection settings must match Arduino program settings.
        // Baud rate, data bits, stop bits, and parity
        sp.setComPortParameters(9600, 8, 1, 0);
        // block until bytes can be written
        sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0);
        if (!sp.openPort()) {
            System.out.println("- Error, failed to open serial port: " + SERIALPORT);
            return;
        }
        System.out.println("+ Port is open.");
        // System.out.println("++ Write out binary file: " + theReadFilename);
        int theLength = 0;
        byte bArray[] = null;
        try {
            // File theFile = new File("/Users/dthurston/Projects/arduino/Altair101/asm/10000000.bin");
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
                System.out.print(byteToString(bArray[i]) + " ");
                sp.getOutputStream().write(bArray[i]);
                sp.getOutputStream().flush();
            }
        } catch (IOException ex) {
            Logger.getLogger(asm.class.getName()).log(Level.SEVERE, null, ex);
        }
        // ---------------------------------------------------------------------
        if (sp.closePort()) {
            System.out.println("+ Serial port is closed.");
        } else {
            System.out.println("- Error: Failed to close serial port.");
        }
        //
        System.out.println("\n+ Write completed.");
    }

    // -------------------------------------------------------------------------
    // Constructor to ...
    public asmUpload() {
        System.out.println("+ asmUpload() constructor");
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) {

        System.out.println("+++ Start.");

        String outFilename = "10000000.bin";
        System.out.println("+ Write to the serail port, the program file: " + outFilename + ":");
        sendFile(outFilename);

        System.out.println("\n+++ Exit.\n");
    }
}
