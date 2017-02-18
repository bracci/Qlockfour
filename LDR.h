/*
   LDR.h
*/

#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Configuration.h"
#include "Boards.h"

class LDR {
  public:
    LDR(byte pin);
    byte value();

  private:
    byte _pin;
    int _lastValue;
    byte _outputValue;
    int _min;
    int _max;
};

#endif

