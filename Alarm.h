/*
   Alarm
*/

#ifndef ALARM_H
#define ALARM_H

#include "Arduino.h"
#include "TimeStamp.h"

class Alarm : public TimeStamp {
  public:
    Alarm(byte speakerPin);

    void activate();
    void deactivate();
    void buzz(boolean on);

    byte getShowAlarmTimeTimer();
    void setShowAlarmTimeTimer(byte seconds);
    void decShowAlarmTimeTimer();

    boolean isActive();

  private:
    boolean _isActive;
    byte _showAlarmTimeTimer;
    byte _speakerPin;
};

#endif
