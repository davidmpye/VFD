#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <FS.h>
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>
#include <NtpClientLib.h>
#include <ArduinoJson.h>


#include "Display.h"
#include "ButtonHandler.h"
#include "WebHandler.h"

class Clock {

    public:

  Clock();
  ~Clock();


  void begin();
  void loop();

  void loadConfig();

private:

  void updateBrightness();
  void setRTC();
  void setupWifi();
  void handleButtonEvent(BUTTON_EVENT e);

  Display display;
  RTC_DS3231 rtc;
  ButtonHandler buttonHandler;
  WebHandler webHandler;
  ConfigManager configManager;


};

#endif
