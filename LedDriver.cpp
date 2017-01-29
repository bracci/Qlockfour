/*
   LedDriver
*/

#include "LedDriver.h"
#include "Debug.h"

void LedDriver::demoTransition() {
  _demoTransition = true;
}

void LedDriver::resetWheelPos() {
  _wheelPos = 0;
  _lastColorUpdate = millis();
}
