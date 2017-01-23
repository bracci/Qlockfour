/*
   IRTranslator
*/

#ifndef IRTRANSLATORAPPLE_H
#define IRTRANSLATORAPPLE_H

#include "IRTranslator.h"

class IRTranslatorApple : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
