/*
   IRTranslator
*/

#ifndef IRTRANSLATORHX1838_H
#define IRTRANSLATORHX1838_H

#include "IRTranslator.h"

class IRTranslatorHX1838 : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
