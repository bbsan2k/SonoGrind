#ifndef WEBGENERATOR_H
#define WEBGENERATOR_H

#include "Arduino.h"
#include "ConfigHandler.h"
#include <ESP8266WebServer.h>    
#include <ESP8266WiFi.h>


void server_set_config_handler(ConfigHandler* const handler);
void server_setup();
void server_main();


#endif
