/*
// -----------------------------------------------------------------------------
    Upload binary files to the Altair 101 through a serial port.

    To view serial ports on a Mac:
    $ ls /dev/tty.*

    Add another sleep option, which is sleep for LF/CR. This is for uploading to 4K Basic.

    Need to control/limit the listing to relavent ports, example contains ".cu".
+ List of serial ports:
++ cu.Bluetooth-Incoming-Port : Bluetooth-Incoming-Port BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
++ tty.Bluetooth-Incoming-Port : Bluetooth-Incoming-Port (Dial-In) BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
++ cu.wchusbserial14120 : USB2.0-Serial BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
++ tty.wchusbserial14120 : USB2.0-Serial (Dial-In) BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
++ cu.SLAB_USBtoUART : CP2102 USB to UART Bridge Controller BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
++ tty.SLAB_USBtoUART : CP2102 USB to UART Bridge Controller (Dial-In) BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
+ End of list.

    Find program files from asm:
    + List files in a program directory.
    + Set and get program directory value.

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
//
public class asmUpload {

    private static int baudRate = 115200;   // 57600
    // This allows buffer time so that bytes are not dropped.
    // Note, had issues with baud rate was 57600 and baudSleepTime was 3. 10 works fine for 57600.
    // Started testing with 115200. 10 worked fine.
    // Set to 20 if baud rate is 9600.
    private static int baudSleepTime = 10;
    private static int baudSleepTimeCr = 0;

    // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
    // Sample default ports: tty.wchusbserial14230 /dev/cu.wchusbserial141230 /dev/tty.SLAB_USBtoUART
    private static String SerialPortName = "/dev/tty.wchusbserial14220";

    // -------------------------------------------------------------------------
    // Constructor to ...
    public asmUpload() {
        System.out.println("+ asmUpload(), current Serial Port Name: " + asmUpload.SerialPortName);
    }

    public static int getBaudRate() {
        return asmUpload.baudRate;
    }

    public static void setBaudRate(int theBaudRate) {
        asmUpload.baudRate = theBaudRate;
    }

    public static int getBaudSleepTime() {
        return asmUpload.baudSleepTime;
    }

    public static void setBaudSleepTime(int theBaudSleepTime) {
        asmUpload.baudSleepTime = theBaudSleepTime;
    }

    public static int getBaudSleepTimeCr() {
        return asmUpload.baudSleepTimeCr;
    }

    public static void setBaudSleepTimeCr(int theBaudSleepTimeCr) {
        asmUpload.baudSleepTimeCr = theBaudSleepTimeCr;
    }

    public static String getSerialPortName() {
        return asmUpload.SerialPortName;
    }

    public static void setSerialPortName(String theSerialPortName) {
        SerialPort serials[] = SerialPort.getCommPorts();
        boolean IsFound = false;
        String theSystemPortName = "tty." + theSerialPortName;
        for (SerialPort serial : serials) {
            // System.out.println("+++ serial.getSystemPortName().toLowerCase(): " + serial.getSystemPortName().toLowerCase());
            if (theSystemPortName.toLowerCase().startsWith(serial.getSystemPortName().toLowerCase())) {
                // System.out.println("++ Found: " + theSerialPortName);
                IsFound = true;
                theSystemPortName = serial.getSystemPortName();
            }
        }
        if (!IsFound) {
            System.out.println("+ Serial port name not found: " + theSerialPortName);
            return;
        }
        asmUpload.SerialPortName = "/dev/" + theSystemPortName;
        System.out.println("+ Serial port set to: " + theSerialPortName);
        System.out.println("+ Serial port set to system name: " + asmUpload.SerialPortName);
    }

    public static void listSerialPorts() {
        System.out.println("+ List of available serial ports:");
        SerialPort serials[] = SerialPort.getCommPorts();
        for (SerialPort serial : serials) {
            if (serial.getSystemPortName().startsWith("tty") && !serial.getSystemPortName().startsWith("tty.Bluetooth")) {
                // ++ cu.Bluetooth-Incoming-Port : Bluetooth-Incoming-Port BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
                // ++ tty.Bluetooth-Incoming-Port : Bluetooth-Incoming-Port (Dial-In) BaudRate:9600 Data Bits:8 Stop Bits:1 Parity:0
                System.out.println(
                        "++ " + serial.getSystemPortName().substring(4)
                        + " : " + serial.getPortDescription()
                        + " BaudRate:" + serial.getBaudRate()
                        + " Data Bits:" + serial.getNumDataBits()
                        + " Stop Bits:" + serial.getNumStopBits()
                        + " Parity:" + serial.getParity()
                );
            }
        }
        System.out.println("+ End of list.");
    }

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

    public static void sendFile(String theReadFilename) {
        SerialPort sp = SerialPort.getCommPort(SerialPortName);
        // Connection settings must match Arduino program settings.
        // Baud rate, data bits, stop bits, and parity
        sp.setComPortParameters(baudRate, 8, 1, 0); // 9600 19200
        // block until bytes can be written
        sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0);
        if (!sp.openPort()) {
            System.out.println("- Error, failed to open serial port: " + SerialPortName);
            return;
        }
        System.out.println("+ Serial port is open.");
        //
        System.out.println("++ Load the program binary file: " + theReadFilename);
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
        Integer i;
        int tenCount = 0;
        try {
            for (i = 0; i < theLength; i++) {
                if (tenCount == 10) {
                    tenCount = 0;
                    System.out.println("");
                }
                tenCount++;
                byte theByte = bArray[i];
                System.out.print(byteToString(theByte) + " ");
                sp.getOutputStream().write(theByte);
                sp.getOutputStream().flush();
                Thread.sleep(baudSleepTime);
                if (theByte == 10 || theByte == 13) {
                    // Allow Basic time to process the newline/carriage return.
                    Thread.sleep(baudSleepTimeCr);
                }
            }
        } catch (IOException ex) {
            Logger.getLogger(asm.class.getName()).log(Level.SEVERE, null, ex);
        } catch (InterruptedException ex) {
            Logger.getLogger(asmUpload.class.getName()).log(Level.SEVERE, null, ex);
        }
        // ---------------------------------------------------------------------
        if (sp.closePort()) {
            System.out.println("\n+ Serial port is closed.");
        } else {
            System.out.println("- Error: Failed to close serial port.");
        }
        //
        System.out.println("\n+ Write completed.");
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) {
        System.out.println("+++ Start.");

        // asmUpload upload = new asmUpload();
        asmUpload.listSerialPorts();
        asmUpload.setSerialPortName("wchusbserial14220");  // tty.wchusbserial14220 tty.SLAB_USBtoUART
        System.out.println("------------");
        asmUpload.setBaudRate(9600);
        asmUpload.setBaudSleepTime(20);
        asmUpload.setBaudSleepTimeCr(0);

        // String outFilename = "10000000.bin";
        String outFilename = "poem.txt";
        //
        System.out.println("+ Write to the serial port, the program file: " + outFilename + ":");
        sendFile(outFilename);

        System.out.println("\n+++ Exit.\n");
    }
}
