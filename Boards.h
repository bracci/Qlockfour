/******************************************************************************
   Boards
******************************************************************************/

#ifndef BOARDS_H
#define BOARDS_H

/******************************************************************************
   Board: NODEMCU/ESP8266
   
   D0  = 16 = BUILTIN_LED
   D1  =  5 = PIN_WIRE_SCL
   D2  =  4 = PIN_WIRE_SDA
   D3  =  0 = SQW Interrupt
   D4  =  2 = IR Sensor
   D5  = 14 = Buzzer
   D6  = 12
   D7  = 13 = LED Stripe Clock
   D8  = 15 = LED Stripe Data
   D9  =  3
   D10 =  1
   A0  = A0 = LDR
******************************************************************************/

#ifdef BOARD_NODEMCU

#define PIN_SQW_LED      16
#define PIN_SQW_SIGNAL    0
#define PIN_IR_RECEIVER   2
#define PIN_BUZZER       14
#define PIN_LEDS_CLOCK   13
#define PIN_LEDS_DATA    15
#define PIN_LDR          A0

#endif

#endif

