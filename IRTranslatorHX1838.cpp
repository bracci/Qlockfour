/*
   IRTranslator
*/

#include "IRTranslatorHX1838.h"
#include "Colors.h"
#include "Debug.h"

#define HX1838_CH_MINUS   16753245
#define HX1838_CH         16736925
#define HX1838_CH_PLUS    16769565
#define HX1838_PREV       16720605
#define HX1838_NEXT       16712445
#define HX1838_PLAY       16761405
#define HX1838_VOL_MINUS  16769055
#define HX1838_VOL_PLUS   16754775
#define HX1838_EQ         16748655
#define HX1838_1          16724175
#define HX1838_2          16718055
#define HX1838_3          16743045
#define HX1838_4          16716015
#define HX1838_5          16726215
#define HX1838_6          16734885
#define HX1838_7          16728765
#define HX1838_8          16730805
#define HX1838_9          16732845
#define HX1838_0          16738455
#define HX1838_100        16750695
#define HX1838_200        16756815

void IRTranslatorHX1838::printSignature() {
  DEBUG_PRINT(F("HX1838, "));
}

byte IRTranslatorHX1838::buttonForCode(unsigned long code) {
  switch (code) {
    case HX1838_CH_MINUS:
      return REMOTE_BUTTON_UNDEFINED;
    case HX1838_CH:
      return REMOTE_BUTTON_UNDEFINED;
    case HX1838_CH_PLUS:
      return REMOTE_BUTTON_UNDEFINED;
    case HX1838_NEXT:
      return REMOTE_BUTTON_MODE;
    case HX1838_PREV:
      return REMOTE_BUTTON_EXTMODE;
    case HX1838_EQ:
      return REMOTE_BUTTON_TOGGLEBLANK;
    case HX1838_VOL_PLUS:
      return REMOTE_BUTTON_HOUR_PLUS;
    case HX1838_VOL_MINUS:
      return REMOTE_BUTTON_MINUTE_PLUS;
    case HX1838_PLAY:
      return REMOTE_BUTTON_SETMODE;
    case HX1838_1:
      setTransition(Settings::TRANSITION_MODE_FADE);
      return REMOTE_BUTTON_TRANSITION;
    case HX1838_2:
      setTransition(Settings::TRANSITION_MODE_MATRIX);
      return REMOTE_BUTTON_TRANSITION;
    case HX1838_3:
      setTransition(Settings::TRANSITION_MODE_SLIDE);
      return REMOTE_BUTTON_TRANSITION;
    case HX1838_4:
      setColor(color_white);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_5:
      setColor(color_red);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_6:
      setColor(color_green);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_7:
      setColor(color_darkBlue);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_8:
      setColor(color_yellow);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_9:
      setColor(color_rgb_continuous);
      return REMOTE_BUTTON_SETCOLOR;
    case HX1838_0:
      return REMOTE_BUTTON_LDR;
    case HX1838_100:
      return REMOTE_BUTTON_DARKER;
    case HX1838_200:
      return REMOTE_BUTTON_BRIGHTER;
    default:
      return REMOTE_BUTTON_UNDEFINED;
  }
}

