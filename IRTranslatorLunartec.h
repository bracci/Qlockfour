/*
   IRTranslatorLunartec
*/

#ifndef IRTRANSLATORLUNARTEC_H
#define IRTRANSLATORLUNARTEC_H

#include "Arduino.h"
#include "IRTranslator.h"

class IRTranslatorLunartec : public IRTranslator {
  public:
    void printSignature();
    byte buttonForCode(unsigned long code);
};

#endif
