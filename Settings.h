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

    void loadFromEEPROM();
    void saveToEEPROM();

    void resetToDefault();

    TimeStamp* getNightModeTime(bool onTime);

  private:
    byte _language;
    boolean _renderCornersCw;
    boolean _use_ldr;
    byte _brightness;
    boolean _enableAlarm;
    boolean _dcfSignalIsInverted;
    char _timeShift;
    eColors _color;
    byte _transitionMode;
    byte _event;
    TimeStamp* _nightModeTime[2];

    byte _jumpToNormalTimeout;
    byte _colorChangeRate;

    byte _esIst;
    
};

#endif
