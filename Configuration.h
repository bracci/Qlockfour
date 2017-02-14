/******************************************************************************
   Configuration.h
******************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/******************************************************************************
  Standard Menu
******************************************************************************/

// Show AM/PM.
#define USE_STD_MODE_AMPM

// Show seconds.
#define USE_STD_MODE_SECONDS

// Show date.
#define USE_STD_MODE_DATE

// Show temperature.
#define USE_STD_MODE_TEMP

// Set an alarm
#define USE_STD_MODE_ALARM

/******************************************************************************
  Extended Menu
******************************************************************************/

// MAIN/TIME/TEST titles.
#define USE_EXT_MODE_TITLES

// Languages.
#define ENABLE_LANGUAGE_DE
#define ENABLE_LANGUAGE_DE_MKF
#define ENABLE_LANGUAGE_D3
#define ENABLE_LANGUAGE_CH
#define ENABLE_LANGUAGE_EN
#define ENABLE_LANGUAGE_FR
#define ENABLE_LANGUAGE_IT
#define ENABLE_LANGUAGE_NL
#define ENABLE_LANGUAGE_ES

// Setup to disable "It is".
#define USE_EXT_MODE_IT_IS

// Setup date.
#define USE_EXT_MODE_DATE_MANUALLY

// Setup night on/off.
#define USE_EXT_MODE_NIGHT_OFF

// LED test.
#define USE_EXT_MODE_TEST

/******************************************************************************
  Settings
******************************************************************************/

// Hostname. Only A-Z, a-z, 0-9 and -
#define HOSTNAME "QLOCKFOUR-NodeMCU"

// OTA password.
#define OTA_PASS "1234"

// NTP-Server.
#define NTP_SERVER "pool.ntp.org"

// Offset from GMT/UTC.
#define UTC_OFFSET +1

// None technical zero.
//#define NONE_TECHNICAL_ZERO

// Temperature-Sensor.
#define TEMP_OFFSET 5

// Alarm.
#define MAX_BUZZ_TIME 5

// Board.
#define BOARD_NODEMCU

// LED on board.
#define ENABLE_SQW_LED

// LED-Driver.
#define LED_DRIVER_NEOPIXEL
//#define LED_DRIVER_LPD8806
//#define LED_DRIVER_LPD8806RGBW

// LED-Layout.
#define LED_LAYOUT_WAAGERECHT
//#define LED_LAYOUT_SENKRECHT

// IR-Remote.
#define REMOTE_NO_REMOTE
//#define REMOTE_SPARKFUN
//#define REMOTE_MOONCANDLES
//#define REMOTE_LUNARTEC
//#define REMOTE_CLT2
//#define REMOTE_APPLE
//#define REMOTE_PHILIPS
//#define REMOTE_HX1838

// Turn off the LED behind the IR-Sensor.
//#define IR_LETTER_OFF_X 8
//#define IR_LETTER_OFF_Y 10

// LDR.
#define LDR_MIN_PERCENT 5
#define LDR_MAX_PERCENT 100
#define LDR_HYSTERESE 50
#define LDR_CHECK_RATE 50

/******************************************************************************
  Debug to serial console.
******************************************************************************/

#define SERIAL_SPEED 57600    // Set speed for serial console.
//#define DEBUG               // Switch on debug.
//#define DEBUG_TIME          // Shows the time every secound.
//#define DEBUG_MATRIX        // Renders the matrix to console - German front - Works best with Putty.
//#define DEBUG_SET_DEFAULTS  // Sets the EEPROM to defauls on every startup.

#endif
