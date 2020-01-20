/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package serialjread;

import com.fazecast.jSerialComm.SerialPort;

/**
 *
 * @author dthurston
 */
public class SerialjRead {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        System.out.println("+++ Start.");

        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        SerialPort sp = SerialPort.getCommPort("/dev/cu.usbmodem14121");
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

        System.out.println("+ Receive bytes.");

        if (sp.closePort()) {
            System.out.println("+ Port is closed.");
        } else {
            System.out.println("- Error: Failed to close port.(");
        }

        System.out.println("+++ Exit.");
    }
    
}
