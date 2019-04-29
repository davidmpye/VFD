#include "ConfigManager.h"
#include "Clock.h"

ConfigManager::ConfigManager() {
  clock = NULL;
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::resetToDefaults() {
  //LED settings.
  data.led_backlight = true;
  data.led_autodim = true;
  data.led_color_mode = 0;

  data.disp_welcomemsg = "HELLO...";

  data.disp_timeformat  =  0; // 0 = 24 hr, 1 = am/pm, 2 = epoch mode.
  data.disp_dateformat = 0;   //0 = DDMMYY, 1 = MMDDYY.
  saveToFlash();
}

String ConfigManager::dumpConfig() {
  return String();
}

void ConfigManager::begin() {
  //Load from flash.
  if (SPIFFS.exists("/config.json")) {
    // File exists, reading and loading
    File configFile=SPIFFS.open("/config.json","r");
    if (configFile) {
      size_t size=configFile.size();
      char buf[size];
      configFile.readBytes(buf, size);
      DynamicJsonDocument doc(1024);
      if (!deserializeJson(doc, buf))  {
        // List of params to load
        data.led_backlight = doc["led_backlight"] == "true" ? true : false;
        data.led_autodim = doc["led_autodim"]  == "true" ? true : false;
        data.led_color_mode = doc["led_color_mode"];

        data.disp_welcomemsg = doc["disp_welcomemsg"].as<String>();

        configFile.close();
      }
    }
  }
}

void ConfigManager::saveToFlash() {
  //Save to flash.
  DynamicJsonDocument doc(1024);

  doc["led_backlight"] = data.led_backlight ? String("true") : String("false");
  doc["led_autodim"] = data.led_autodim ? String("true") : String("false");
  doc["led_color_mode"] = data.led_color_mode;

  doc["disp_welcomemsg"] = data.disp_welcomemsg;

  // Save params
  File configFile=SPIFFS.open("/config.json", "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
  }
}
