/*
    For serial communications between my computer and the arduino of the Altair 101.

    The following is base of the video:
        https://www.youtube.com/watch?v=BdzzyEuUWYk
    Documentation:
        https://www.mschoeffler.de/2017/12/29/tutorial-serial-connection-between-java-application-and-arduino-uno/
    Library:
        http://fazecast.github.io/jSerialComm/
        I downloaded the JAR file, jSerialComm-2.5.3.jar, and added it to my project.
 */
package serialcoms;

import java.io.IOException;
import com.fazecast.jSerialComm.SerialPort;

public class SerialComs {

    public static void main(String[] args) throws IOException, InterruptedException {

        System.out.println("+++ Start.");

        // Uses the device name that can be found in the Arduino IDE, under the menu item Tools/Port.
        SerialPort sp = SerialPort.getCommPort("/dev/cu.usbmodem14121"); // device name
        sp.setComPortParameters(9600, 8, 1, 0); // default connection settings for Arduino
        sp.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written
        if (sp.openPort()) {
            System.out.println("+ Port is open.");
        } else {
            System.out.println("- Error, failed to open port.");
            return;
        }
        System.out.println("+ Send bytes.");
        for (Integer i = 0; i < 5; ++i) {
            sp.getOutputStream().write(i.byteValue());
            sp.getOutputStream().flush();
            System.out.println("Sent number: " + i);
            Thread.sleep(1000);
        }
        if (sp.closePort()) {
            System.out.println("+ Port is closed.");
        } else {
            System.out.println("- Error: Failed to close port.(");
        }

        System.out.println("+++ Exit.");
    }

}
