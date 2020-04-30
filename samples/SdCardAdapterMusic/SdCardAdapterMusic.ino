// -----------------------------------------------------------------------------
/*
  Use the SimpleSDAudio library for audio playback.
    Use an SD-Card connected to an Arduino SPI pins.
    Play /EXAMPLE.AFM (Fullrate, mono, root directory) from an SD card.

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
  SdPlay.init flags for the above,
    SSDA_MODE_HALFRATE | SSDA_MODE_MONO
  Sox, SoX, the Swiss Army knife of sound processing programs.

  SoX is a cross-platform (Windows, Linux, MacOS X, etc.) command line utility.
  Can convert various formats of computer audio files in to other formats.
    http://sox.sourceforge.net/
  Mono
    sox inputfile.wav --norm=-1 -e unsigned-integer -b 8 -r 31250 -c 1 -t raw outputfile.raw
  Stereo
    sox inputfile.wav --norm=-1 -e unsigned-integer -b 8 -r 31250 -c 2 -t raw outputfile.raw
  Other
    For full rate use -r 62500 @ 16MHz, -r 31250 @ 8 MHz
    For half rate use -r 31250 @ 16MHz, -r 15625 @ 8 MHz
    The option --norm=-1 is used to avoid bad sounding clipping effects.
    
  SdPlay.play();
    If audio is not playing, playing will be started. If it is already playing, it will start playing from zero again.
  stop()
    Stops playback if playing, sets playposition to zero.
  pause()
    Pauses playing if not playing, resumes playing if was paused.
  Status,
    isStopped(), isPlaying(), isPaused()
*/
// -----------------------------------------------------------------------------
#include <SimpleSDAudio.h>

// -----------------------------------------------------------------------------
void setup() {
    // Enable the SPI SD card CS-Pin.
    SdPlay.setSDCSPin(10);
    /*
     Sample rate setting flags
        SSDA_MODE_FULLRATE - 62.500 kHz @ 16 MHz, 31.250 kHz @ 8 MHz
        SSDA_MODE_HALFRATE - 31.250 kHz @ 16 MHz, 15.625 kHz @ 8 MHz
     Output channel configuration flags
        SSDA_MODE_MONO        - Use only 1st PWM pin
        SSDA_MODE_STEREO      - Use both PWM pins for stereo output or mono 16 bit
     Other
        SSDA_MODE_AUTOWORKER - Add this flag and you don't need to call worker for playback.
     */
    if (!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER))
        { while(1); }
    //
    // Select audio file by providing the filename.
    if(!SdPlay.setFile("EXAMPLE.AFM")) // File name such as: ST01.WAV.
        { while(1);}
}
// -----------------------------------------------------------------------------
void loop(void) {
    Serial.println("+ Start play.");
    SdPlay.play(); // play the file
    while(!SdPlay.isStopped()) { ; }
    Serial.println("+ End play. Wait 3 seconds before playing again.");
    delay("3000");
}
// -----------------------------------------------------------------------------
