#include "ConfigManager.h"
#include "Clock.h"

ConfigManager::ConfigManager() {
  configStore = new DynamicJsonDocument(JSON_CONFIG_FILE_SIZE);
  clock = NULL;
}

ConfigManager::~ConfigManager() {
    delete configStore;
}

void ConfigManager::resetToDefaults() {
  delete configStore;
  SPIFFS.remove("/config.json");
  configStore = new DynamicJsonDocument(JSON_CONFIG_FILE_SIZE);

  //LED settings.
  saveParam("led_backlight", "true");
  saveParam("led_autodim", "true");
  saveParam("led_color_mode", "0"); //rainbow

  saveToFlash();
}

String ConfigManager::dumpConfig() {
  String dump = ":";
  serializeJsonPretty(*configStore, dump);
  return dump;
}

void ConfigManager::begin() {
    File configFile=SPIFFS.open("/config.json","r");
  //  if (!configFile) {
      //There is no config file yet, so build the default one.
      resetToDefaults();
    //  return;
  //  }

    size_t size=configFile.size();
    std::unique_ptr<char[]> buf(new char[size]); // Buffer for config
    configFile.readBytes(buf.get(), size);
    if (deserializeJson(*configStore, buf.get()) != DeserializationError::Ok) {
        //Config file appears to be corrupt.   Reset will delete it and build a sane one.
        resetToDefaults();
    }
    configFile.close();
}

void ConfigManager::setClock(DMPClock *d) {
  clock = d;
}

String ConfigManager::loadParam(String name) {
  //Will return NULL if not present.
  return configStore->getMember(name).as<String>();
}

void ConfigManager::saveParam(String name, String value) {
  (*configStore)[name] = value;
  if (clock != NULL) clock->loadConfig();
  saveToFlash();
}

void ConfigManager::saveToFlash() {
  File configFile=SPIFFS.open("/config.json", "w");
  if (configFile){
    serializeJson(*configStore, configFile);
    configFile.close();
  }
}
