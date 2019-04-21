/*
    VFD clock firmware (c) 2018 David Pye <davidmpye@gmail.com>

    http://www.davidmpye.com/VFD

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
extern "C" {
#include "user_interface.h"
}

#include <FS.h>
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ButtonHandler.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>
#include "Display.h"
#include "Config.h"
#include "ConfigManager.h"
#include <NtpClientLib.h>
#include <WebHandler.h>
#include <ArduinoJson.h>

char led_mode[sizeof(LED_MODE)];

Display display;
RTC_DS3231 rtc;
ButtonHandler buttonHandler;
WebHandler webHandler;
ConfigManager configManager;

void updateBrightness() {
  int b = analogRead(A0) / 4;
  if (b > 255) b = 255;  //In case b is 1024...
  display.setBrightness(255 - b);
}

void setRTC() {
  DateTime t = rtc.now(); //This will get the default time from above.
  tmElements_t y = buttonHandler.getDate(&t);
  tmElements_t x  = buttonHandler.getTime(&t);
  //Set the time
  rtc.adjust(DateTime(y.Year + 1970, y.Month, y.Day, x.Hour, x.Minute, x.Second));
}

void setupWifi() {
  IPAddress local_ip(192,168,0,1);
  IPAddress gateway(192,168,0,2);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig (local_ip, gateway, subnet);
  WiFi.softAP("ESP_Clock");
}

void loadConfig() {
    display.setTimeMode((TIME_MODE) configManager.loadParam("time_mode").toInt() );
    display.setLEDMode((LED_MODE)configManager.loadParam("led_mode").toInt() );
}

void saveConfig(){
  configManager.saveParam("time_mode", String(display.getTimeMode()));
  configManager.saveParam("led_mode", String(display.getLEDMode()));
  configManager.saveToFlash();
}

void setup() {
  //Button 1
  pinMode(D0, INPUT);
  //Button 2
  pinMode(D5, INPUT);
  //Button 3
  pinMode(3, INPUT);
  //Button 4
  pinMode(1, INPUT);

  SPIFFS.begin();

  configManager.begin();
  configManager.setConfigChangedCallback(loadConfig);
  loadConfig(); // Needs to happen before the display is started
  display.begin();
  Wire.begin(D2,D1);
  setupWifi();

  webHandler.setConfigManager(&configManager);
  webHandler.begin();

  display.setBrightness(0xFF);
  display.scrollMessage("HELLO...", 4);
//  buttonHandler.begin(D0, D5, 3, 1, &display);
}

void handleButtonEvent(BUTTON_EVENT e) {
  switch(e) {
    case BUTTON_A_SHORTPRESS:
      display.displayDate(rtc.now());
      display.update();
      delay(2000);
      break;

    case BUTTON_A_LONGPRESS:
      setRTC();
      break;

    case BUTTON_C_SHORTPRESS:
      switch (display.getTimeMode()) {
        case TWENTYFOURHR_MODE:
          display.setTimeMode(AMPM_MODE);
          break;
        case AMPM_MODE:
          display.setTimeMode(EPOCH_MODE);
          break;
        case EPOCH_MODE:
          display.setTimeMode(TWENTYFOURHR_MODE);
          break;
        }
        break;

      case BUTTON_B_SHORTPRESS:
        switch(display.getLEDMode()) {
          case RAINBOW_MODE:
            display.setLEDMode(COL_PER_NUM_MODE);
            break;
          case COL_PER_NUM_MODE:
            display.setLEDMode(COL_BY_TIME_MODE);
            break;
          case COL_BY_TIME_MODE:
            display.setLEDMode(STEALTH_MODE);
            break;
          case STEALTH_MODE:
            display.setLEDMode(RAINBOW_MODE);
            break;
          }
          break;

      case BUTTON_C_LONGPRESS:
        display.displayInt(analogRead(A0));
        display.update();
        delay(1000);
        break;

      case BUTTON_D_LONGPRESS:
        display.test();
        display.update();
        delay(1000);
        break;

        default:
          break;
  }
  // Only save relevant changes
  if(e & (BUTTON_B_SHORTPRESS | BUTTON_C_SHORTPRESS)) saveConfig();
}

void loop() {
  static unsigned long lastMillis = 0;
  static int lastSec = -1;
  DateTime t = rtc.now();
  //If the time has moved forward, we will update the display:
  if (t.second() != lastSec) {
     lastSec = t.second();
     display.displayTime(t);
  }

  if (millis() > lastMillis + 100) {
    //We update brightness and the display every 100mS
    lastMillis = millis();
    updateBrightness();
    display.update();
  }

  handleButtonEvent(buttonHandler.poll());
  webHandler.handleEvents();
  //Handle any other events.
  delay(10);
}
