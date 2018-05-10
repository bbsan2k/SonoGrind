#include "WebGenerator.h"
#include "Grinder.h"

const char HTTP_TITLE[] PROGMEM           = "<h1>{t}</h1>";
const char HTTP_FORM_START_POST[] PROGMEM      = "<form method='post' action='{dest}'>";
const char HTTP_LINK[] PROGMEM       = "<p><form action='{dest}'><button type='submit'>{text}</button></form></p>";
const char HTTP_LABELED_PARAM[] PROGMEM     = "<p>{lbl}:<input id='{i}' name='{n}' type='{t}' length={l} placeholder='{p}' value='{v}'></p>";
const char HTTP_NO_OF_GRINDS[] PROGMEM = "<p><b>Current Number of Grinds: </b>{n}</p>";
const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";



static String create_head(String title);
static String create_link(String destination, String text);
static String create_number_of_grinds(uint32_t number_u32);
static String create_title(String title);
static String create_form_start(String destination);
static String create_input(String label, String name, String type, unsigned int length, String placeholder, String value);
static void handle_root();
static void handle_wifi();
static void handle_config();
static String generate_root();
static String generate_wifi();
static String generate_config();

static ESP8266WebServer server(80);
static ConfigHandler* configHandler;


static String create_input(String label, String name, String type, unsigned int length, String placeholder, String value)
{
  String message = FPSTR(HTTP_LABELED_PARAM);
  message.replace("{lbl}", label);
  message.replace("{i}", name);
  message.replace("{n}", name);
  message.replace("{t}", type);
  message.replace("{l}", ""+length);
  message.replace("{p}", placeholder);
  message.replace("{v}", value);
  return message;
}


static String create_head(String title)
{
  String message = FPSTR(HTTP_HEAD);
  message.replace("{v}", title);
  message += FPSTR(HTTP_STYLE);
  message += FPSTR(HTTP_HEAD_END);
  return message;
}

static String create_link(String destination, String text)
{
  String message = FPSTR(HTTP_LINK);
  message.replace("{dest}", destination);
  message.replace("{text}", text);

  return message;
}

static String create_number_of_grinds(uint32_t number_u32)
{
  String message = FPSTR(HTTP_NO_OF_GRINDS);
  message.replace("{n}", String(number_u32));
  return message;
}

static String create_title(String title)
{
  String message = FPSTR(HTTP_TITLE);
  message.replace("{t}", title);
  return message;
}

static String create_form_start(String destination)
{
  String message = FPSTR(HTTP_FORM_START_POST);
  message.replace("{dest}", destination);

  return message;
}


static String generate_root()
{
  String message = create_head("Choose Menu");
  message += create_title("Choose Menu");
  message += create_link("/wifi_config", "WiFi Config");
  message += create_link("/sonogrind_config", configHandler->get_hostname() + " Config");
  message += create_link("/reset_config", "Reset Config");
  message += create_link("/reset_sonogrind", "Reset " + configHandler->get_hostname());
  message += create_number_of_grinds(configHandler->get_no_grinds());
  message += FPSTR(HTTP_END);
  return message;
}

static String generate_wifi()
{
  String ssid = WiFi.SSID();

  String message = create_head("WiFi Config");
  message += create_title("WiFi Config");
  message += create_form_start("/wifi_config");
  message += create_input("SSID", "SSID", "text", 32, "SSID", ssid);
  message += create_input("Password", "PW", "password", 64, "Password", "");
  message += create_input("Hostname", "HN", "text", 32, "Hostname", configHandler->get_hostname());

  message += FPSTR(HTTP_FORM_END);
  message += create_link("/", "back");
  message += FPSTR(HTTP_END);

  return message;
}

static String generate_config()
{
  String message = create_head("SonoGrind Config");
  message += create_title("SonoGrind Config");
  message += create_form_start("/sonogrind_config");
  message += create_input("Single Grind Time (ms)", "SGT", "text", 65535, "Single Grind Time", String(configHandler->get_single_grind_time()));
  message += create_input("Constant Grind Threshold (ms)", "CGT", "text", 65535, "Constant Grind Threshold", String(configHandler->get_constant_grind_threshold()));
  message += create_input("Relais Pin", "RP", "text", 255, "Relais Pin", String(configHandler->get_relay_pin()));
  message += create_input("Button Pin", "BP", "text", 255, "Button Pin", String(configHandler->get_button_pin()));


  message += FPSTR(HTTP_FORM_END);
  message += create_link("/", "back");
  message += create_link("/reset_config", "Reset Config");
  message += create_link("/reset_sonogrind", "Reset SonoGrind");
  message += FPSTR(HTTP_END);

  return message;
}

static String generate_restart()
{
  String message = create_head("Restarted");
  message += create_title(configHandler->get_hostname() + " restarting...");
  message += create_link("/", "Main page");

  return message;
}

static void handle_root()
{
  DEBUG_PRINT("Generating Root Page...");
  server.send(200, "text/html", generate_root());
  DEBUG_PRINTLN("Done!");
}

static void handle_wifi()
{
  if (server.hasArg("HN") && server.arg("HN") != "")
  {
    configHandler->set_hostname(server.arg("HN"));
    configHandler->save();
  }

  if (server.hasArg("SSID") && server.hasArg("PW") && (server.arg("PW") != "") && server.hasArg("HN"))
  {
    unsigned char cnt = 0;
    String SSID = server.arg("SSID");
    String PW = server.arg("PW");
    WiFi.hostname(configHandler->get_hostname());
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID.c_str(), PW.c_str());

    while (WiFi.status() != WL_CONNECTED && cnt < 10)
    {
      delay(1000);
      cnt++;
    }

  }
  server.send(200, "text/html", generate_wifi());
}

static void handle_config()
{
  if (server.hasArg("SGT"))
  {
    configHandler->set_single_grind_time(server.arg("SGT").toInt());
  }
  if (server.hasArg("CGT"))
  {
    configHandler->set_constant_grind_threshold(server.arg("CGT").toInt());
  }
  if (server.hasArg("RP"))
  {
    configHandler->set_relay_pin(server.arg("RP").toInt());
  }
  if (server.hasArg("BP"))
  {
    configHandler->set_buton_pin(server.arg("BP").toInt());
  }

  configHandler->save();


  server.send(200, "text/html", generate_config());

}

void server_set_config_handler(ConfigHandler* const handler)
{
  configHandler = handler;
}

static void reset_config()
{
  server.send(200, "text/html", generate_restart());
  configHandler->reset();
}

static void restart_esp()
{
  server.send(200, "text/html", generate_restart());
  delay(200);
  ESP.restart();
}

void server_setup()
{
  server.on("/", handle_root);
  server.on("/wifi_config", handle_wifi);
  server.on("/sonogrind_config", handle_config);
  server.on("/reset_config", reset_config);
  server.on("/reset_sonogrind", restart_esp);
  server.begin();
}

void server_main()
{
    server.handleClient();
}
