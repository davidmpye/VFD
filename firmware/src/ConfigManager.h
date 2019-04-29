
#ifndef __CONFIGMANAGER_H__
#define  __CONFIGMANAGER_H__

#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Config.h"

class DMPClock;

struct configObject {
  bool led_autodim;
  bool led_backlight;
  int led_color_mode;

  String disp_welcomemsg;
  int disp_timeformat;
  int disp_dateformat;

};


class ConfigManager  {

      public:

       ConfigManager();
       ~ConfigManager();
       void begin();
       void resetToDefaults();
       void saveToFlash();
       const String loadParam(String name);
       void saveParam(String name, String value);

       String dumpConfig();
       void setClock(DMPClock*);
       configObject data;

     private:


       DMPClock *clock;



  };

  #endif
