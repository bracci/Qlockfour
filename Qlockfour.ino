/*
   QLOCKFOUR NodeMCU
   Eine Firmware der Selbstbau-QLOCKTWO.

   @mc       NodeMCU/ESP8266
   @created  22.01.2017
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <IRremoteESP8266.h>
#include "pins_arduino.h"
#include "Configuration.h"
#include "Debug.h"
#include "Boards.h"
#include "MyRTC.h"
#include "LedDriver.h"
#include "LedDriverNeoPixel.h"
#include "LedDriverLPD8806.h"
#include "Renderer.h"
#include "IRTranslator.h"
#include "IRTranslatorSparkfun.h"
#include "IRTranslatorMooncandles.h"
#include "IRTranslatorLunartec.h"
#include "IRTranslatorCLT.h"
#include "IRTranslatorApple.h"
#include "IRTranslatorPhilips.h"
#include "Staben.h"
#include "Zahlen.h"
#include "LDR.h"
#include "Settings.h"
#include "Modes.h"

/******************************************************************************
   Declaration
******************************************************************************/

#ifdef LED_DRIVER_NEOPIXEL
LedDriverNeoPixel ledDriver(PIN_LEDS_DATA);
#endif
#ifdef LED_DRIVER_LPD8806
LedDriverLPD8806 ledDriver(PIN_LEDS_DATA, PIN_LEDS_CLOCK);
#endif

Settings settings;

Renderer renderer;

word matrix[16];

Mode mode = STD_MODE_NORMAL;
Mode lastMode = mode;
byte fallBackCounter = 0;

MyRTC rtc(0x68, PIN_SQW_LED);
volatile byte helperSeconds;
volatile bool needsUpdateFromRtc = true;

LDR ldr(PIN_LDR);
unsigned long lastBrightnessCheck;
byte brightnessToDisplay;

unsigned long temperature = 0;

byte testColumn;

IRrecv irrecv(PIN_IR_RECEIVER);
decode_results irDecodeResults;

#ifdef REMOTE_SPARKFUN
IRTranslatorSparkfun irTranslator;
#endif
#ifdef REMOTE_MOONCANDLES
IRTranslatorMooncandles irTranslator;
#endif
#ifdef REMOTE_LUNARTEC
IRTranslatorLunartec irTranslator;
#endif
#ifdef REMOTE_CLT2
IRTranslatorCLT irTranslator;
#endif
#ifdef REMOTE_APPLE
IRTranslatorApple irTranslator;
#endif
#ifdef REMOTE_PHILIPS
IRTranslatorPhilips irTranslator;
#endif

char ssid[] = WLAN_SSID;
char pass[] = WLAN_PASS;
unsigned int localPort = 2390;
IPAddress timeServerIP;
const char* ntpServerName = NTP_SERVER;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;
int nextNtpSync = 0;

/******************************************************************************
   setup()
******************************************************************************/

void setup() {
  Serial.begin(SERIAL_SPEED);

#ifdef DEBUG_SET_DEFAULTS
  factoryReset();
#endif

  // RTC DS3231 initialisieren.
  Wire.begin();
  pinMode(PIN_SQW_SIGNAL, INPUT);
  digitalWrite(PIN_SQW_SIGNAL, HIGH);
  rtc.enableSQWOnDS3231();
  attachInterrupt(digitalPinToInterrupt(PIN_SQW_SIGNAL), updateFromRtc, FALLING);
  rtc.readTime();
  if ((rtc.getSeconds() >= 60) || (rtc.getMinutes() >= 60) || (rtc.getHours() >= 24) || (rtc.getYear() < 17)) {
    rtc.set(11, 11, 1, 1, 1, 17);
    rtc.setSeconds(11);
  }
  rtc.writeTime();
  helperSeconds = rtc.getSeconds();

  // LED-Treiber initialisieren und Display einschalten.
  ledDriver.init();
  ledDriver.clearData();
  ledDriver.setLinesToWrite(10);
  ledDriver.wakeUp();
  ledDriver.setBrightness(settings.getBrightness());
  renderer.clearScreenBuffer(matrix);

  // IR-Empfaenger initialisieren.
  irrecv.enableIRIn();

  // LDR initialisieren.
  pinMode(PIN_LDR, INPUT);
  LDR ldr(PIN_LDR);

#ifdef LED_TEST_INTRO
  // Renderer-, Leddriver- und LED-Test.
  unsigned long initMillis = millis();
  while ((millis() - initMillis) < 3000) {
    renderer.setAllScreenBuffer(matrix);
    ledDriver.writeScreenBufferToMatrix(matrix, true, eColors::color_white);
  }
  renderer.clearScreenBuffer(matrix);
  ledDriver.writeScreenBufferToMatrix(matrix, true, eColors::color_white);
#endif

  DEBUG_PRINTLN();
  DEBUG_PRINTLN("QLOCKFOUR NodeMCU");
  DEBUG_PRINT("Version: ");
  DEBUG_PRINTLN(FIRMWARE_VERSION);

  // Mit WiFi verbinden. Timeout ist 1 Minute.
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINTLN(ssid);
  int i = 0;
  WiFi.begin(ssid, pass);
  while ((WiFi.status() != WL_CONNECTED) && (i < 60)) {
    delay(1000);
    i++;
  }
  if (WiFi.status() != WL_CONNECTED) DEBUG_PRINTLN("Error connecting to WiFi.");
  else {
    DEBUG_PRINTLN("WiFi connected.");
    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    DEBUG_PRINT("Starting UDP on Port ");
    udp.begin(localPort);
    DEBUG_PRINTLN(udp.localPort());
  }
}

/******************************************************************************
   loop()
******************************************************************************/

void loop() {

  // LDR lesen und Helligkeit einstellen.
  if (settings.getUseLdr()) {
    if (lastBrightnessCheck + LDR_CHECK_RATE < millis()) {
      byte lv = ldr.value();
      if (ledDriver.getBrightness() > lv) {
        ledDriver.setBrightness(ledDriver.getBrightness() - 1);
      } else if (ledDriver.getBrightness() < lv) {
        ledDriver.setBrightness(ledDriver.getBrightness() + 1);
      }
      lastBrightnessCheck = millis();
    }
  }

  if (needsUpdateFromRtc) {
    needsUpdateFromRtc = false;
    // Alle 60 Sekunden ausführen.
    if (helperSeconds > 59) {
      rtc.readTime();
      helperSeconds = rtc.getSeconds();
      // Alle 23 Stunden die Zeit vom NTP-Server holen.
      if ((nextNtpSync <= 0) && (WiFi.status() == WL_CONNECTED)) {
        if (setTimeFromNtp(ntpServerName)) nextNtpSync = 1380;
      }
      nextNtpSync--;
      DEBUG_PRINT("Minutes until next NTP-sync: ");
      DEBUG_PRINTLN(nextNtpSync);
    }

    switch (mode) {

      /******************************************************************************
        Render STD_MODE_*
      ******************************************************************************/

      case STD_MODE_NORMAL:
        // Aktuelle Zeit in die Matrix schreiben.
        renderer.clearScreenBuffer(matrix);
        renderer.setMinutes(rtc.getHours(), rtc.getMinutes(), settings.getLanguage(), matrix);
        renderer.setCorners(rtc.getMinutes(), matrix);
#ifdef USE_EXT_MODE_IT_IS
        // "ES IST" weg. Zur vollen Stunde und um halb aber anzeigen
        if (!settings.getEsIst() && ((rtc.getMinutes() / 5) % 6)) {
          renderer.clearEntryWords(settings.getLanguage(), matrix);
        }
#endif
#ifdef DEBUG_TIME
        Serial.print(rtc.getHours());
        Serial.print(F(":"));
        Serial.print(rtc.getMinutes());
        Serial.print(F(":"));
        Serial.println(helperSeconds);
#endif
        break;
#ifdef USE_STD_MODE_AMPM
      case STD_MODE_AMPM:
        renderer.clearScreenBuffer(matrix);
        if (rtc.getHours() < 12) {
          renderer.setMenuText("AM", Renderer::TEXT_POS_MIDDLE, matrix);
          DEBUG_PRINTLN("AM");
        } else {
          renderer.setMenuText("PM", Renderer::TEXT_POS_MIDDLE, matrix);
          DEBUG_PRINTLN("PM");
        }
        break;
#endif
#ifdef USE_STD_MODE_SECONDS
      case STD_MODE_SECONDS:
        renderer.clearScreenBuffer(matrix);
        renderer.setCorners(rtc.getMinutes(), matrix);
        for (byte i = 0; i < 7; i++) {
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[helperSeconds / 10][i])) << 11;
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[helperSeconds % 10][i])) << 5;
        }
        DEBUG_PRINTLN(helperSeconds);
        break;
#endif
#ifdef USE_STD_MODE_DATE
      case STD_MODE_DATE:
        renderer.clearScreenBuffer(matrix);
        for (byte i = 0; i < 5; i++) {
          matrix[0 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getDate() / 10][i])) << 11;
          matrix[0 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getDate() % 10][i])) << 6;
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getMonth() / 10][i])) << 11;
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getMonth() % 10][i])) << 6;
        }
        ledDriver.setPixelInScreenBuffer(10, 4, matrix);
        ledDriver.setPixelInScreenBuffer(10, 9, matrix);
        DEBUG_PRINT(rtc.getDate());
        DEBUG_PRINT(F("."));
        DEBUG_PRINT(rtc.getMonth());
        DEBUG_PRINT(F("."));
        DEBUG_PRINTLN(rtc.getYear());
        break;
#endif
#ifdef USE_STD_MODE_TEMP
      case STD_MODE_TEMP:
        temperature = 0;
        for (byte i = 0; i < 4; i++) temperature += rtc.getTemperature() + TEMP_OFFSET;
        renderer.clearScreenBuffer(matrix);
        for (byte i = 0; i < 7; i++) {
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[(temperature / 5) / 10][i])) << 11;
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[(temperature / 5) % 10][i])) << 5;
        }
        matrix[0] |= 0b0000000000011111; // LED rechts oben setzen als "Grad".
        DEBUG_PRINTLN(temperature / 5);
        break;
#endif
      case STD_MODE_BRIGHTNESS:
        renderer.clearScreenBuffer(matrix);
        brightnessToDisplay = map(settings.getBrightness(), 1, 100, 0, 9);
        for (byte xb = 0; xb < brightnessToDisplay; xb++) {
          for (byte yb = 0; yb <= xb; yb++) {
            matrix[9 - yb] |= 1 << (14 - xb);
          }
        }
        DEBUG_PRINTLN(settings.getBrightness());
        break;
      case STD_MODE_BLANK:
      case STD_MODE_NIGHT:
        renderer.clearScreenBuffer(matrix);
        break;

        /******************************************************************************
           Render EXT_MODE_*
        ******************************************************************************/

#ifdef USE_EXT_MODE_TITLES
      case EXT_MODE_TEXT_MAIN:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("MA", Renderer::TEXT_POS_TOP, matrix);
        renderer.setMenuText("IN", Renderer::TEXT_POS_BOTTOM, matrix);
        DEBUG_PRINTLN(F("MAIN"));
        break;
#endif
      case EXT_MODE_LDR_MODE:
        renderer.clearScreenBuffer(matrix);
        if (settings.getUseLdr()) {
          renderer.setMenuText("A", Renderer::TEXT_POS_MIDDLE, matrix);
          DEBUG_PRINTLN(F("A"));
        } else {
          renderer.setMenuText("M", Renderer::TEXT_POS_MIDDLE, matrix);
          DEBUG_PRINTLN(F("M"));
        }
        break;
      case EXT_MODE_TRANSITION:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("TM", Renderer::TEXT_POS_TOP, matrix);
        switch (settings.getTransitionMode()) {
          case Settings::TRANSITION_MODE_NORMAL:
            renderer.setMenuText("NO", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("TM NO"));
            break;
          case Settings::TRANSITION_MODE_FADE:
            renderer.setMenuText("FD", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("TM FD"));
            break;
          case Settings::TRANSITION_MODE_MATRIX:
            renderer.setMenuText("MX", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("TM MX"));
            break;
          case Settings::TRANSITION_MODE_SLIDE:
            renderer.setMenuText("SD", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("TM SD"));
            break;
        }
        break;
      case EXT_MODE_COLOR:
        renderer.clearScreenBuffer(matrix);
        if (settings.getColor() <= color_single_max) {
          renderer.setMenuText("C", Renderer::TEXT_POS_TOP, matrix);
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getColor() / 10][i])) << 10;
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getColor() % 10][i])) << 5;
          }
          DEBUG_PRINT(F("C "));
          DEBUG_PRINTLN(settings.getColor());
        }
        else {
          renderer.setMenuText("CC", Renderer::TEXT_POS_TOP, matrix);
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[(settings.getColor() - color_single_max) / 10][i])) << 10;
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[(settings.getColor() - color_single_max) % 10][i])) << 5;
          }
          DEBUG_PRINT(F("CC "));
          DEBUG_PRINTLN(settings.getColor() - color_single_max);
        }
        break;
      case EXT_MODE_COLOR_CHANGE:
        if (settings.getColor() > color_single_max) { // nur anzeigen wenn CC01 oder CC02
          renderer.clearScreenBuffer(matrix);
          renderer.setMenuText("CR", Renderer::TEXT_POS_TOP, matrix);
          for (byte i = 0; i < 7; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getColorChangeRate() / 10][i])) << 10;
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getColorChangeRate() % 10][i])) << 5;
          }
          DEBUG_PRINT(F("CR "));
          DEBUG_PRINTLN(settings.getColorChangeRate());
        } else mode++;
        break;
      case EXT_MODE_LANGUAGE:
        renderer.clearScreenBuffer(matrix);
        switch (settings.getLanguage()) {
#ifdef ENABLE_LANGUAGE_DE
          case LANGUAGE_DE_DE:
            renderer.setMenuText("DE", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("DE"));
            break;
          case LANGUAGE_DE_SW:
            renderer.setMenuText("DE", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("SW", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("DE SW"));
            break;
          case LANGUAGE_DE_BA:
            renderer.setMenuText("DE", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("BA", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("DE BA"));
            break;
          case LANGUAGE_DE_SA:
            renderer.setMenuText("DE", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("SA", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("DE SA"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_DE_MKF
          case LANGUAGE_DE_MKF_DE:
            renderer.setMenuText("MK", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("MK"));
            break;
          case LANGUAGE_DE_MKF_SW:
            renderer.setMenuText("MK", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("SW", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("MK SW"));
            break;
          case LANGUAGE_DE_MKF_BA:
            renderer.setMenuText("MK", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("BA", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("MK BA"));
            break;
          case LANGUAGE_DE_MKF_SA:
            renderer.setMenuText("MK", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("SA", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("MK SA"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_D3
          case LANGUAGE_D3:
            renderer.setMenuText("D3", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("D3"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_CH
          case LANGUAGE_CH:
            renderer.setMenuText("CH", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("CH"));
            break;
          case LANGUAGE_CH_X:
            renderer.setMenuText("CH", Renderer::TEXT_POS_TOP, matrix);
            renderer.setMenuText("GS", Renderer::TEXT_POS_BOTTOM, matrix);
            DEBUG_PRINTLN(F("CH GS"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_EN
          case LANGUAGE_EN:
            renderer.setMenuText("EN", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("EN"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_FR
          case LANGUAGE_FR:
            renderer.setMenuText("FR", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("FR"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_IT
          case LANGUAGE_IT:
            renderer.setMenuText("IT", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("IT"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_NL
          case LANGUAGE_NL:
            renderer.setMenuText("NL", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("NL"));
            break;
#endif
#ifdef ENABLE_LANGUAGE_ES
          case LANGUAGE_ES:
            renderer.setMenuText("ES", Renderer::TEXT_POS_MIDDLE, matrix);
            DEBUG_PRINTLN(F("ES"));
            break;
#endif
          default:
            ;
        }
        break;
      case EXT_MODE_JUMP_TIMEOUT:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("FB", Renderer::TEXT_POS_TOP, matrix);
        for (byte i = 0; i < 7; i++) {
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getJumpToNormalTimeout() / 10][i])) << 10;
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[settings.getJumpToNormalTimeout() % 10][i])) << 5;
        }
        DEBUG_PRINT(F("FB "));
        DEBUG_PRINTLN(settings.getJumpToNormalTimeout());
        break;
#ifdef USE_EXT_MODE_TITLES
      case EXT_MODE_TEXT_TIME:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("TI", Renderer::TEXT_POS_TOP, matrix);
        renderer.setMenuText("ME", Renderer::TEXT_POS_BOTTOM, matrix);
        DEBUG_PRINTLN(F("TIME"));
        break;
#endif
#ifdef USE_EXT_MODE_IT_IS
      case EXT_MODE_IT_IS:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("IT", Renderer::TEXT_POS_TOP, matrix);
        if (settings.getEsIst()) {
          renderer.setMenuText("EN", Renderer::TEXT_POS_BOTTOM, matrix);
          DEBUG_PRINTLN(F("IT EN"));
        } else {
          renderer.setMenuText("DA", Renderer::TEXT_POS_BOTTOM, matrix);
          DEBUG_PRINTLN(F("IT DA"));
        }
        break;
#endif
      case EXT_MODE_TIMESET:
        if (helperSeconds % 2 == 0) {
          renderer.clearScreenBuffer(matrix);
        } else {
          renderer.clearScreenBuffer(matrix);
          renderer.setMinutes(rtc.getHours(), rtc.getMinutes(), settings.getLanguage(), matrix);
          renderer.setCorners(rtc.getMinutes(), matrix);
          renderer.clearEntryWords(settings.getLanguage(), matrix);
          renderer.activateAMPM(rtc.getHours(), settings.getLanguage(), matrix);
        }
        DEBUG_PRINT(rtc.getHours());
        DEBUG_PRINT(F(":"));
        DEBUG_PRINT(rtc.getMinutes());
        DEBUG_PRINT(F(":"));
        DEBUG_PRINTLN(helperSeconds);
        break;
#ifdef USE_EXT_MODE_DATE_MANUALLY
      case EXT_MODE_YEARSET: // Einstellung Jahr
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("YY", Renderer::TEXT_POS_TOP, matrix);
        for (byte i = 0; i < 5; i++) {
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getYear() / 10][i])) << 10;
          matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getYear() % 10][i])) << 5;
        }
        DEBUG_PRINT(F("YY "));
        DEBUG_PRINTLN(rtc.getYear());
        break;
      case EXT_MODE_MONTHSET: // Einstellung Monat
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("MM", Renderer::TEXT_POS_TOP, matrix);
        if (rtc.getMonth() > 9) {
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getMonth() / 10][i])) << 10;
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getMonth() % 10][i])) << 5;
          }
        } else {
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getMonth() % 10][i])) << 8;
          }
        }
        DEBUG_PRINT(F("MM "));
        DEBUG_PRINTLN(rtc.getMonth());
        break;
      case EXT_MODE_DAYSET: // Einstellung Tag
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("DD", Renderer::TEXT_POS_TOP, matrix);
        if (rtc.getDate() > 9) {
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getDate() / 10][i])) << 10;
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getDate() % 10][i])) << 5;
          }
        } else {
          for (byte i = 0; i < 5; i++) {
            matrix[5 + i] |= pgm_read_byte_near(&(ziffernB[rtc.getDate() % 10][i])) << 8;
          }
        }
        DEBUG_PRINT(F("DD "));
        DEBUG_PRINTLN(rtc.getDate());
        break;
#endif
#ifdef USE_EXT_MODE_NIGHT_OFF
      case EXT_MODE_NIGHT_OFF:
        renderer.clearScreenBuffer(matrix);
        if (!fallBackCounter) {
          renderer.setMenuText("N", Renderer::TEXT_POS_TOP, matrix);
          renderer.setMenuText("OF", Renderer::TEXT_POS_BOTTOM, matrix);
          DEBUG_PRINTLN(F("N OF"));
        }
        else {
          if (helperSeconds % 2 == 0) {
            renderer.clearScreenBuffer(matrix);
          } else {
            renderer.clearScreenBuffer(matrix);
            renderer.setMinutes(settings.getNightModeTime(false)->getHours(), settings.getNightModeTime(false)->getMinutes(), settings.getLanguage(), matrix);
            renderer.clearEntryWords(settings.getLanguage(), matrix);
            renderer.activateAMPM(settings.getNightModeTime(false)->getHours(), settings.getLanguage(), matrix);
          }
          DEBUG_PRINT(settings.getNightModeTime(false)->getHours());
          DEBUG_PRINT(F(":"));
          DEBUG_PRINTLN(settings.getNightModeTime(false)->getMinutes());
        }
        break;
      case EXT_MODE_NIGHT_ON:
        renderer.clearScreenBuffer(matrix);
        if (!fallBackCounter) {
          renderer.setMenuText("N", Renderer::TEXT_POS_TOP, matrix);
          renderer.setMenuText("ON", Renderer::TEXT_POS_BOTTOM, matrix);
          DEBUG_PRINTLN(F("N ON"));
        }
        else {
          if (helperSeconds % 2 == 0) {
            renderer.clearScreenBuffer(matrix);
          } else {
            renderer.clearScreenBuffer(matrix);
            renderer.setMinutes(settings.getNightModeTime(true)->getHours(), settings.getNightModeTime(true)->getMinutes(), settings.getLanguage(), matrix);
            renderer.clearEntryWords(settings.getLanguage(), matrix);
            renderer.activateAMPM(settings.getNightModeTime(true)->getHours(), settings.getLanguage(), matrix);
          }
          DEBUG_PRINT(settings.getNightModeTime(true)->getHours());
          DEBUG_PRINT(F(":"));
          DEBUG_PRINTLN(settings.getNightModeTime(true)->getMinutes());
        }
        break;
#endif
#ifdef USE_EXT_MODE_TITLES
      case EXT_MODE_TEXT_TEST:
        renderer.clearScreenBuffer(matrix);
        renderer.setMenuText("TE", Renderer::TEXT_POS_TOP, matrix);
        renderer.setMenuText("ST", Renderer::TEXT_POS_BOTTOM, matrix);
        DEBUG_PRINTLN(F("TEST"));
        break;
#endif
#ifdef USE_EXT_MODE_TEST
      case EXT_MODE_TEST:
        renderer.clearScreenBuffer(matrix);
        renderer.setCorners(helperSeconds % 5, matrix);
        testColumn++;
        if (testColumn > 10) {
          testColumn = 0;
        }
        for (byte i = 0; i < 11; i++) {
          ledDriver.setPixelInScreenBuffer(testColumn, i, matrix);
        }
        DEBUG_PRINTLN(F("LED anim."));
        break;
#endif

      /******************************************************************************
         Matrix gerendert.
      ******************************************************************************/

      default:
        break;
    }

#ifdef IR_LETTER_OFF
    // Die LED mit dem IR-Sensor abschalten.
    IR_LETTER_OFF;
#endif

#ifdef DEBUG_MATRIX
    // Matrix auf der Konsole ausgeben.
    debug_matrix(matrix);
#endif

    // Die neu erstellte Matrix auf das Display schreiben.
    ledDriver.writeScreenBufferToMatrix(matrix, true, settings.getColor());
  }

  /******************************************************************************
     misc in loop()
  ******************************************************************************/

  // Die Fernbedinung abfragen und ggf. reagieren.
  unsigned long lastIrCode = 0;
  if (irrecv.decode(&irDecodeResults)) {
    lastIrCode = irTranslator.buttonForCode(irDecodeResults.value);
    irrecv.resume();
    DEBUG_PRINT("IR signal: ");
    DEBUG_PRINTLN(irDecodeResults.value);
  }
  if (lastIrCode != 0) {
    remoteAction(lastIrCode, &irTranslator);
  }

#ifdef USE_EXT_MODE_NIGHT_OFF
  // Display zeitgesteuert abschalten.
  if ((mode < EXT_MODE_START) && (mode != STD_MODE_NIGHT) && (settings.getNightModeTime(false)->getMinutesOfDay(0) == rtc.getMinutesOfDay(0)) && (helperSeconds == 0)) {
    mode = STD_MODE_NIGHT; // hier nicht setMode() verwenden
    ledDriver.shutDown();
  }
  if ((mode == STD_MODE_NIGHT) && (settings.getNightModeTime(true)->getMinutesOfDay(0) == rtc.getMinutesOfDay(0)) && (helperSeconds == 0)) {
    mode = lastMode;
    ledDriver.wakeUp();
  }
#endif

#ifdef ENABLE_SQW_LED
  // LED blinken lassen.
  rtc.statusLed(digitalRead(PIN_SQW_SIGNAL) == HIGH);
#endif

  // Die alte Matrix auf das Display schreiben.
  if ((mode != STD_MODE_BLANK) && (mode != STD_MODE_NIGHT)) {
    ledDriver.writeScreenBufferToMatrix(matrix, false, settings.getColor());
  }

}

/******************************************************************************
   ENDE der loop().
******************************************************************************/

/******************************************************************************
   Remote button pressed.
******************************************************************************/

void remoteAction(unsigned int irCode, IRTranslator * irTranslatorGeneric) {
  needsUpdateFromRtc = true;

  switch (irCode) {
    case REMOTE_BUTTON_TOGGLEBLANK:
      setDisplayToToggle();
      break;
    case REMOTE_BUTTON_BLANK:
      setDisplayToBlank();
      break;
    case REMOTE_BUTTON_RESUME:
      setDisplayToResume();
      break;
    case REMOTE_BUTTON_MODE:
      modePressed();
      break;
    case REMOTE_BUTTON_SECONDS:
      if (mode < STD_MODE_BLANK) {
        modePressed();
      } else {
        setMode(STD_MODE_NORMAL);
      }
      break;
  }

  if ((mode != STD_MODE_BLANK) &&
      (mode != STD_MODE_NIGHT)) {

    switch (irCode) {
      case REMOTE_BUTTON_MINUTE_PLUS:
        minutePlusPressed();
        break;
      case REMOTE_BUTTON_HOUR_PLUS:
        hourPlusPressed();
        break;
      case REMOTE_BUTTON_LDR:
        if (EXT_MODE_LDR_MODE == mode) {
          settings.setUseLdr(!settings.getUseLdr());
          if (!settings.getUseLdr()) {
            settings.setBrightness(ledDriver.getBrightness());
          }
        } else {
          setMode(EXT_MODE_LDR_MODE);
        }
        break;
      case REMOTE_BUTTON_BRIGHTER:
        settings.setUseLdr(false);
        if (STD_MODE_BRIGHTNESS == mode) {
          setDisplayBrighter();
        } else {
          setMode(STD_MODE_BRIGHTNESS);
        }
        break;
      case REMOTE_BUTTON_DARKER:
        settings.setUseLdr(false);
        if (STD_MODE_BRIGHTNESS == mode) {
          setDisplayDarker();
        } else {
          setMode(STD_MODE_BRIGHTNESS);
        }
        break;
      case REMOTE_BUTTON_EXTMODE:
        if (mode < EXT_MODE_START) {
          setMode(EXT_MODE_START);
        } else {
          modePressed();
        }
        break;
      case REMOTE_BUTTON_SETCOLOR:
        if ((irTranslatorGeneric->getColor() == color_rgb_continuous) && (settings.getColor() == color_rgb_continuous)) {
          settings.setColor(eColors::color_rgb_step);
        } else {
          settings.setColor(irTranslatorGeneric->getColor());
        }
        if (settings.getColor() > color_single_max) {
          setMode(EXT_MODE_COLOR);
        }
        ledDriver.resetWheelPos();
        break;
      case REMOTE_BUTTON_SETMODE:
        setMode(STD_MODE_NORMAL);
        break;
      case REMOTE_BUTTON_REGION:
        if (EXT_MODE_LANGUAGE == mode) {
          hourPlusPressed();
        } else {
          setMode(EXT_MODE_LANGUAGE);
        }
        break;
      case REMOTE_BUTTON_TIME_H_PLUS:
        if (mode == EXT_MODE_TIMESET) {
          incDecHours(true);
        } else
          hourPlusPressed();
        break;
      case REMOTE_BUTTON_TIME_H_MINUS:
        if (mode == EXT_MODE_TIMESET) {
          incDecHours(false);
        } else
          minutePlusPressed();
        break;
      case REMOTE_BUTTON_TIME_M_PLUS:
        if (mode == EXT_MODE_TIMESET) {
          incDecMinutes(true);
        } else
          hourPlusPressed();
        break;
      case REMOTE_BUTTON_TIME_M_MINUS:
        if (mode == EXT_MODE_TIMESET) {
          incDecMinutes(false);
        } else
          minutePlusPressed();
        break;
      case REMOTE_BUTTON_TRANSITION:
        settings.setTransitionMode(irTranslatorGeneric->getTransition());
        ledDriver.demoTransition();
        break;
      default:
        break;
    }
  }

  // Fallback für Funktionen welche eine eigene Taste auf der Fernbedienung haben.
  // Im Menue diesen Fallback jedoch verhindern.
  if ((irCode != REMOTE_BUTTON_TIME_H_PLUS)  &&
      (irCode != REMOTE_BUTTON_TIME_M_PLUS)  &&
      (irCode != REMOTE_BUTTON_TIME_H_MINUS) &&
      (irCode != REMOTE_BUTTON_TIME_M_MINUS) &&
      (irCode != REMOTE_BUTTON_HOUR_PLUS)    &&
      (irCode != REMOTE_BUTTON_MINUTE_PLUS)  &&
      (irCode != REMOTE_BUTTON_SECONDS)      &&
      (irCode != REMOTE_BUTTON_MODE)         &&
      (irCode != REMOTE_BUTTON_EXTMODE)) {

    switch (mode) {
#ifdef USE_STD_MODE_AMPM
      case STD_MODE_AMPM:
#endif
#ifdef USE_STD_MODE_SECONDS
      case STD_MODE_SECONDS:
#endif
#ifdef USE_STD_MODE_DATE
      case STD_MODE_DATE:
#endif
#ifdef USE_STD_MODE_TEMP
      case STD_MODE_TEMP:
#endif
      case STD_MODE_BRIGHTNESS:
      case EXT_MODE_LANGUAGE:
      case EXT_MODE_LDR_MODE:
        enableFallBackCounter(settings.getJumpToNormalTimeout());
        break;
      case EXT_MODE_COLOR:
        enableFallBackCounter(2);
        break;
      default:
        disableFallBackCounter();
        break;
    }
  }

  // Werte speichern (die Funktion speichert nur bei geaenderten Werten)...
  settings.saveToEEPROM();
}

/******************************************************************************
   "Mode" button pressed.
******************************************************************************/

void modePressed() {
  needsUpdateFromRtc = true;
  // Displaytreiber einschalten, wenn BLANK verlassen wird
  if (mode == STD_MODE_BLANK) {
    DEBUG_PRINTLN(F("LED-Driver: WakeUp"));
    ledDriver.wakeUp();
  }
  switch (mode) {
    // Durch Drücken der MODE-Taste den Nachtmodus verlassen
    case STD_MODE_NIGHT:
      setDisplayToToggle();
      break;
    default:
      mode++;
      break;
  }

  // Brightness ueberspringen, wenn automatische Helligkeit verwendet wird.
  if (settings.getUseLdr() && (mode == STD_MODE_BRIGHTNESS)) {
    mode++;
  }

  // End of menu. Return to time.
  if ((mode == STD_MODE_COUNT) || (mode == EXT_MODE_COUNT)) {
    setMode(STD_MODE_NORMAL);
  }

  // Set fallback in standard mode.
  switch (mode) {
#ifdef USE_STD_MODE_AMPM
    case STD_MODE_AMPM:
#endif
#ifdef USE_STD_MODE_SECONDS
    case STD_MODE_SECONDS:
#endif
#ifdef USE_STD_MODE_DATE
    case STD_MODE_DATE:
#endif
#ifdef USE_STD_MODE_TEMP
    case STD_MODE_TEMP:
#endif
    case STD_MODE_BRIGHTNESS:
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
    default:
      disableFallBackCounter();
  }

  // Turn off display.
  if (mode == STD_MODE_BLANK) {
    DEBUG_PRINTLN(F("LED-Driver: ShutDown"));
    ledDriver.shutDown();
  }

  // Save the last mode when going to sleep.
  if ((mode != STD_MODE_BLANK) && (mode != STD_MODE_NIGHT)) {
    lastMode = mode;
  }

  // Save changed settings.
  settings.saveToEEPROM();

  DEBUG_PRINT(F("Mode-button pressed, now: "));
  DEBUG_PRINTLN(mode);

}

/******************************************************************************
   "H+" button pressed.
******************************************************************************/

void hourPlusPressed() {
  needsUpdateFromRtc = true;
  DEBUG_PRINTLN(F("H+ Button pressed."));

  switch (mode) {
    case EXT_MODE_TIMESET:
      incDecHours(true);
      break;
#ifdef USE_EXT_MODE_DATE_MANUALLY
    case EXT_MODE_YEARSET:
      rtc.incYear();
      rtc.writeTime();
      break;
    case EXT_MODE_MONTHSET:
      rtc.incMonth();
      rtc.writeTime();
      break;
    case EXT_MODE_DAYSET:
      rtc.incDate();
      rtc.writeTime();
      break;
#endif
    case STD_MODE_BRIGHTNESS:
      setDisplayBrighter();
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
    case EXT_MODE_LDR_MODE:
      settings.setUseLdr(!settings.getUseLdr());
      if (!settings.getUseLdr()) {
        settings.setBrightness(ledDriver.getBrightness());
      }
      break;
    case EXT_MODE_TRANSITION:
      if (settings.getTransitionMode() == Settings::TRANSITION_MODE_MAX - 1) {
        settings.setTransitionMode(0);
      } else {
        settings.setTransitionMode(settings.getTransitionMode() + 1);
      }
      break;
    case EXT_MODE_COLOR:
      if (settings.getColor() == color_max) {
        settings.setColor((eColors)0);
      } else {
        settings.setColor((eColors)(settings.getColor() + 1));
      }
      if (settings.getColor() > color_single_max) {
        ledDriver.resetWheelPos();
      }
      break;
    case EXT_MODE_COLOR_CHANGE:
      if (settings.getColorChangeRate() < 10) {
        settings.setColorChangeRate(settings.getColorChangeRate() + 1);
      }
      break;
    case EXT_MODE_JUMP_TIMEOUT:
      if (settings.getJumpToNormalTimeout() < 99) {
        settings.setJumpToNormalTimeout(settings.getJumpToNormalTimeout() + 1);
      }
      break;
#ifdef USE_EXT_MODE_NIGHT_OFF
    case EXT_MODE_NIGHT_OFF:
      if (fallBackCounter > 0) {
        settings.getNightModeTime(false)->incHours();
      }
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
    case EXT_MODE_NIGHT_ON:
      if (fallBackCounter > 0) {
        settings.getNightModeTime(true)->incHours();
      }
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
#endif
    case EXT_MODE_LANGUAGE:
      settings.setLanguage(settings.getLanguage() + 1);
      if (settings.getLanguage() >= LANGUAGE_COUNT) {
        settings.setLanguage(0);
      }
      break;
#ifdef USE_EXT_MODE_IT_IS
    case EXT_MODE_IT_IS:
      settings.toggleEsIst();
      break;
#endif
#ifdef USE_EXT_MODE_TITLES
    case EXT_MODE_TEXT_MAIN:
      setMode(EXT_MODE_TEXT_TIME);
      break;
    case EXT_MODE_TEXT_TIME:
      setMode(EXT_MODE_TEXT_TEST);
      break;
    case EXT_MODE_TEXT_TEST:
      setMode(EXT_MODE_TEXT_MAIN);
      break;
#endif
    default:
      break;
  }
}

/******************************************************************************
   "M+" button pressed.
******************************************************************************/

void minutePlusPressed() {
  needsUpdateFromRtc = true;
  DEBUG_PRINTLN(F("M+ Button pressed."));

  switch (mode) {
    case EXT_MODE_TIMESET:
      incDecMinutes(true);
      break;
#ifdef USE_EXT_MODE_DATE_MANUALLY
    case EXT_MODE_YEARSET:
      rtc.incYear(-1);
      rtc.writeTime();
      break;
    case EXT_MODE_MONTHSET:
      rtc.incMonth(-1);
      rtc.writeTime();
      break;
    case EXT_MODE_DAYSET:
      rtc.incDate(-1);
      rtc.writeTime();
      break;
#endif
    case STD_MODE_BRIGHTNESS:
      setDisplayDarker();
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
    case EXT_MODE_LDR_MODE:
      settings.setUseLdr(!settings.getUseLdr());
      if (!settings.getUseLdr()) {
        settings.setBrightness(ledDriver.getBrightness());
      }
      break;
    case EXT_MODE_TRANSITION:
      if (settings.getTransitionMode() == 0) {
        settings.setTransitionMode(Settings::TRANSITION_MODE_MAX - 1);
      } else {
        settings.setTransitionMode(settings.getTransitionMode() - 1);
      }
      break;
    case EXT_MODE_COLOR:
      if (settings.getColor() == 0)
      {
        settings.setColor(color_max);
      } else {
        settings.setColor((eColors)(settings.getColor() - 1));
      }
      break;
    case EXT_MODE_COLOR_CHANGE:
      if (settings.getColorChangeRate() > 0) {
        settings.setColorChangeRate(settings.getColorChangeRate() - 1);
      }
      break;
    case EXT_MODE_JUMP_TIMEOUT:
      if (settings.getJumpToNormalTimeout() > 0) {
        settings.setJumpToNormalTimeout(settings.getJumpToNormalTimeout() - 1);
      }
      break;
#ifdef USE_EXT_MODE_NIGHT_OFF
    case EXT_MODE_NIGHT_OFF:
      if (fallBackCounter > 0) {
        settings.getNightModeTime(false)->incFiveMinutes();
      }
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
    case EXT_MODE_NIGHT_ON:
      if (fallBackCounter > 0) {
        settings.getNightModeTime(true)->incFiveMinutes();
      }
      enableFallBackCounter(settings.getJumpToNormalTimeout());
      break;
#endif
    case EXT_MODE_LANGUAGE:
      if (settings.getLanguage() == 0) {
        settings.setLanguage(LANGUAGE_COUNT - 1);
      } else {
        settings.setLanguage(settings.getLanguage() - 1);
      }
      break;
#ifdef USE_EXT_MODE_IT_IS
    case EXT_MODE_IT_IS:
      settings.toggleEsIst();
      break;
#endif
#ifdef USE_EXT_MODE_TITLES
    case EXT_MODE_TEXT_MAIN:
      setMode(EXT_MODE_TEXT_TEST);
      break;
    case EXT_MODE_TEXT_TIME:
      setMode(EXT_MODE_TEXT_MAIN);
      break;
    case EXT_MODE_TEXT_TEST:
      setMode(EXT_MODE_TEXT_TIME);
      break;
#endif
    default:
      break;
  }
}

/******************************************************************************
   Interrupt from PIN_SQW_SIGNAL.
******************************************************************************/

void updateFromRtc() {
  needsUpdateFromRtc = true;
  helperSeconds++;
  if (fallBackCounter > 0) {
    if (mode != STD_MODE_BLANK) {
      updateFallBackCounter();
    }
  }
}

/******************************************************************************
   Display on and off.
******************************************************************************/

void setDisplayToBlank() {
  mode = STD_MODE_BLANK;
  ledDriver.shutDown();
  DEBUG_PRINTLN(F("LED-Driver: ShutDown"));
}

void setDisplayToResume() {
  mode = lastMode;
  ledDriver.wakeUp();
  DEBUG_PRINTLN(F("LED-Driver: WakeUp"));
}

void setDisplayToToggle() {
  if ((mode != STD_MODE_BLANK) && (mode != STD_MODE_NIGHT)) {
    setDisplayToBlank();
  } else {
    setDisplayToResume();
  }
}

/******************************************************************************
   Set brightness of display.
******************************************************************************/

void setDisplayBrighter() {
  if ((!settings.getUseLdr()) && (settings.getBrightness() < LDR_MAX_PERCENT)) {
    int8_t b = settings.getBrightness() + (LDR_MAX_PERCENT - LDR_MIN_PERCENT) / 9;
    if (b > LDR_MAX_PERCENT) {
      b = LDR_MAX_PERCENT;
    }
    setDisplayBrightness(b);
  }
}

void setDisplayDarker() {
  if (!settings.getUseLdr() && (settings.getBrightness() > LDR_MIN_PERCENT)) {
    int8_t i = settings.getBrightness() - (LDR_MAX_PERCENT - LDR_MIN_PERCENT) / 9;
    if (i < LDR_MIN_PERCENT) {
      i = LDR_MIN_PERCENT;
    }
    setDisplayBrightness(i);
  }
}

void setDisplayBrightness(byte brightness) {
  settings.setBrightness(brightness);
  settings.saveToEEPROM();
  ledDriver.setBrightness(brightness);
}

/******************************************************************************
   Timeout to switch back to time.
******************************************************************************/

void enableFallBackCounter(byte timeoutSec) {
  fallBackCounter = timeoutSec;
}

void disableFallBackCounter() {
  fallBackCounter = 0;
}

void updateFallBackCounter() {
  fallBackCounter--;
  if (!fallBackCounter) {
#ifdef USE_EXT_MODE_NIGHT_OFF
    if ((mode != EXT_MODE_NIGHT_OFF) && (mode != EXT_MODE_NIGHT_ON)) {
      setMode(STD_MODE_NORMAL);
    }
#else
    setMode(STD_MODE_NORMAL);
#endif
  }
}

/******************************************************************************
   Set time.
******************************************************************************/

void incDecMinutes(boolean inc) {
  if (inc) {
    rtc.incMinutes();
  }
  else {
    rtc.decMinutes();
  }
  resetSeconds();
  DEBUG_PRINT(F("Minute is now: "));
  DEBUG_PRINTLN(rtc.getMinutes());
}

void incDecHours(boolean inc) {
  if (inc) {
    rtc.incHours();
  }
  else {
    rtc.decHours();
  }
  resetSeconds();
  DEBUG_PRINT(F("Hour is now: "));
  DEBUG_PRINTLN(rtc.getHours());
}

void resetSeconds() {
  rtc.setSeconds(0);
  rtc.writeTime();
  rtc.readTime();
  helperSeconds = rtc.getSeconds();
}

/******************************************************************************
   Set mode.
******************************************************************************/

void setMode(Mode a_mode) {
  mode = a_mode;
  lastMode = mode;
}

/******************************************************************************
   isCurrentTimeInNightRange.
******************************************************************************/

bool isCurrentTimeInNightRange() {
  return (((settings.getNightModeTime(false)->getMinutesOfDay(0) < settings.getNightModeTime(true)->getMinutesOfDay(0)) &&
           ((rtc.getMinutesOfDay(0) > settings.getNightModeTime(false)->getMinutesOfDay(0)) &&
            (rtc.getMinutesOfDay(0) < settings.getNightModeTime(true)->getMinutesOfDay(0)))) ||
          ((settings.getNightModeTime(false)->getMinutesOfDay(0) > settings.getNightModeTime(true)->getMinutesOfDay(0)) &&
           ((rtc.getMinutesOfDay(0) > settings.getNightModeTime(false)->getMinutesOfDay(0)) ||
            (rtc.getMinutesOfDay(0) < settings.getNightModeTime(true)->getMinutesOfDay(0)))));
}

/******************************************************************************
   Set EEPROM to defaults.
******************************************************************************/

void factoryReset() {
  DEBUG_PRINTLN(F("*** Set all defaults! *** "));
  settings.resetToDefault();
  settings.saveToEEPROM();
}

/******************************************************************************
   Set time from NTP-Server.
******************************************************************************/

boolean setTimeFromNtp(const char* ntpServerName) {
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  delay(1000);
  int cb = udp.parsePacket();
  if (cb) {
    DEBUG_PRINT("Packet received, length=");
    DEBUG_PRINTLN(cb);
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    DEBUG_PRINT("UTC time is ");
    DEBUG_PRINT((epoch % 86400L) / 3600); // Stunde
    DEBUG_PRINT(':');
    DEBUG_PRINT((epoch % 3600) / 60); // Minute
    DEBUG_PRINT(':');
    DEBUG_PRINTLN(epoch % 60); // Sekunde
    rtc.setHours(((epoch % 86400L) / 3600) + UTC_OFFSET);
    rtc.setMinutes((epoch % 3600) / 60);
    rtc.setSeconds(epoch % 60);
    rtc.writeTime();
    helperSeconds = rtc.getSeconds();
    DEBUG_PRINTLN("Time written to RTC.");
    return true;
  } else {
    DEBUG_PRINT("Sorry, no packet received.");
    return false;
  }
}

/******************************************************************************
   Send an NTP request to the time server at the given address.
******************************************************************************/

unsigned long sendNTPpacket(IPAddress & address)
{
  DEBUG_PRINTLN("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

/******************************************************************************
   DEBUG: Render the Matrix to console.
******************************************************************************/

#ifdef DEBUG_MATRIX
void debug_matrix(word debugMatrix[]) {
  const char buchstabensalat[][12] = {
    {'E', 'S', 'K', 'I', 'S', 'T', 'A', 'F', 'U', 'N', 'F', '2'},
    {'Z', 'E', 'H', 'N', 'Z', 'W', 'A', 'N', 'Z', 'I', 'G', '1'},
    {'D', 'R', 'E', 'I', 'V', 'I', 'E', 'R', 'T', 'E', 'L', '4'},
    {'V', 'O', 'R', 'F', 'U', 'N', 'K', 'N', 'A', 'C', 'H', '3'},
    {'H', 'A', 'L', 'B', 'A', 'E', 'L', 'F', 'U', 'N', 'F', 'A'},
    {'E', 'I', 'N', 'S', 'X', 'A', 'M', 'Z', 'W', 'E', 'I', '_'},
    {'D', 'R', 'E', 'I', 'P', 'M', 'J', 'V', 'I', 'E', 'R', '_'},
    {'S', 'E', 'C', 'H', 'S', 'N', 'L', 'A', 'C', 'H', 'T', '_'},
    {'S', 'I', 'E', 'B', 'E', 'N', 'Z', 'W', 'O', 'L', 'F', '_'},
    {'Z', 'E', 'H', 'N', 'E', 'U', 'N', 'K', 'U', 'H', 'R', '_'}
  };
  Serial.println(F("\033[0;0H")); // Set cursor to 0, 0 position in console.
  Serial.println(F(" -----------"));
  for (byte zeile = 0; zeile < 10; zeile++) {
    word leds = debugMatrix[zeile];
    char spalte[16];
    for (int i = 15; i >= 0; i--) {
      spalte[i] = ((leds & 1) ? buchstabensalat[zeile][i] : ' ');
      leds = leds >> 1;
    }
    Serial.print('|');
    for (byte i = 0; i < 11; i++) {
      Serial.print(spalte[i]);
    }
    Serial.print('|');
    Serial.println(spalte[11]); // Corner LEDs
  }
  Serial.println(F(" -----------"));
}
#endif

