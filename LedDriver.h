/*
   LedDriver
*/

#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include "Arduino.h"
#include "Settings.h"
#include "MyRTC.h"
#include "Transitions.h"
#include "Modes.h"
#include "Configuration.h"
#include "Colors.h"

extern volatile byte helperSeconds;
extern Mode mode;
extern Settings settings;
extern MyRTC rtc;

class LedDriver {
  public:
    virtual void init();

    virtual void printSignature();

    virtual void writeScreenBufferToMatrix(word matrix[16], boolean onChange, eColors a_color);

    virtual void setBrightness(byte brightnessInPercent);
    virtual byte getBrightness();

    virtual void setLinesToWrite(byte linesToWrite);

    virtual void shutDown();
    virtual void wakeUp();

    virtual void clearData();

    void resetWheelPos(void);

    void demoTransition();

  protected:
    word _matrixOld[11];
    word _matrixNew[11];
    word _matrixOverlay[11];
    byte _wheelPos;
    unsigned long _lastColorUpdate;
    boolean _transitionCompleted;
    uint16_t _transitionCounter;
    boolean _demoTransition;
};

#endif
