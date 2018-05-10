#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include "Arduino.h"

#define GRINDING_TIME 7000
#define GRINDING_CONSTANT_THRESH 1000

#define WIFI_HOSTNAME "SonoGrind"

#define CONFIG_LENGTH 512
#define CONFIG_START 0

#ifdef DEBUG
  #define RELAY_PIN 13U
  #define BUTTON_PIN 0U
#else
  #define RELAY_PIN 12U
  #define BUTTON_PIN 14U
#endif


typedef struct
{
  bool config_new_b;
  char HostName[32];
  unsigned long SingleGrindTime_u32;
  unsigned long ConstantGrindThreshold_u32;
  unsigned long NoSingleGrind_u32;
  unsigned char Relay_Pin_u8;
  unsigned char Button_Pin_u8;
} Grinder_Config_t;


class ConfigHandler
{
public:
  ConfigHandler();
  void reset();
  unsigned long get_single_grind_time();
  void set_single_grind_time(unsigned long time);
  unsigned long get_constant_grind_threshold();
  void set_constant_grind_threshold(unsigned long time);
  unsigned char get_relay_pin();
  void set_relay_pin(unsigned char pin);
  unsigned char get_button_pin();
  void set_buton_pin(unsigned char pin);
  unsigned long get_no_grinds();
  void register_grind();
  String get_hostname();
  void set_hostname(String hostname);
  void save();
private:
  Grinder_Config_t Config;

};


#endif
