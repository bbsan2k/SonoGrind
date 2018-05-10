#ifndef GRINDER_H
#define GRINDER_H


#define GRINDER_OTA

#define WIFI_THRESHOLD  5000

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINTLN(x)   Serial.println (x)
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTF(x, y)    Serial.printf (x, y)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTF(x)
#endif



#endif
