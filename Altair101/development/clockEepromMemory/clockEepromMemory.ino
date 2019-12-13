// -----------------------------------------------------------------------------
/*
  Read and write to the 24C32 EEPROM of the DS3231 clock module.

  Has 32K bits of EEPROM, which is 4K, 4096 bytes.
    4096 / 256(Dev machine's memory size) = 16.
    16 complete memory writes.
    Or, have an index, and trim trailing "0" bytes.

  Clock Pins:
  + VCC to 3.3-5.5V
  + GND to ground
  + SDA to D4 on Uno and Nano
  + SCL to D5 on Uno and Nano
  
  + 32K to D4 on Uno and Nano ?

  Reference URL:
  https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/


    https://www.theengineeringprojects.com/2019/03/introduction-to-ds3231.html
    Pin#1	32K	This is a 32 kHz output.
                        This is an open drain pin requires an external pull up resistor.
                        If it is not in use can be left open.
    Pin#3	INT/SQW	This is a low interrupt or square wave output pin.
                        It requires an external pull up resistor.

*/

#include <Wire.h>

const int memoryDeviceAddress = 0x57;

// DS3232 Register Addresses
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x01
#define RTC_HOURS 0x02
#define RTC_DAY 0x03
#define RTC_DATE 0x04
#define RTC_MONTH 0x05
#define RTC_YEAR 0x06
#define ALM1_SECONDS 0x07
#define ALM1_MINUTES 0x08
#define ALM1_HOURS 0x09
#define ALM1_DAYDATE 0x0A
#define ALM2_MINUTES 0x0B
#define ALM2_HOURS 0x0C
#define ALM2_DAYDATE 0x0D
#define RTC_CONTROL 0x0E
#define RTC_STATUS 0x0F
#define RTC_AGING 0x10
#define RTC_TEMP_MSB 0x11
#define RTC_TEMP_LSB 0x12
#define SRAM_START_ADDR 0x14    // first SRAM address
#define SRAM_SIZE 236           // number of bytes of SRAM

// -----------------------------------------------------------------------------
void writeMemoryByte(unsigned int eeaddress, byte data) {
    int rdata = data;
    Wire.beginTransmission(memoryDeviceAddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(rdata);
    Wire.endTransmission();
}

// Note, address is a page address, 6-bit end will wrap around.
// Also, data can be a maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes.
void writeMemoryPage(unsigned int eeaddresspage, byte* data, byte length) {
    Wire.beginTransmission(memoryDeviceAddress);
    Wire.write((int)(eeaddresspage >> 8));      // MSB
    Wire.write((int)(eeaddresspage & 0xFF));    // LSB
    byte c;
    for ( c = 0; c < length; c++) {
        Wire.write(data[c]);
    }
    Wire.endTransmission();
}

// -----------------------------------------------------------------------------
byte readMemoryByte(unsigned int eeaddress) {
    byte rdata = 0xFF;
    Wire.beginTransmission(memoryDeviceAddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) {
        rdata = Wire.read();
    }
    return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void readMemoryBuffer(unsigned int eeaddress, byte *buffer, int length ) {
    Wire.beginTransmission(memoryDeviceAddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,length);
    int c = 0;
    for ( c = 0; c < length; c++ ) {
        if (Wire.available()) buffer[c] = Wire.read();
    }
}

// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
    Wire.begin(); // initialize the connection

    Serial.print("+ Write data into memory: ");
    char theData[] = "lastminuteengineers.com";
    Serial.println(theData);
    writeMemoryPage(0, (byte *)theData, sizeof(theData));
    delay(100);     // Allow the write to complete.
    Serial.println("+ Data written to memory.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
void loop()
{
    Serial.print("+ Read and echo memory from the first address.");
    int addr=0;
    byte b = readMemoryByte(0);
    while (b!=0) {
        Serial.print((char)b);
        addr++;
        b = readMemoryByte(addr);
    }
    Serial.println("+ End of read.");
    delay(3000);
}
// -----------------------------------------------------------------------------
