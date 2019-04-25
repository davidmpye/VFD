#include "ConfigManager.h"
#include "Clock.h"

ConfigManager::ConfigManager() {
  clock = NULL;
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::resetToDefaults() {
  //LED settings.
  data.led_autodim = true;
  data.led_color_mode = 0;
  data.led_backlight = true;

  saveToFlash();
}

String ConfigManager::dumpConfig() {

}

void ConfigManager::begin() {
  //Load from flash.
}

void ConfigManager::saveToFlash() {
  //Save to flash.

}
