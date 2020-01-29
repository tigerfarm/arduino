--------------------------------------------------------------------------------
+++ Configure Mac with USB Serial drivers.

The drivers allow:
+ The Mac Arduino IDE to communicate with Arduino boards.
+ An Arduino serial port module to communicate with Mac apps such as my Java serial upload program.

--------------------------------------------------------------------------------
+++ CP2102 serial port module

CP2102 USB 2.0 to TTL UART Module 6Pin Serial Converter STC FT232 26.5mm*15.6mm
+ Download driver:https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU
++ http://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver_10_6.zip
++ I saved it as: CP2102-MacSerialDriver.zip.

After installing, requires a reboot.

--------------------------------------------------------------------------------
+++ CH340 USB serial driver.

How to steps from:
https://medium.com/@thuc/connect-arduino-nano-with-mac-osx-f922a46c0a5d

1. Download the driver from url: https://github.com/ledongthuc/example/blob/master/file/CH34x_Install_V1.3.zip
2. Extract and install file CH34x_Install.pkg

3. Run the command: sudo nvram boot-args="kext-dev-mode=1"
++ you need to be booted to Recovery to change

To use Recovery Mode, reboot or start your Mac and hold the Command and R keys simultaneously on your keyboard as soon as you hear the familiar startup chime. Keep holding as your Mac boots, which may take a few moments depending on its specific configuration. You can let go of the keys when you see a screen similar to the screenshot below.

Check the files in /dev/ with start name “tty*”, if you see /dev/tty.wch ch341 USB=>RS232 1420, it is correct.

How to run the command:
https://apple.stackexchange.com/questions/256138/trying-to-set-boot-args-with-nvram-gets-general-error-on-sierra

++ Add and remove an arg:
# nvram boot-args="serverperfmode=1 $(nvram boot-args 2>/dev/null | cut -f 2-)"
# nvram boot-args
	serverperfmode=1  
# sudo nvram boot-args="$(nvram boot-args 2>/dev/null | sed -e $'s/boot-args\t//;s/serverperfmode=1//')"
# nvram boot-args

4. Restart the Mac.

5. Replace the text -P{serial.port} to "-P{serial.port}" in 2 files:
+ /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/platform.txt
+ /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/programmers.txt

--------------------------------------------------------------------------------
