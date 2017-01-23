/*
   LDR
*/

#include "LDR.h"
#include "Debug.h"

LDR::LDR(byte pin) {
  _pin = pin;
  _lastValue = 1;
  _outputValue = 0;
  _min = 1023;
  _max = 0;
}

// Wert des LDR in Prozent
byte LDR::value() {
  int rawVal, val;
  rawVal = (1023 - analogRead(_pin));
  if ((rawVal != _lastValue) && ((rawVal == 0) || (rawVal == 1023) || (rawVal > (_lastValue + LDR_HYSTERESE) || (rawVal < _lastValue - LDR_HYSTERESE)))) {
    val = rawVal;
    _lastValue = val;
    if (val < _min) {
      _min = val;
    }
    if (val > _max) {
      _max = val + 1;
    }
    byte mapVal = map(val, _min, _max, 0, 100);
    mapVal = constrain(mapVal, LDR_MIN_PERCENT, LDR_MAX_PERCENT);
    _outputValue = mapVal;
  }
  return _outputValue;
}
