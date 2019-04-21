#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Config.h"

  #ifndef __CONFIGHANDLER_H__
  #define  __CONFIGHANDLER_H__

  class  ConfigManager

  {
      public:

       ConfigManager();
       ~ConfigManager();
       void begin();
       void resetToDefaults();
       void saveToFlash();
       String loadParam(String name);
       void saveParam(String name, String value);

       String dumpConfig();

       void setConfigChangedCallback(void (*ptr)());

     private:

       DynamicJsonDocument *configStore;
       void (*changedCallback)(void);

  };

  #endif
void resetToDefaults();
void saveToFlash();
String loadParam(String name);
