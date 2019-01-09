/********
   A basic implementation of BME280.

   updated by Stacy David: https://github.com/tigerfarm/arduino/tree/master/samples/pongv2

   bme280_example.ino
   Example sketch for bme280

   Copyright (c) 2016 seeed technology inc.
   Website    : www.seeedstudio.com
   http://wiki.seeedstudio.com/Grove-Barometer_Sensor-BME280/
   Author     : Lambor
   The MIT License (MIT)
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
*/
#include "Seeed_BME280.h"
#include <Wire.h>

BME280 bme280;

void diplayValues() {
  /*
    + Temperature:          0.00 Celsius
    + Humidity:             0 %
    + Atmospheric pressure: 0 Pa
    + Altitude:             44330.76 m
  */
  Serial.print("+ Temperature:          ");
  Serial.print(bme280.getTemperature());
  Serial.println(" Celsius");
  //
  Serial.print("+ Humidity:             ");
  Serial.print(bme280.getHumidity());
  Serial.println( " %");
  //
  Serial.print("+ Atmospheric pressure: ");
  float pressure = bme280.getPressure();
  Serial.print(pressure);
  Serial.println(" Pa");
  //
  Serial.print("+ Altitude:             ");
  Serial.print(bme280.calcAltitude(pressure));
  Serial.println(" m");
}

void setup() {
  Serial.begin(9600);
  Serial.println("++ Setup BME280.");
  if (!bme280.init()) {
    Serial.println("Device error!");
  }
  Serial.println("--------------------------------");
  delay(3000);
}

void loop() {
  diplayValues();
  Serial.println("--------------------------------");
  delay(9000);
}

// -----------------------------------------------------------------------
// eof
