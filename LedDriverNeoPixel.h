/*
   LedDriverNeoPixel
*/

#ifndef LED_DRIVER_NEOPIXEL_H
#define LED_DRIVER_NEOPIXEL_H

#include "Arduino.h"
#include "LedDriver.h"
#ifdef LED_DRIVER_NEOPIXELRGBW
  #include <Adafruit_NeoPixel_RGBW.h>
#else
  #include <Adafruit_NeoPixel.h>
#endif


class LedDriverNeoPixel : public LedDriver {
  public:
    LedDriverNeoPixel(byte dataPin);

    void init();

    void printSignature();

    void writeScreenBufferToMatrix(word matrix[16], boolean onChange, eColors a_color = color_none);

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

    byte _brightnessScaleColor(byte brightness, byte colorPart);
    
#ifdef LED_DRIVER_NEOPIXELRGBW
  Adafruit_NeoPixel_RGBW *_strip;
#else
  Adafruit_NeoPixel *_strip;
#endif
    
};

#endif
