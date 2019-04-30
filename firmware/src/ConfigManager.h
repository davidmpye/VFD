
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

struct configObject {
  bool led_autodim;
  bool led_backlight;
  int led_color_mode;

  String disp_welcomemsg;
  int disp_timeformat;
  int disp_dateformat;


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

       String dumpConfig();
       void setClock(DMPClock*);
       configObject data;

     private:


       DMPClock *clock;



  };

  #endif
