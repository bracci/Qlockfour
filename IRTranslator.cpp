/*
   IRTranslator
*/

#include "IRTranslator.h"

eColors IRTranslator::getColor() {
  return _color;
}

Mode IRTranslator::getMode() {
  return _mode;
}

byte IRTranslator::getTransition() {
  return _transition;
}

void IRTranslator::setColor(eColors color) {
  _color = color;
}

void IRTranslator::setMode(Mode mode) {
  _mode = mode;
}

void IRTranslator::setTransition(byte transition) {
  _transition = transition;
}
