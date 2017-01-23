/*
   LedDriver
*/

#include "LedDriver.h"
#include "Debug.h"

void LedDriver::setPixelInScreenBuffer(byte x, byte y, word matrix[16]) {
  matrix[y] |= 0b1000000000000000 >> x;
}

boolean LedDriver::getPixelFromScreenBuffer(byte x, byte y, word matrix[16]) {
  return (matrix[y] & (0b1000000000000000 >> x)) == (0b1000000000000000 >> x);
}

void LedDriver::demoTransition() {
  _demoTransition = true;
}

void LedDriver::resetWheelPos() {
  _wheelPos = 0;
  _lastColorUpdate = millis();
}
