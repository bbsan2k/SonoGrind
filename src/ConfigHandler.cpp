#include <EEPROM.h>
#include "ConfigHandler.h"
#include "Arduino.h"
#include "Grinder.h"

ConfigHandler::ConfigHandler()
{
  EEPROM.begin(CONFIG_LENGTH);
  EEPROM.get(CONFIG_START, this->Config);
  EEPROM.end();
  if (Config.config_new_b)
  {
    this->reset();
  }
}

void ConfigHandler::reset()
{
  DEBUG_PRINTLN("In Reset....");
  String hostname = WIFI_HOSTNAME + String("-") + String(ESP.getChipId());

  this->Config.ConstantGrindThreshold_u32 = GRINDING_CONSTANT_THRESH;
  this->Config.SingleGrindTime_u32 = GRINDING_TIME;
  this->Config.NoSingleGrind_u32 = 0U;

  hostname.toCharArray(this->Config.HostName, 32);

  this->Config.Relay_Pin_u8 = RELAY_PIN;
  this->Config.Button_Pin_u8 = BUTTON_PIN;
  this->Config.config_new_b = false;
  this->save();
  DEBUG_PRINT("Done!");
  ESP.restart();
}

unsigned long ConfigHandler::get_single_grind_time()
{
  return this->Config.SingleGrindTime_u32;
}

void ConfigHandler::set_single_grind_time(unsigned long time)
{
  this->Config.SingleGrindTime_u32 = time;
}

unsigned long ConfigHandler::get_constant_grind_threshold()
{
  return this->Config.ConstantGrindThreshold_u32;
}

void ConfigHandler::set_constant_grind_threshold(unsigned long time)
{
  this->Config.ConstantGrindThreshold_u32 = time;
}

unsigned char ConfigHandler::get_relay_pin()
{
  return this->Config.Relay_Pin_u8;
}

void ConfigHandler::set_relay_pin(unsigned char pin)
{
  this->Config.Relay_Pin_u8 = pin;
}

unsigned char ConfigHandler::get_button_pin()
{
  return this->Config.Button_Pin_u8;
}

void ConfigHandler::set_buton_pin(unsigned char pin)
{
  this->Config.Button_Pin_u8 = pin;
}

unsigned long ConfigHandler::get_no_grinds()
{
  return this->Config.NoSingleGrind_u32;
}

void ConfigHandler::register_grind()
{
  this->Config.NoSingleGrind_u32++;
}

String ConfigHandler::get_hostname()
{
  return this->Config.HostName;
}
void ConfigHandler::set_hostname(String hostname)
{
  hostname.toCharArray(this->Config.HostName, 32);
  DEBUG_PRINTLN(hostname + " " + this->Config.HostName);
}

void ConfigHandler::save()
{
  DEBUG_PRINT("In Save...");
  EEPROM.begin(CONFIG_LENGTH);
  EEPROM.put(CONFIG_START, this->Config);
  delay(300);
  EEPROM.commit();
  EEPROM.end();
  DEBUG_PRINTLN("Done!");
}
