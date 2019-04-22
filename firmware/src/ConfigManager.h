
#ifndef __CONFIGMANAGER_H__
#define  __CONFIGMANAGER_H__

#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Config.h"

class DMPClock;

class ConfigManager  {

      public:

       ConfigManager();
       ~ConfigManager();
       void begin();
       void resetToDefaults();
       void saveToFlash();
       String loadParam(String name);
       void saveParam(String name, String value);

       String dumpConfig();
       void setClock(DMPClock*);
     private:

       DynamicJsonDocument *configStore;
       DMPClock *clock;



  };

  #endif
