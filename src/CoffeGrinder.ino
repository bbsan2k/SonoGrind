#include <ESP8266WiFi.h>
#include "Grinder.h"
#include "ConfigHandler.h"
#include "WebGenerator.h"

#ifdef GRINDER_OTA
  #include <ArduinoOTA.h>
#endif


typedef enum
{
  SYSTEM_STATE_IDLE,
  SYSTEM_STATE_TIMED_GRINDING,
  SYSTEM_STATE_CONSTANT_GRINDING
} SystemState_e;

typedef enum
{
  BUTTON_OFF,
  BUTTON_ON
} Button_e;


unsigned long grindStartMillis = 0U;
unsigned long pressChangeMillis = 0U;
unsigned long initTime = 0U;

SystemState_e SystemState = SYSTEM_STATE_IDLE;
Button_e ButtonState = BUTTON_OFF;

ESP8266WebServer server(80);
ConfigHandler configHandler = ConfigHandler();


void setup_wifi()
{
  unsigned char cnt = 0;
  WiFi.hostname(configHandler.get_hostname());
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED && cnt < 10)
  {
    delay(1000);
    cnt++;
  }
}


void setup() {
  pinMode(configHandler.get_relay_pin(), OUTPUT);
  pinMode(configHandler.get_button_pin(), INPUT_PULLUP);
  digitalWrite(configHandler.get_relay_pin(), 0);

  initTime = millis();
  setup_wifi();
  server_setup();
  server_set_config_handler(&configHandler);

  #ifdef GRINDER_OTA
    ArduinoOTA.begin();
  #endif

  #ifdef DEBUG
    Serial.begin(115200);
  #endif
}

void loop() {

  if ((WiFi.getMode() != WIFI_AP)
        && ((WiFi.status() != WL_CONNECTED)  ))
  {
      DEBUG_PRINTLN("Need to start AP, no connection!");

      WiFi.mode(WIFI_AP);
      WiFi.softAP(configHandler.get_hostname().c_str());
  }

  #ifdef GRINDER_OTA
    ArduinoOTA.handle();
  #endif

  processInputs();
  processGrinding();
  processGrindingTime();
  server_main();

  delay(100);
}

void processGrinding()
{
  if ((SYSTEM_STATE_TIMED_GRINDING == SystemState) || (SYSTEM_STATE_CONSTANT_GRINDING == SystemState))
  {
    digitalWrite(configHandler.get_relay_pin(), 1);
  }
  else
  {
    digitalWrite(configHandler.get_relay_pin(), 0);
  }
}

void processGrindingTime()
{
  unsigned long currentMillis = millis();

  if (SystemState == SYSTEM_STATE_TIMED_GRINDING)
  {
    if (((currentMillis - grindStartMillis) > configHandler.get_single_grind_time())
         || (grindStartMillis == 0))
    {
      changeSystemState(SYSTEM_STATE_IDLE);
    }
  }

}

void processInputs()
{
  Button_e newState = (digitalRead(configHandler.get_button_pin()) == LOW) ? BUTTON_ON : BUTTON_OFF;


  unsigned long currentMillis = millis();

  if (newState != ButtonState)
  {
    if (newState == BUTTON_ON)
    {
      notifyButtonPress();
    }
    else if (SYSTEM_STATE_CONSTANT_GRINDING == SystemState)
    {
      changeSystemState(SYSTEM_STATE_IDLE);
    }

    pressChangeMillis = currentMillis;
    ButtonState = newState;
  }

  if (((currentMillis - pressChangeMillis) > configHandler.get_constant_grind_threshold() )
        && (pressChangeMillis != 0)
        && (ButtonState == BUTTON_ON)
        && (SYSTEM_STATE_CONSTANT_GRINDING != SystemState))
  {
    changeSystemState(SYSTEM_STATE_CONSTANT_GRINDING);
  }


}

void notifyButtonPress()
{
  if (SYSTEM_STATE_IDLE == SystemState)
  {
    changeSystemState(SYSTEM_STATE_TIMED_GRINDING);
  }
  else if (SYSTEM_STATE_TIMED_GRINDING == SystemState)
  {
    changeSystemState(SYSTEM_STATE_IDLE);
  }
}


void changeSystemState(SystemState_e newState)
{
  DEBUG_PRINT("Switching to state: ");
  switch (newState)
  {
    case SYSTEM_STATE_TIMED_GRINDING:
      DEBUG_PRINTLN("Grinding Timed");
      if (SystemState == SYSTEM_STATE_IDLE)
      {
        grindStartMillis = millis();
        SystemState = newState;
        configHandler.register_grind();
        configHandler.save();
      }
      break;
    case SYSTEM_STATE_CONSTANT_GRINDING:
      DEBUG_PRINTLN("Grinding Constant");
      if (SystemState == SYSTEM_STATE_TIMED_GRINDING)
      {
        grindStartMillis = millis();
        SystemState = newState;
      }
      break;
    case SYSTEM_STATE_IDLE:
      DEBUG_PRINTLN("Idle");
      SystemState = newState;
      break;
    default:
      break;
  }
}
