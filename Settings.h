/*
   Settings
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"
#include "Colors.h"
#include "TimeStamp.h"

class Settings {
  public:
    enum eTransitions : byte {
      TRANSITION_MODE_NORMAL,
      TRANSITION_MODE_FADE,
      TRANSITION_MODE_MATRIX,
      TRANSITION_MODE_SLIDE,
      TRANSITION_MODE_MAX
    };

    Settings();

    byte getLanguage();
    void setLanguage(byte language);

    boolean getUseLdr();
    void setUseLdr(boolean useLdr);

    byte getBrightness();
    void setBrightness(byte brightness);

    byte getTransitionMode();
    void setTransitionMode(byte transitionMode);

    void setColor(eColors color);
    eColors getColor();

    byte getColorChangeRate();
    void setColorChangeRate(byte rate);

    byte getJumpToNormalTimeout();
    void setJumpToNormalTimeout(byte jumpToNormalTimeout);

    boolean getEsIst();
    void toggleEsIst();

    TimeStamp* getNightModeTime(bool onTime);

    void loadFromEEPROM();
    void saveToEEPROM();
    void resetToDefault();

  private:
    byte _language;
    boolean _use_ldr;
    byte _brightness;
    byte _transitionMode;
    eColors _color;
	byte _colorChangeRate;
    byte _jumpToNormalTimeout;
    byte _esIst;
    TimeStamp* _nightModeTime[2];
};

#endif
