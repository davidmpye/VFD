
#ifndef __CONFIGMANAGER_H__
#define  __CONFIGMANAGER_H__

#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <IPAddress.h>
#include "Config.h"

class DMPClock;


enum WIFI_IP_MODE {
  WIFI_IP_STATIC,
  WIFI_IP_DHCP,
};

enum WIFI_MODE {
  WIFI_STANDALONE,
  WIFI_JOINNETWORK,
};

enum SEPARATOR_MODE {
   SEP_NONE,
   SEP_DASH,
   SEP_DOUBLE_DASH,
};
enum TIME_MODE {
  TWENTYFOURHR_MODE,
  AMPM_MODE,
  EPOCH_MODE,
};

enum DATE_MODE {
  DDMMYY_MODE,
  MMDDYY_MODE,
};

enum LED_MODE {
  RAINBOW_MODE,
  COL_PER_NUM_MODE,
  COL_BY_TIME_MODE,
};

struct configObject {
  int version;
  bool led_autodim;
  bool led_backlight;
  int led_color_mode;

  String disp_welcomemsg;
  int disp_timeformat;
  int disp_dateformat;
  int disp_separator;


  //Wifi settings
  int wifi_mode; //standalone or join
  String wifi_mdns_name;
  String wifi_ssid;
  String wifi_pw;
  int wifi_ip_mode; //DHCP or static
  //if static:

  IPAddress wifi_ip;
  IPAddress wifi_netmask;
  IPAddress wifi_gateway;
  IPAddress wifi_dnsserver;

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

       configObject data;

     private:
       const int configVersion = 1;




  };

  #endif
