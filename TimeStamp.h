/*
   TimeStamp
*/

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Arduino.h"

class TimeStamp {
  public:
    TimeStamp(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year);

    void incMinutes();
    void decMinutes();
    void incFiveMinutes();
    void decFiveMinutes();
    void incHours();
    void decHours();
    void incDate(byte addDate = 1);
    void incMonth(byte addMonth = 1);
    void incYear(byte addYear = 1);

    byte getMinutes();
    int getMinutesOfDay(int minutesDiff);
    int getMinutesOf12HoursDay(int minutesDiff);
    byte getHours();

    byte getDate();
    byte getDayOfWeek();
    byte getMonth();
    byte getYear();
    unsigned long getMinutesOfCentury();

    void setMinutes(byte minutes);
    void setHours(byte hours);
    void setDate(byte date);
    void setDayOfWeek(byte dayOfWeek);
    void setMonth(byte month);
    void setYear(byte year);

    void set(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year);
    void set(TimeStamp* timeStamp);

    char* asString();

  protected:
    byte getDaysOfMonth(byte month, byte year);
    void CalculateAndSetDayOfWeek();
    void CheckDateValidity();

    byte _minutes;
    byte _hours;

    byte _date;
    byte _dayOfWeek;
    byte _month;
    byte _year;

    char _cDateTime[17];
};

#endif
