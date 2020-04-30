// -----------------------------------------------------------------------------
/*
  Use the SimpleSDAudio library for audio playback.
    Use an SD-Card connected to an Arduino SPI pins.
    Play EXAMPLE.AFM on freshly formated SD card into root folder

  Instructable,
    https://www.instructables.com/id/Audio-Playback-From-SD-Card-With-Arduino/

  Arduino Audio pin for signal output,
    Arduino Nano or Uno: Audio-Pin 9
    Arduino Mega2560:    Audio-Pin 44
  Using passive speaker:
    Arduino                            Speaker
    Audio-Pin --- 100 Ohm resistor --- Positive
    GND ------------------------------ Ground
  Using amplifier / active speaker / line-in etc.:
    Audio-Pin --||--------------[10k resistor]----+----[1k resistor]---- GND
              100nF capacitor                   to amp

  This program is from:
    https://github.com/isramos/mico-shield/blob/master/libraries/SimpleSDAudio/examples/BareMinimum/BareMinimum.ino
  Library,
    http://www.hackerspace-ffm.de/wiki/index.php?title=SimpleSDAudio
    Download the zip file and place it in you Arduino zip file library directory.
    I downloaded: SimpleSDAudio_V1.03.zip.
    From the IDE menu: Sketch/Include Library/Add Zip library.
    In the popup, Select SimpleSDAudio_V1.03.zip, and click Choose.
    Optional install:
      Download from: https://github.com/isramos/mico-shield
      Copy the "SimpleSDAudio" folder into: C:\Program Files (x86)\Arduino\libraries 

  Program plays WAV files that are 8-bit 32000 Hz in stereo or mono (PCM).
  Audio file must be converted prior use
  Audio files must reside in root directory of card
  File name of audio file must be in 8.3-format

  Online convertion URL: https://audio.online-convert.com/convert-to-wav
    8 Bit
    32000 HZ
    mono, could use stereo

*/
// -----------------------------------------------------------------------------
#include <SimpleSDAudio.h>

// -----------------------------------------------------------------------------
void setup() {
    // Enable the SPI SD card CS-Pin.
    SdPlay.setSDCSPin(10);
    if (!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER))
        { while(1); }
    if(!SdPlay.setFile("EXAMPLE.AFM")) // File name such as: ST01.WAV.
        { while(1);}
}
// -----------------------------------------------------------------------------
void loop(void) {
    Serial.println("+ Start play.");
    SdPlay.play(); // play the file
    while(!SdPlay.isStopped()) { ; }
    Serial.println("End play. Wait 3 seconds before playing again.");
    delay("3000");
    
}
// -----------------------------------------------------------------------------
