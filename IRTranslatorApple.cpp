/*
   IRTranslator
*/

#include "IRTranslatorApple.h"
#include "Debug.h"

#define APPLE_PLAY  2011275367
#define APPLE_MENUE 2011283559
#define APPLE_BACK  2011271271
#define APPLE_FF    2011258983
#define APPLE_PLUS  2011254887
#define APPLE_MINUS 2011246695

void IRTranslatorApple::printSignature() {
  DEBUG_PRINT(F("Apple, "));
}

byte IRTranslatorApple::buttonForCode(unsigned long code) {
  switch (code) {
    case APPLE_PLAY:
      return REMOTE_BUTTON_MODE;
    case APPLE_MENUE:
      return REMOTE_BUTTON_EXTMODE;
    case APPLE_BACK:
      return REMOTE_BUTTON_SETMODE;
    case APPLE_FF:
      return REMOTE_BUTTON_TOGGLEBLANK;
    case APPLE_PLUS:
      return REMOTE_BUTTON_HOUR_PLUS;
    case APPLE_MINUS:
      return REMOTE_BUTTON_MINUTE_PLUS;
    default:
      return REMOTE_BUTTON_UNDEFINED;
  }
}
