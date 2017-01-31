/*
   IRTranslator
*/

#include "IRTranslatorPhilips.h"
#include "Colors.h"
#include "Debug.h"

#define PHILIPS_POWER      2021932000
#define PHILIPS_MUTE       2021931988
#define PHILIPS_PROG_PLUS  2021931984
#define PHILIPS_PROG_MINUS 2021932016
#define PHILIPS_VOL_PLUS   2021931976
#define PHILIPS_VOL_MINUS  2021932008
#define PHILIPS_1          2021932002
#define PHILIPS_2          2021931986
#define PHILIPS_3          2021932018
#define PHILIPS_4          2021931978
#define PHILIPS_5          2021932010
#define PHILIPS_6          2021931994
#define PHILIPS_7          2021932026
#define PHILIPS_8          2021931974
#define PHILIPS_9          2021932006
#define PHILIPS_0          2021931970
#define PHILIPS_AV         2021931996
#define PHILIPS_00         2021931990

void IRTranslatorPhilips::printSignature() {
  DEBUG_PRINT(F("Philips SRP1 101/10 - 0815"));
}

byte IRTranslatorPhilips::buttonForCode(unsigned long code) {
  switch (code) {
    case PHILIPS_PROG_PLUS:
      return REMOTE_BUTTON_MODE;
    case PHILIPS_PROG_MINUS:
      return REMOTE_BUTTON_EXTMODE;
    case PHILIPS_POWER:
      return REMOTE_BUTTON_TOGGLEBLANK;
    case PHILIPS_VOL_PLUS:
      return REMOTE_BUTTON_HOUR_PLUS;
    case PHILIPS_VOL_MINUS:
      return REMOTE_BUTTON_MINUTE_PLUS;
    case PHILIPS_MUTE:
      return REMOTE_BUTTON_SETMODE;
    case PHILIPS_1:
      setTransition(Settings::TRANSITION_MODE_FADE);
      return REMOTE_BUTTON_TRANSITION;
    case PHILIPS_2:
      setTransition(Settings::TRANSITION_MODE_MATRIX);
      return REMOTE_BUTTON_TRANSITION;
    case PHILIPS_3:
      setTransition(Settings::TRANSITION_MODE_SLIDE);
      return REMOTE_BUTTON_TRANSITION;
    case PHILIPS_4:
      setColor(color_white);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_5:
      setColor(color_red);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_6:
      setColor(color_green);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_7:
      setColor(color_darkBlue);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_8:
      setColor(color_yellow);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_9:
      setColor(color_rgb_continuous);
      return REMOTE_BUTTON_SETCOLOR;
    case PHILIPS_0:
      return REMOTE_BUTTON_LDR;
    case PHILIPS_AV:
      return REMOTE_BUTTON_DARKER;
    case PHILIPS_00:
      return REMOTE_BUTTON_BRIGHTER;
    default:
      return REMOTE_BUTTON_UNDEFINED;
  }
}
