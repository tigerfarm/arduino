// -----------------------------------------------------------------------------
/*
  Use the SimpleSDAudio library for audio playback.
    Use an SD-Card connected to an Arduino SPI pins.
    Play /EXAMPLE.AFM (Fullrate, mono, root directory) from an SD card.

  This works. But the volume is low, non-adjustable, which would work if I was using an amp.
  However, there is a hiss noice.

  Instructable,
    https://www.instructables.com/id/Audio-Playback-From-SD-Card-With-Arduino/

  Arduino Audio pin for signal output,
    Arduino Nano or Uno: Audio-Pin 9
    Arduino Mega2560:    Audio-Pin 44
  Using a single passive speaker:
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
  Install:
    Download the zip file and place it in you Arduino zip file library directory.
    I downloaded: SimpleSDAudio_V1.03.zip.
    From the IDE menu: Sketch/Include Library/Add Zip library.
    In the popup, Select SimpleSDAudio_V1.03.zip, and click Choose.
  Install, optional method:
    Download from: https://github.com/isramos/mico-shield
    Copy the "SimpleSDAudio" folder into: C:\Program Files (x86)\Arduino\libraries

  Program plays,
    WAV files that are 8-bit 32000 Hz in stereo or mono (PCM).
    Audio files in root directory of card
    8.3-formatted file names.
  Online convertion URL: https://audio.online-convert.com/convert-to-wav
    8 Bit
    32000 HZ
    mono, could use stereo
  SdPlay.init flags for the above,
    SSDA_MODE_HALFRATE | SSDA_MODE_MONO
  Commandline coversion program,
    Sox, SoX, the Swiss Army knife of sound processing programs.
    http://sox.sourceforge.net/
  I downloaded,
    https://github.com/macports/macports-base/releases/tag/v2.6.2

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

  Other libraries
  + Instructions:
  https://maker.pro/arduino/projects/arduino-audio-player
  + Instructions, another:
  https://www.youtube.com/watch?v=uSUZbLlRi1g, code: https://github.com/abhijitbrain/creative-research/blob/master/_1mp3.ino
  + Instructions, another:
  https://www.c-sharpcorner.com/article/audio-play-using-sd-card-module-and-arduino/

*/
// -----------------------------------------------------------------------------
#include <SimpleSDAudio.h>

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

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
  // SSDA_MODE_FULLRATE : For the EXMAMPLE.AFM
  // SSDA_MODE_HALFRATE : For WAV files converted using: 8 Bit, 32000 HZ, mono.
  if (!SdPlay.init(SSDA_MODE_HALFRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)) {
    while (1);
  }

  Serial.println(F("+++ Go to loop."));
}
// -----------------------------------------------------------------------------
void loop(void) {
  // Set to play the audio file, such as: ST01.WAV or EXAMPLE.AFM.
  if (!SdPlay.setFile("ST01.WAV")) {
    while (1);
  }
  Serial.println("+ Start play.");
  SdPlay.play(); // play the file
  while (!SdPlay.isStopped()) {
    ;
  }
  //
  Serial.println("+ End play. Wait 3 seconds before playing next.");
  delay(3000);
  //
  if (!SdPlay.setFile("LEAVESB.WAV")) {
    while (1);
  }
  Serial.println("+ Start play.");
  SdPlay.play();
  while (!SdPlay.isStopped()) {
    ;
  }
  //
  Serial.println("+ End play. Wait 3 seconds before playing next.");
  delay(3000);
  //
  if (!SdPlay.setFile("MVILLE.WAV")) {
    while (1);
  }
  Serial.println("+ Start play.");
  SdPlay.play();
  while (!SdPlay.isStopped()) {
    ;
  }
}
// -----------------------------------------------------------------------------
