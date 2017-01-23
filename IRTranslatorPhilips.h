/*
   IRTranslator
*/

#ifndef IRTRANSLATORPHILIPS_H
#define IRTRANSLATORPHILIPS_H

#include "IRTranslator.h"

class IRTranslatorPhilips : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
