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

*/
#include <Wire.h>

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

void setup()
{
    char somedata[] = "lastminuteengineers.com"; // data to write
    Wire.begin(); // initialise the connection
    Serial.begin(9600);
    Serial.println("Writing into memory...");
  
    // write to EEPROM
    i2c_eeprom_write_page(0x57, 0, (byte *)somedata, sizeof(somedata));

    delay(100); //add a small delay
    Serial.println("Memory written");
}

void loop()
{
    Serial.print("Reading memory: ");
    int addr=0; //first address
    // access the first address from the memory
    byte b = i2c_eeprom_read_byte(0x57, 0);
    while (b!=0) {
        Serial.print((char)b); //print content to serial port
        addr++; //increase address
        b = i2c_eeprom_read_byte(0x57, addr); //access an address from the memory
    }
    Serial.println(" ");
    delay(2000);
}

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(rdata);
    Wire.endTransmission();
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) {
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddresspage >> 8)); // MSB
    Wire.write((int)(eeaddresspage & 0xFF)); // LSB
    byte c;
    for ( c = 0; c < length; c++)
        Wire.write(data[c]);
    Wire.endTransmission();
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length ) {
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,length);
    int c = 0;
    for ( c = 0; c < length; c++ )
        if (Wire.available()) buffer[c] = Wire.read();
}
