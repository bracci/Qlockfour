/*
   LedDriverLPD8806RGBW
*/

#ifndef LED_DRIVER_LPD8806RGBW_H
#define LED_DRIVER_LPD8806RGBW_H

#include "Arduino.h"
#include "LedDriver.h"
#include <LPD8806RGBW.h>


class LedDriverLPD8806RGBW : public LedDriver {
  public:
    LedDriverLPD8806RGBW(byte dataPin, byte clockPin);

    void init();

    void printSignature();

    void writeScreenBufferToMatrix(word matrix[16], boolean onChange, eColors a_color);

    void setBrightness(byte brightnessInPercent);
    byte getBrightness();

    void setLinesToWrite(byte linesToWrite);

    void shutDown();
    void wakeUp();

    void clearData();

  private:
    byte _brightnessInPercent;

    boolean _dirty;

    void _setPixel(byte x, byte y, uint32_t c);
    void _setPixel(byte num, uint32_t c);

    uint32_t _wheel(byte brightness, byte wheelPos);

    void _clear();

    byte _brightnessScaleColor(byte brightness, byte colorPart);

    unsigned int _lastLEDsOn;

    LPD8806RGBW *_strip;

};

#endif

