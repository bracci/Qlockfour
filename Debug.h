/******************************************************************************
   Debug
******************************************************************************/

#ifdef DEBUG
#define DEBUG_PRINT(x)       Serial.print(x)
#define DEBUG_PRINT2(x, y)   Serial.print(x, y)
#define DEBUG_PRINTLN(x)     Serial.println(x)
#define DEBUG_PRINTLN2(x, y) Serial.println(x, y)
#define DEBUG_FLUSH()        Serial.flush()
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINT2(x, y)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTLN2(x, y)
#define DEBUG_FLUSH()
#endif
