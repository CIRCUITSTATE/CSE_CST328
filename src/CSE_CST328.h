
//============================================================================================//
/*
  Filename: CSE_CST328.h
  Description: Main header file for the CSE_CST328 Arduino library.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.1
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_CST328
  Last Modified: +05:30 16:37:41 PM 16-02-2025, Sunday
 */
//============================================================================================//

#ifndef CSE_CST328_H
#define CSE_CST328_H

#include "Arduino.h"
#include <Wire.h>
#include "CSE_CST328_Constants.h"

#define debugSerial Serial  // Select the serial port to use for debug output

//============================================================================================//
/*!
  @brief  Helper class that stores a TouchScreen Point with x, y, and z
  coordinates, for easy math/comparison.
*/
class TS_Point {
  public:
    TS_Point (void);
    TS_Point (int16_t x, int16_t y, int16_t z, uint8_t id);

    bool operator== (TS_Point);
    bool operator!= (TS_Point);

    uint8_t touchId;
    int16_t x; // X coordinate
    int16_t y; // Y coordinate
    int16_t z; // Z coordinate (often used for pressure)
    int16_t state; // State
};

//============================================================================================//
/*!
  @brief  Class that stores state and functions for interacting with FT6206
  capacitive touch chips.
*/
class CSE_CST328 {
  public:
    // These values are common to both P1 and P2 touch points
    // uint8_t touches;  // Number of touches registered
    // uint8_t gestureID;  // The gesture ID of the touch
    uint16_t defWidth, defHeight; // Default width and height of the touch screen
    uint16_t width, height; // The size of the touch screen

    // FT6206 can detect two touch points, P1 and P2 at the same time.
    // So have two sets of values for each point.
    // uint8_t touchEvent [2];  // The type of touch
    // uint16_t touchX [2], touchY [2];
    // uint16_t touchID [2];
    // uint8_t touchArea [2];
    // uint8_t touchWeight [2];

    TS_Point touchPoints [5];
    uint8_t rotation;

    CSE_CST328 (uint16_t width, uint16_t height, TwoWire *i2c = &Wire, int8_t pinRst = -1, int8_t pinIrq = -1);
    bool begin();
    void readData (void);
    uint8_t getTouches (void);  // Returns the number of touches detected
    bool isTouched (void); // Returns true if there are any touches detected
    TS_Point getPoint (uint8_t n = 0);  // By default, P1 touch point is returned
    uint8_t setRotation (uint8_t rotation = 0);  // Set the rotation of the touch panel (0-3
    uint8_t getRotation();  // Set the rotation of the touch panel (0-3
    uint16_t getWidth();
    uint16_t getHeight();

    void writeRegister8 (uint16_t reg, uint8_t val); // Write an 8 bit value to a register
    uint8_t readRegister8 (uint16_t reg);  // Read an 8 bit value from a register
    uint32_t readRegister32 (uint16_t reg);  // Read a 16 bit value from a register
    void write16 (uint16_t reg);

  private:
    TwoWire *wireInstance; // Touch panel I2C
    int8_t pinReset;  // Touch panel reset pin
    int8_t pinInterrupt;  // Touch panel reset pin
};

//============================================================================================//

#endif // CSE_FT6206_LIBRARY
