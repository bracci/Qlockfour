/*
   IRTranslatorSparkfun
*/

#ifndef IRTRANSLATORSPARKFUN_H
#define IRTRANSLATORSPARKFUN_H

#include "Arduino.h"
#include "IRTranslator.h"

class IRTranslatorSparkfun : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
