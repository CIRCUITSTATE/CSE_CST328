

#
### **+05:30 12:43:17 PM 02-03-2025, Sunday**

  - Updated Readme.

#
### **+05:30 12:40:05 PM 02-03-2025, Sunday**

  - Updated documentation.

#
### **+05:30 03:33:06 PM 01-03-2025, Saturday**

  - Removed commented out code.

#
### **+05:30 03:21:25 PM 01-03-2025, Saturday**

  - Updated the library naming string in Arduino library properties.
  - Removed the `TS_Point` class.
    - This was redundant and was causing conflicts with the `CSE_TouchPoint` class.
    - This also caused the touch issue in the `UI_Test` example.
    - Replaced all `TS_Point` instances with `CSE_TouchPoint`.
    - We will now use the `CSE_TouchPoint` class instead, everywhere.
    - The commented out lines will be removed in the next version.
  - New Version 🆕 `v0.0.4`.

#
### **+05:30 11:33:35 PM 20-02-2025, Thursday**

  - Fixed touch panel rotation issue.
    - Previously the coordinates were rotated incorrectly.
  - New Version 🆕 `v0.0.3`.

#
### **+05:30 07:32:44 PM 19-02-2025, Wednesday**

  - Fixed semantic versioning issue.
  - Fixed missing `#endif` from `CSE_CST328_Constants.h`.
  - New Version 🆕 `v0.0.2`.

#
### **+05:30 10:02:38 PM 18-02-2025, Tuesday**

  - Added [API](/docs/API.md) documentation.

#
### **+05:30 08:57:27 PM 18-02-2025, Tuesday**

  - Updated constants file.
  - Updated all macro instances.
  - Updated Readme.

#
### **+05:30 11:29:05 PM 17-02-2025, Monday**

  - Added new `Read-Touch-Interrupt.ino` example.
    - Tested and working.
  - Updated Readme.

#
### **+05:30 07:37:25 PM 17-02-2025, Monday**

  - Added `Fast-Read-Touch-Polling` example.
    - This now uses the `fastReadData()` to read a single touch point data.
  - Added `fastReadData()` function.
    - This is faster than `readData()`.
  - Added `isTouched (uint8_t id = 0)` overload.
    - This will only check for the provided touch point for activity.
    - If no parameters are passed, all touch points are checked.

#
### **+05:30 06:18:01 PM 17-02-2025, Monday**

  - Updated `Read-Touch-Polling` example.
    - Removed unnecessary code.
    - Added more documentation.
  - Added `Test` example for internal testing.
  - Updated Readme.
    - Added Adafruit FT6206 library link.
    - Added links to PlatformIO.

#
### **+05:30 09:11:26 PM 16-02-2025, Sunday**

  - Added Arduino library specification files.
  - Updated function documentation.
  - Updated Readme.
  - Updated `touchPoints` array initialization.
  - Added new `inited` variable.
  - `begin()` now returns if already initialized.
  - Moved register write and read functions to `private` scope.

#
### **+05:30 04:38:52 PM 16-02-2025, Sunday**

  - Reading multiple touch points working.
  - Initial commit.
  - New Version 🆕 `v0.1`.
