
//============================================================================================//
/*
  Filename: CSE_CST328.cpp
  Description: Main source file for the CSE_CST328 Arduino library.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.1
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_CST328
  Last Modified: +05:30 16:37:45 PM 16-02-2025, Sunday
 */
//============================================================================================//

#include "CSE_CST328.h"

//============================================================================================//
/*!
  @brief  Constructor for a point with no arguments
*/
TS_Point::TS_Point (void) {
  x = 0;
  y = 0;
  z = 0;
  touchId = 0;
  state = 0;
}

//============================================================================================//
/*!
  @brief  Constructor for a point with arguments for x, y, and z
  @param  x  X coordinate
  @param  y  Y coordinate
  @param  z  Z coordinate (often used for pressure)
*/
TS_Point::TS_Point (int16_t x, int16_t y, int16_t z, uint8_t id) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->touchId = id;
  state = 0;
}

//============================================================================================//
/*!
  @brief  Compare two points for equality
  @param  p  Point to compare
  @returns  True if points are equal, False if not
*/
bool TS_Point:: operator== (TS_Point p) {
  return ((p.x == x) && (p.y == y) && (p.z == z) && (p.touchId == touchId));
}

//============================================================================================//
/*!
  @brief  Compare two points for inequality
  @param  p  Point to compare
  @returns  True if points are not equal, False if they are
*/
bool TS_Point:: operator!= (TS_Point p) {
  return ((p.x != x) || (p.y != y) || (p.z != z) || (p.touchId != touchId));
}

//============================================================================================//
/*!
  @brief  Constructor for CSE_CST328 touch controller
  @param  width   Width of the touch screen
  @param  height  Height of the touch screen
  @param  i2c     Pointer to I2C bus
  @param  pinRst  Reset pin (default -1 for no hardware reset)
*/
CSE_CST328:: CSE_CST328 (uint16_t width, uint16_t height, TwoWire *i2c, int8_t pinRst, int8_t pinIrq) {
  wireInstance = i2c;
  pinReset = pinRst;
  pinInterrupt = pinIrq;
  
  // Store the default width and height
  defWidth = width;
  defHeight = height;
  
  // Set the current width and height (will be adjusted for rotation)
  this->width = width;
  this->height = height;

  for (int i = 0; i < 5; i++) {
    touchPoints [i] = TS_Point (0, 0, 0, i+1);
  }
  
  // Initialize other variables
  rotation = 0;
}

//============================================================================================//
/*!
  @brief  Initialize the CST328 touch controller
  @returns  True if initialization successful, otherwise false
 */
bool CSE_CST328:: begin() {
  // Initialize I2C if not already done
  wireInstance->begin();
  
  // If reset pin is defined, toggle it to reset the controller.
  if (pinReset != -1) {
    pinMode (pinReset, OUTPUT);
    digitalWrite (pinReset, HIGH);
    delay (10);
    digitalWrite (pinReset, LOW);
    delay (10);
    digitalWrite (pinReset, HIGH);
    delay (100); // Wait for chip to initialize (TRON = 200ms from datasheet)
  }

  // The chip can take a few tries to read the info.
  for (int i = 0; i < 3; i++) {
    // Enable debug mode to read the chip information.
    // In normal mode, reading the chip info will not work.
    write16 (MODE_DEBUG_INFO_REG);

    // Read the firmware checksum. Shoule be 0xCACAxxxx.
    uint32_t id = readRegister32 (CST328_INFO_3_REG);
    // The high bytes 3 and 2 should be 0xCACA. Extract the bytes.
    uint32_t fw_vc = (id >> 16) & 0xFFFF;
    
    if (fw_vc == 0xCACA) {
      Serial.println ("CST328 is found on the bus.");
      // Serial.println (id, HEX);
      break;
    }
    else {
      if (i == 2) {
        Serial.println ("CST328 not found on the bus.");
        return false;
      }
    }
  }

  // Put the device in normal reporting mode
  write16 (MODE_NORMAL_REG);
  
  return true;
}

//============================================================================================//
/*!
  @brief  Read the touch data from the controller
*/
void CSE_CST328:: readData() {
  uint8_t data [27]; // Buffer for all touch registers (0xD000-0xD01A)
  
  // Read all data registers at once for efficiency
  wireInstance->beginTransmission (CTS328_SLAVE_ADDRESS);
  wireInstance->write (0xD0);
  wireInstance->write (0x00);
  wireInstance->endTransmission (false);
  
  wireInstance->requestFrom (CTS328_SLAVE_ADDRESS, 27);
  
  uint8_t i = 0;

  for (i = 0; i < 27; i++) {
    if (wireInstance->available()) {
      data [i] = wireInstance->read();
    }
    else {
      break;
    }
  }

  if (i < 27) {
    Serial.println ("readData [WARNING]: Not all registers were read.");
  }
  
  // touches = data [5] & 0x0F; // Get number of touches reported

  // // Touch 1
  // int dataIndex = 0;
  // touchEvent [0] = data [dataIndex] & 0x0F; // The type of touch
  // touchID [0] = (data [dataIndex] >> 4) & 0x0F;
  // touchX [0] = (data [dataIndex + 1] << 4) | ((data [dataIndex + 3] >> 4) & 0x0F); // Calculate X coordinate (combining high and low bits)
  // touchY [0] = (data [dataIndex + 2] << 4) | (data [dataIndex + 3] & 0x0F); // Calculate Y coordinate (combining high and low bits)
  // touchWeight [0] = data [dataIndex + 4]; // Touch weight/pressure
  
  // Touch 1
  int dataIndex = 0;
  touchPoints [0].state = ((data [dataIndex] & 0x0F) == 6) ? 1 : 0;
  touchPoints [0].x = (data [dataIndex + 1] << 4) | ((data [dataIndex + 3] >> 4) & 0x0F); // Calculate X coordinate (combining high and low bits)
  touchPoints [0].y = (data [dataIndex + 2] << 4) | (data [dataIndex + 3] & 0x0F); // Calculate Y coordinate (combining high and low bits)
  touchPoints [0].z = data [dataIndex + 4]; // Touch weight/pressure

  // There is a two byte (0xD005 and 0xD006) gap between finger 1 and 2. The rest of the data is contiguous.
  // So we will use a loop for Touch 2~5.
  for (int i = 1, dataIndex = 7; dataIndex < 27; i++) { 
    touchPoints [i].state = ((data [dataIndex] & 0x0F) == 6) ? 1 : 0;
    touchPoints [i].x = (data [dataIndex + 1] << 4) | ((data [dataIndex + 3] >> 4) & 0x0F); // Calculate X coordinate (combining high and low bits)
    touchPoints [i].y = (data [dataIndex + 2] << 4) | (data [dataIndex + 3] & 0x0F); // Calculate Y coordinate (combining high and low bits)
    touchPoints [i].z = data [dataIndex + 4]; // Touch weight/pressure
    dataIndex += 5; // Increment by 5 to move to next finger.
  }
  
  // // Apply rotation if necessary
  // for (uint8_t i = 0; i < 2; i++) {
  //   switch (rotation) {
  //     case 0: // Default orientation
  //       break;
  //     case 1: // 90 degrees clockwise
  //       {
  //         int16_t temp = touchY[i];
  //         touchY[i] = width - touchX[i] - 1;
  //         touchX[i] = temp;
  //       }
  //       break;
  //     case 2: // 180 degrees
  //       touchX[i] = width - touchX[i] - 1;
  //       touchY[i] = height - touchY[i] - 1;
  //       break;
  //     case 3: // 270 degrees clockwise
  //       {
  //         int16_t temp = touchY[i];
  //         touchY[i] = touchX[i];
  //         touchX[i] = height - temp - 1;
  //       }
  //       break;
  //   }
  // }
}

//============================================================================================//
/*!
  @brief  Get the number of touches detected
  @returns  Number of touches (0-5)
*/
uint8_t CSE_CST328:: getTouches() {
  readData();

  uint8_t touches = 0;

  for (uint8_t i = 0; i < 5; i++) {
    if (touchPoints [i].state == 1) {
      touches++;
    }
  }

  return touches;
}

//============================================================================================//
/*!
  @brief  Check if the screen is being touched
  @returns  True if touched, false if not
*/
bool CSE_CST328:: isTouched() {
  return (getTouches() > 0);
}

//============================================================================================//
/*!
  @brief  Get the coordinates of a touch point
  @param  n  Touch point to get (0 or 1, default 0)
  @returns  TS_Point object with x, y, and z coordinates
*/
TS_Point CSE_CST328:: getPoint (uint8_t n) {
  return touchPoints [n];
}

//============================================================================================//
/*!
  @brief  Set the rotation of the touch panel
  @param  r  Rotation (0-3, where 0=0°, 1=90°, 2=180°, 3=270°)
  @returns  Current rotation setting
*/
uint8_t CSE_CST328:: setRotation (uint8_t r) {
  rotation = r % 4; // Ensure rotation is 0-3
  
  // Update width and height based on rotation
  switch (rotation) {
    case 0:
    case 2:
      width = defWidth;
      height = defHeight;
      break;
    case 1:
    case 3:
      width = defHeight;
      height = defWidth;
      break;
  }
  
  return rotation;
}

//============================================================================================//
/*!
  @brief  Get the current rotation setting
  @returns  Current rotation (0-3)
*/
uint8_t CSE_CST328:: getRotation() {
  return rotation;
}

//============================================================================================//
/*!
  @brief  Get the current width considering rotation
  @returns  Width in pixels
*/
uint16_t CSE_CST328:: getWidth() {
  return width;
}

//============================================================================================//
/*!
  @brief  Get the current height considering rotation
  @returns  Height in pixels
*/
uint16_t CSE_CST328:: getHeight() {
  return height;
}

//============================================================================================//
/*!
  @brief  Write 8 bits to a register
  @param  reg  Register address
  @param  val  Value to write
*/
void CSE_CST328:: writeRegister8 (uint16_t reg, uint8_t val) {
  wireInstance->beginTransmission (CTS328_SLAVE_ADDRESS);
  wireInstance->write (byte (reg >> 8));
  wireInstance->write (byte (reg & 0xFF));
  wireInstance->write (byte (val));
  wireInstance->endTransmission();
}

//============================================================================================//

void CSE_CST328:: write16 (uint16_t reg) {
  wireInstance->beginTransmission (CTS328_SLAVE_ADDRESS);
  wireInstance->write (byte (reg >> 8));
  wireInstance->write (byte (reg & 0xFF));
  wireInstance->endTransmission();
}

//============================================================================================//
/*!
@brief  Read 8 bits from a register
@param  reg  16-bit register address
@returns  Value read from register
*/
uint8_t CSE_CST328:: readRegister8 (uint16_t reg) {
  uint8_t value;

  wireInstance->beginTransmission (CTS328_SLAVE_ADDRESS);
  // Write high byte first
  wireInstance->write (byte (reg >> 8));
  wireInstance->write (byte (reg & 0xFF));
  wireInstance->endTransmission();

  wireInstance->requestFrom (byte (CTS328_SLAVE_ADDRESS), byte (1));

  if (wireInstance->available()) {
    value = wireInstance->read();
  }

  return value;
}

//============================================================================================//

uint32_t CSE_CST328:: readRegister32 (uint16_t reg) {
  uint32_t value;
  uint8_t buffer [4] = {0, 0, 0, 0};

  wireInstance->beginTransmission (CTS328_SLAVE_ADDRESS);
  // Write high byte first
  wireInstance->write (byte (reg >> 8));
  wireInstance->write (byte (reg & 0xFF));
  wireInstance->endTransmission();

  wireInstance->requestFrom (byte (CTS328_SLAVE_ADDRESS), byte (4));

  wireInstance->readBytes (buffer, 4);
  value = (uint32_t) buffer [0] | (uint32_t) buffer [1] << 8 | (uint32_t) buffer [2] << 16 | (uint32_t) buffer [3] << 24;

  // if (wireInstance->available()) {
  //   value = wireInstance->read();
  //   value |= (uint32_t) wireInstance->read() << 8;
  //   value |= (uint32_t) wireInstance->read() << 16;
  //   value |= (uint32_t) wireInstance->read() << 24;
  // }

  return value;
}

//============================================================================================//