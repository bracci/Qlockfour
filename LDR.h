/*
   LDR
*/

#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Configuration.h"

class LDR {
  public:
    LDR(byte pin);

    byte value();

  private:
int myConstrain(int x, int a, int b);
  
    byte _pin;
    int _lastValue;
    byte _outputValue;
    int _min;
    int _max;
};

#endif
