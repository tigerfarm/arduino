/*

    Library examples:
        https://github.com/Fazecast/jSerialComm/wiki/Usage-Examples
    Reference,
        https://github.com/Fazecast/jSerialComm/wiki/Blocking-and-Semiblocking-Reading-Usage-Example

    Another serial Java library, looks pretty good:
        https://code.google.com/archive/p/java-simple-serial-connector/

    https://xtermjs.org/
    https://www.electricimp.com/secure-web-based-serial-terminal/

 */
package serialjread;

import com.fazecast.jSerialComm.SerialPort;

public class SerialjRead {

    public static void main(String[] args) {
        System.out.println("+++ Start.");

        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        SerialPort sp = SerialPort.getCommPort("/dev/cu.usbmodem14121");
        // Connection settings must match Arduino program settings.
        // Baud rate, data bits, stop bits, and parity
        sp.setComPortParameters(9600, 8, 1, 0);
        
        // Block until ready.
        sp.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 100, 0);  // READ/100 or WRITE/0.
        if (!sp.openPort()) {
            System.out.println("- Error, failed to open port.");
            return;
        }
        
        System.out.println("+ Port is open, ready to receive bytes.");
        // ...
        try {
            while (true) {
                while (sp.bytesAvailable() == 0) {
                    Thread.sleep(20);
                }
                byte[] readBuffer = new byte[sp.bytesAvailable()];
                int numRead = sp.readBytes(readBuffer, readBuffer.length);
                System.out.println("+ Read " + numRead + " bytes.");
            }
        } catch (InterruptedException e) {
            System.out.println("- Read error: " + e.getMessage());
        }
        sp.closePort();
    }

}
