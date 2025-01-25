#include "ConfigManager.h"
#include "Clock.h"

ConfigManager::ConfigManager() {

}

ConfigManager::~ConfigManager() {
}

void ConfigManager::resetToDefaults() {
  //LED settings.
  data.version = configVersion;
  data.led_backlight = true;
  data.led_autodim = true;
  data.led_color_mode = 0;

  data.disp_welcomemsg = "HELLO...";
  data.disp_timezone = TZ_Europe_London;
  data.disp_timeformat  =  TWENTYFOURHR_MODE;
  data.disp_dateformat = DDMMYY_MODE;
  data.disp_separator = SEP_NONE;

  //WiFi settings.
  data.wifi_mode = WIFI_STANDALONE;
  data.wifi_ssid = "VFD_CLOCK_" + WiFi.macAddress().substring(12,14) + WiFi.macAddress().substring(15,17);
  data.wifi_pw = "VFDCLOCK";
  data.wifi_mdns_name = "VFD_CLOCK";

  data.wifi_ip_mode = WIFI_IP_DHCP;
  data.wifi_ip = IPAddress(192,168,0,1);
  data.wifi_netmask = IPAddress(255,255,255,0);
  data.wifi_gateway = IPAddress(192,168,0,199);

  saveToFlash();
}

void ConfigManager::begin() {
  if (!SPIFFS.exists("/config.json")) {
    resetToDefaults();
    return;
  }
  //Load from flash.
  // File exists, reading and loading
  File configFile=SPIFFS.open("/config.json","r");
  if (configFile) {
    size_t size=configFile.size();
    char buf[size];
    configFile.readBytes(buf, size);
    DynamicJsonDocument doc(2048);
    if (!deserializeJson(doc, buf))  {
      // List of params to load
      //LED settings
      data.version = doc["version"];
      data.led_backlight = doc["led_backlight"] == "true" ? true : false;
      data.led_autodim = doc["led_autodim"]  == "true" ? true : false;
      data.led_color_mode = doc["led_color_mode"];
      //Display settings
      data.disp_welcomemsg = doc["disp_welcomemsg"].as<String>();
      data.disp_timeformat = doc["disp_timeformat"];
      data.disp_dateformat = doc["disp_dateformat"];
      data.disp_separator = doc["disp_separator"];
      //WiFi settings.
      data.wifi_mode = doc["wifi_mode"];
      data.wifi_ssid = doc["wifi_ssid"].as<String>();
      data.wifi_pw  = doc["wifi_pw"].as<String>();
      data.wifi_mdns_name = doc["wifi_mdns_name"].as<String>();
      data.wifi_ip_mode = doc["wifi_ip_mode"];
      data.wifi_ip.fromString(doc["wifi_ip"].as<String>());
      data.wifi_netmask.fromString(doc["wifi_netmask"].as<String>());
      data.wifi_gateway.fromString(doc["wifi_gateway"].as<String>());
      configFile.close();
    }
  }

  if (data.version != configVersion) {
    //Either we've failed to load the config file, or the version number has been
    //changed, so reset everything
    resetToDefaults();
  }
}


void ConfigManager::saveToFlash() {
  //Save to flash.
  DynamicJsonDocument doc(2048);

  doc["version"] = data.version;
  doc["led_backlight"] = data.led_backlight ? String("true") : String("false");
  doc["led_autodim"] = data.led_autodim ? String("true") : String("false");
  doc["led_color_mode"] = data.led_color_mode;

  doc["disp_welcomemsg"] = data.disp_welcomemsg;
  doc["disp_timeformat"] = data.disp_timeformat;
  doc["disp_dateformat"] = data.disp_dateformat;
  doc["disp_separator"] = data.disp_separator;

  //WiFi settings.
  doc["wifi_mode"] = data.wifi_mode;
  doc["wifi_ssid"] = data.wifi_ssid;
  doc["wifi_pw"] = data.wifi_pw;
  doc["wifi_mdns_name"] = data.wifi_mdns_name;
  doc["wifi_ip_mode"] = data.wifi_ip_mode;
  doc["wifi_ip"] = data.wifi_ip.toString();
  doc["wifi_netmask"] =  data.wifi_netmask.toString();
  doc["wifi_gateway"] = data.wifi_gateway.toString();

  // Save params
  File configFile=SPIFFS.open("/config.json", "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
  }
}
