/*
   IRTranslatorCLT
*/

#ifndef IRTRANSLATORCLT_H
#define IRTRANSLATORCLT_H

#include "IRTranslator.h"

class IRTranslatorCLT : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
