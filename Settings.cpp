/*
  Settings
*/

#include "Settings.h"
#include <EEPROM.h>
#include "Renderer.h"
#include "debug.h"

#define SETTINGS_MAGIC_NUMBER 0x2A
#define SETTINGS_VERSION 3

/*
    Konstruktor
*/
Settings::Settings() {
  _nightModeTime[0] = new TimeStamp(0, 0, 0, 0, 0, 0);
  _nightModeTime[1] = new TimeStamp(0, 0, 0, 0, 0, 0);
  resetToDefault();
  loadFromEEPROM();
}

/*
   Setzt alle Werte auf Defauleinstellungen
*/
void Settings::resetToDefault() {
  _language = 0;
  _use_ldr = true;
  _brightness = 25;
  _transitionMode = TRANSITION_MODE_NORMAL;
  _color = color_white;
  _colorChangeRate = 0;
  // um 3 Uhr Display abschalten (Minuten, Stunden, -, -, -, -)
  _nightModeTime[0]->set(0, 3, 0, 0, 0, 0);
  // um 4:30 Uhr Display wieder anschalten (Minuten, Stunden, -, -, -, -)
  _nightModeTime[1]->set(30, 4, 0, 0, 0, 0);
  _jumpToNormalTimeout = 5;
  _esIst = true;
}

byte Settings::getLanguage() {
  return _language;
}

void Settings::setLanguage(byte language) {
  _language = language;
}

boolean Settings::getUseLdr() {
  return _use_ldr;
}

void Settings::setUseLdr(boolean useLdr) {
  _use_ldr = useLdr;
}

byte Settings::getBrightness() {
  return _brightness;
}

void Settings::setBrightness(byte brightness) {
  _brightness = brightness;
}

byte Settings::getTransitionMode() {
  return _transitionMode;
}

void Settings::setTransitionMode(byte transitionMode) {
  _transitionMode = transitionMode;
}

void Settings::setColor(eColors color) {
  _color = color;
}

eColors Settings::getColor() {
  return _color;
}

void Settings::setColorChangeRate(byte rate) {
  _colorChangeRate = rate;
}

byte Settings::getColorChangeRate() {
  return _colorChangeRate;
}

TimeStamp* Settings::getNightModeTime(bool onTime) {
  return _nightModeTime[onTime];
}

byte Settings::getJumpToNormalTimeout() {
  return _jumpToNormalTimeout;
}

void Settings::setJumpToNormalTimeout(byte jumpToNormalTimeout) {
  _jumpToNormalTimeout = jumpToNormalTimeout;
}

boolean Settings::getEsIst() {
  return _esIst;
}

void Settings::toggleEsIst() {
  _esIst = !_esIst;
}

/*
   Die Einstellungen laden
*/
void Settings::loadFromEEPROM() {
  EEPROM.begin(512);
  if ((EEPROM.read(0) == SETTINGS_MAGIC_NUMBER) && (EEPROM.read(1) == SETTINGS_VERSION)) {
    // es sind gueltige Einstellungen vorhanden...
    if (EEPROM.read(2) < LANGUAGE_COUNT) {
      _language = EEPROM.read(2);
    }
    _use_ldr = EEPROM.read(3);
    _brightness = EEPROM.read(4);
    _transitionMode = EEPROM.read(5);
    _color = (eColors)EEPROM.read(6);
    _colorChangeRate = EEPROM.read(7);
    _nightModeTime[0]->set(EEPROM.read(8), EEPROM.read(9), 0, 0, 0, 0);
    _nightModeTime[1]->set(EEPROM.read(10), EEPROM.read(11), 0, 0, 0, 0);
    _jumpToNormalTimeout = EEPROM.read(12);
    _esIst = EEPROM.read(14);
  }
}

/*
   Die Einstellungen speichern
*/
void Settings::saveToEEPROM() {
  EEPROM.begin(512);
  if (EEPROM.read(0) != SETTINGS_MAGIC_NUMBER) {
    EEPROM.write(0, SETTINGS_MAGIC_NUMBER);
  }
  if (EEPROM.read(1) != SETTINGS_VERSION) {
    EEPROM.write(1, SETTINGS_VERSION);
  }
  if (EEPROM.read(2) != _language) {
    EEPROM.write(2, _language);
  }
  if (EEPROM.read(3) != _use_ldr) {
    EEPROM.write(3, _use_ldr);
  }
  if (EEPROM.read(4) != _brightness) {
    EEPROM.write(4, _brightness);
  }
  if (EEPROM.read(5) != _transitionMode) {
    EEPROM.write(5, _transitionMode);
  }
  if (EEPROM.read(6) != _color) {
    EEPROM.write(6, _color);
  }
  if (EEPROM.read(7) != _colorChangeRate) {
    EEPROM.write(7, _colorChangeRate);
  }
  if (EEPROM.read(8) != _nightModeTime[0]->getMinutes()) {
    EEPROM.write(8, _nightModeTime[0]->getMinutes());
  }
  if (EEPROM.read(9) != _nightModeTime[0]->getHours()) {
    EEPROM.write(9, _nightModeTime[0]->getHours());
  }
  if (EEPROM.read(10) != _nightModeTime[1]->getMinutes()) {
    EEPROM.write(10, _nightModeTime[1]->getMinutes());
  }
  if (EEPROM.read(11) != _nightModeTime[1]->getHours()) {
    EEPROM.write(11, _nightModeTime[1]->getHours());
  }
  if (EEPROM.read(12) != _jumpToNormalTimeout) {
    EEPROM.write(12, _jumpToNormalTimeout);
  }
  if (EEPROM.read(14) != _esIst) {
    EEPROM.write(14, _esIst);
  }
  EEPROM.commit();
}
