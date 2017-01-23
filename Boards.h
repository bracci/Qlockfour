/*
   Boards
*/

#ifndef BOARDS_H
#define BOARDS_H

#include "pins_arduino.h"

/******************************************************************************
   Board: NODEMCU
   
   D0 = SQW LED
   D1 = I2C SCL
   D2 = I2C SDA
   D3 = SQW Interrupt
   D4 = IR Sensor
   D5 = Buzzer
   D6 = 
   D7 = LED Stripe Clock
   D8 = LED Stripe Data
   A0 = LDR
******************************************************************************/

#ifdef BOARD_NODEMCU

#define PIN_SQW_LED      LED_BUILTIN
#define PIN_SQW_SIGNAL   D3
#define PIN_IR_RECEIVER  D4
#define PIN_BUZZER       D5
#define PIN_LEDS_CLOCK   D7
#define PIN_LEDS_DATA    D8
#define PIN_LDR          A0

#endif

#endif
