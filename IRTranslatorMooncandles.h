/*
   IRTranslatorMooncandles
*/

#ifndef IRTRANSLATORMOONCANDLES_H
#define IRTRANSLATORMOONCANDLES_H

#include "Arduino.h"
#include "IRTranslator.h"

class IRTranslatorMooncandles : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
