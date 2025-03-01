
//============================================================================================//
/*
  Filename: CSE_CST328.h
  Description: Main header file for the CSE_CST328 Arduino library.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.0.3
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_CST328
  Last Modified: +05:30 15:31:36 PM 01-03-2025, Saturday
 */
//============================================================================================//

#ifndef CSE_CST328_H // CSE_CST328_LIBRARY
#define CSE_CST328_H

#include "Arduino.h"
#include <Wire.h>
#include "CSE_CST328_Constants.h"
#include "CSE_Touch.h"

#define DEBUG_SERIAL Serial  // Select the serial port to use for debug output

//============================================================================================//
/*!
  @brief  Class that stores state and functions for interacting with the CST328
  capacitive touch controller.
*/
class CSE_CST328 {
  public:
    uint16_t defWidth, defHeight; // Default width and height of the touch screen
    uint16_t width, height; // The size of the touch screen
    uint8_t rotation;

    CSE_TouchPoint touchPoints [5];

    CSE_CST328 (uint16_t width, uint16_t height, TwoWire *i2c = &Wire, int8_t pinRst = -1, int8_t pinIrq = -1);
    
    bool begin();
    void readData (void);
    void fastReadData (uint8_t n = 0); // Reads only one touch point data at a time
    uint8_t getTouches();  // Returns the number of touches detected
    bool isTouched(); // Returns true if there are any touches detected
    bool isTouched (uint8_t id); // Returns true if there are any touches detected
    CSE_TouchPoint getPoint (uint8_t n = 0);  // By default, first touch point is returned
    uint8_t setRotation (uint8_t rotation = 0);  // Set the rotation of the touch panel (0-3)
    uint8_t getRotation();  // Set the rotation of the touch panel (0-3)
    uint16_t getWidth();
    uint16_t getHeight();
    
    // In CST328, all register addresses are 16 bits.
    void writeRegister8 (uint16_t reg, uint8_t val); // Write an 8 bit value to a register
    uint8_t readRegister8 (uint16_t reg);  // Read an 8 bit value from a register
    uint32_t readRegister32 (uint16_t reg);  // Read a 32 bit value from the registers
    void write16 (uint16_t reg);  // Write a 16 bit value to the registers. No values required.

  private:
    TwoWire *wireInstance; // Touch panel I2C
    int8_t pinReset;  // Touch panel reset pin
    int8_t pinInterrupt;  // Touch panel interrupt pin
    bool inited;

};

//============================================================================================//

#endif // CSE_CST328_LIBRARY
