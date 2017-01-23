/*
   Renderer
*/

#ifndef RENDERER_H
#define RENDERER_H

#include "Arduino.h"
#include "Configuration.h"

enum eLanguage : byte {
#ifdef ENABLE_LANGUAGE_DE
  LANGUAGE_DE_DE,
  LANGUAGE_DE_SW,
  LANGUAGE_DE_BA,
  LANGUAGE_DE_SA,
#endif
#ifdef ENABLE_LANGUAGE_DE_MKF
  LANGUAGE_DE_MKF_DE,
  LANGUAGE_DE_MKF_SW,
  LANGUAGE_DE_MKF_BA,
  LANGUAGE_DE_MKF_SA,
#endif
#ifdef ENABLE_LANGUAGE_D3
  LANGUAGE_D3,
#endif
#ifdef ENABLE_LANGUAGE_CH
  LANGUAGE_CH,
  LANGUAGE_CH_X,
#endif
#ifdef ENABLE_LANGUAGE_EN
  LANGUAGE_EN,
#endif
#ifdef ENABLE_LANGUAGE_FR
  LANGUAGE_FR,
#endif
#ifdef ENABLE_LANGUAGE_IT
  LANGUAGE_IT,
#endif
#ifdef ENABLE_LANGUAGE_NL
  LANGUAGE_NL,
#endif
#ifdef ENABLE_LANGUAGE_ES
  LANGUAGE_ES,
#endif
  LANGUAGE_COUNT,
};

class Renderer {
  public:
    enum eTextPos : byte {
      TEXT_POS_TOP,
      TEXT_POS_MIDDLE = 2,
      TEXT_POS_BOTTOM = 5
    };
  public:
    Renderer();

    void setMinutes(char hours, byte minutes, byte language, word matrix[16]);
    void setCorners(byte minutes, word matrix[16]);
    void activateAlarmLed(word matrix[16]);
    void clearEntryWords(byte language, word matrix[16]);
    void clearScreenBuffer(word matrix[16]);
    void setAllScreenBuffer(word matrix[16]);
    void setMenuText(const char* menuText, eTextPos textPos, word matrix[16]);
    void activateAMPM(byte hours, byte language, word matrix[16]);

  private:
    void setHours(byte hours, boolean glatt, byte language, word matrix[16]);
    boolean isNumber(char symbol);

    // Spezialfaelle
#ifdef ENABLE_LANGUAGE_FR
    void FR_hours(byte hours, word matrix[16]);
#endif
#ifdef ENABLE_LANGUAGE_IT
    void IT_hours(byte hours, word matrix[16]);
#endif
#ifdef ENABLE_LANGUAGE_ES
    void ES_hours(byte hours, word matrix[16]);
#endif
};

#endif
