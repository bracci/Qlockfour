/*
   Alarm
*/

#include "Alarm.h"
#include "Debug.h"

/*
   Konstruktor.
*/
Alarm::Alarm(byte speakerPin) : TimeStamp(0, 1, 0, 0, 0, 0, false) {
  _speakerPin = speakerPin;
  pinMode(_speakerPin, OUTPUT);
  _isActive = false;
  _showAlarmTimeTimer = 0;
}

/*
   Den Weckton einschalten.
*/
void Alarm::buzz(boolean on) {
  if (on) {
    digitalWrite(_speakerPin, HIGH);
  } else {
    digitalWrite(_speakerPin, LOW);
  }
}

/*
   Die verbleibende Zeit in Sekunden bekommen, fuer die die Weckzeit angezeigt werden soll.
*/
byte Alarm::getShowAlarmTimeTimer() {
  return _showAlarmTimeTimer;
}

/*
   Die Zeit in Sekunden setzten, fuer die die Weckzeit angezeigt werden soll.
*/
void Alarm::setShowAlarmTimeTimer(byte seconds) {
  _showAlarmTimeTimer = seconds;
}

/*
   Die Zeit, fuer die die Weckzeit angezeigt werden soll, um eine Sekunde verringern.
*/
void Alarm::decShowAlarmTimeTimer() {
  if (_showAlarmTimeTimer > 0) {
    _showAlarmTimeTimer--;
  }
}

/*
   Ist der Wecker aktiv?
*/
boolean Alarm::isActive() {
  return _isActive;
}

/*
   Den Wecker einschalten.
*/
void Alarm::activate() {
  _isActive = true;
}

/*
   Den Wecker ausschalten.
*/
void Alarm::deactivate() {
  _isActive = false;
  buzz(false);
}
