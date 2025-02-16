

//============================================================================================//
/*
  Filename: Read-Touch-Polling.ino
  Description: Example Arduino sketch from the CSE_CST328 Arduino library.
  Reads the touch sensor through polling method and prints the data to the serial monitor.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.1
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_CST328
  Last Modified: +05:30 19:54:55 PM 16-02-2025, Sunday
 */
//============================================================================================//

#include <Wire.h>
#include <CSE_CST328.h>

#define CST328_PIN_RST  4
#define CST328_PIN_INT  16
#define CST328_PIN_SDA  21
#define CST328_PIN_SCL  22

//===================================================================================//

// Create a new instance of the CST328 class and send the Wire instance,
// and the reset pin to the constructor.
// You can leave all of these parameters if you want to use the default values.
// The default values are: Wire, -1.
CSE_CST328 tsPanel = CSE_CST328 (240, 320, &Wire, CST328_PIN_RST, CST328_PIN_INT);

//===================================================================================//

void setup() {
  Serial.begin (115200);
  delay (100);
  Serial.println();
  Serial.println ("CST328 Touch Controller Test");

  // Set the I2C pins if your board allows it.
  // For RP2040
  // Wire.setSDA (CST328_PIN_SDA);
  // Wire.setSCL (CST328_PIN_SCL);

  // For ESP32.
  Wire.begin (CST328_PIN_SDA, CST328_PIN_SCL);

  // findI2CDevices();

  tsPanel.begin();
  delay (500);
}

//===================================================================================//

void loop() {
  readTouch();
  delay (500);
}

//===================================================================================//

void readTouch() {
  tsPanel.readData();

  uint8_t touches = tsPanel.getTouches();

  if (touches > 0) {
    for (uint8_t i = 0; i < touches; i++) {
      Serial.print ("Touch ID: ");
      Serial.print (i);
      Serial.print (", X: ");
      Serial.print (tsPanel.getPoint (i).x);
      Serial.print (", Y: ");
      Serial.print (tsPanel.getPoint (i).y);
      Serial.print (", Z: ");
      Serial.print (tsPanel.getPoint (i).z);
      Serial.print (", State: ");
      Serial.println (tsPanel.getPoint (i).state);
    }
  }
  else {
    Serial.println ("No touches detected");
  }
}

//===================================================================================//

void findI2CDevices() {
  uint8_t error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n"); 
  }
}

//===================================================================================//