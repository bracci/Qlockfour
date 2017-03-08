/*
   MyRTC
*/

#include <Wire.h>
#include "MyRTC.h"
#include "Debug.h"

/*
   Initialisierung
*/
MyRTC::MyRTC(int address, byte statusLedPin) : TimeStamp(0, 0, 0, 0, 0, 0, false) {
  _address = address;
  _statusLedPin = statusLedPin;
  pinMode(_statusLedPin, OUTPUT);
  digitalWrite(_statusLedPin, LOW);
}

/*
   LED ein- oder ausschalten
*/
void MyRTC::statusLed(boolean on) {
  if (on) {
    digitalWrite(_statusLedPin, HIGH);
  } else {
    digitalWrite(_statusLedPin, LOW);
  }
}

/*
   Uhrzeit auslesen und in den Variablen ablegen
*/
void MyRTC::readTime() {
  byte returnStatus, count, result, retries = 0;
  do {
    // Reset the register pointer
    Wire.beginTransmission(_address);
    Wire.write((uint8_t) 0x00);
    result = Wire.endTransmission(false);
    count = Wire.requestFrom(_address, 7);
    if (count == 7) {
      // Success - A few of these need masks because certain bits are control bits
      _seconds = bcdToDec(Wire.read() & 0x7f);
      _minutes = bcdToDec(Wire.read());
      _hours = bcdToDec(Wire.read() & 0x3f);
      _dayOfWeek = bcdToDec(Wire.read());
      _date = bcdToDec(Wire.read());
      _month = bcdToDec(Wire.read());
      _year = bcdToDec(Wire.read());
    } else {
      // Fail - keine 7 Byte zurueck gekommen? Buffer verwerfen...
      for (int i = 0; i < count; i++) {
        Wire.read();
      }
      retries++;
    }
    result = Wire.endTransmission(true); // true, jetzt den Bus freigeben.
  } while ((count != 7) && (retries < 8));
  // Es konnte nichts gelesen werden
  if (retries == 8) {
    _seconds = 11;
    _minutes = 11;
    _hours = 11;
    _dayOfWeek = 1;
    _date = 1;
    _month = 1;
    _year = 17;
    _isDST = false;
    DEBUG_PRINTLN(F("RTC error. Set defaults."));
  }
}

/*
   Uhrzeit aus den Variablen in die DS3231 schreiben.
*/
void MyRTC::writeTime() {
  Wire.beginTransmission(_address);
  Wire.write((uint8_t) 0x00);
  Wire.write(decToBcd(_seconds));
  Wire.write(decToBcd(_minutes));
  Wire.write(decToBcd(_hours));
  Wire.write(decToBcd(_dayOfWeek));
  Wire.write(decToBcd(_date));
  Wire.write(decToBcd(_month));
  Wire.write(decToBcd(_year));
  Wire.endTransmission();
}

/**
 * SQW fuer DS1307.
 */
void MyRTC::enableSQWOnDS1307() {
    Wire.beginTransmission(_address);
    Wire.write(0x07); // Datenregister
    Wire.write(0b00010000); // enable 1HZ square wave output
    Wire.endTransmission();
}

/*
   SQW mit 1Hz einschalten
*/
void MyRTC::enableSQWOnDS3231() {
  Wire.beginTransmission(_address);
  Wire.write(0x0E);
  Wire.write(0b0000000);
  Wire.endTransmission();
}

/*
   Konvertierung Dezimal zu "Binary Coded Decimal"
*/
byte MyRTC::decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

/*
   Konvertierung "Binary Coded Decimal" zu Dezimal
*/
byte MyRTC::bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

/*
   Aus einem String zwei Stellen als Zahl bekommen.
*/
uint8_t MyRTC::conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

/*
   Setter/Getter
*/

void MyRTC::setHours(byte hours) {
  _hours = hours;
}

void MyRTC::setMinutes(byte minutes) {
  _minutes = minutes;
}

void MyRTC::setSeconds(byte seconds) {
  _seconds = seconds;
}

byte MyRTC::getSeconds() {
  return _seconds;
}

/*
   Temperatur lesen
*/
int8_t MyRTC::getTemperature() {
  Wire.beginTransmission(_address);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(_address, 2);
  return Wire.read();
}
