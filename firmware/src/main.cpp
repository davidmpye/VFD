#include <Arduino.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "Display.h"
#include "Config.h"
const char *ssid = "";
const char *password = "";
const char *ota_hostname="espvfd";
const char *ota_password="";

IPAddress ip(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

Display display;
RTC_DS3231 rtc;

void updateBrightness() {
  int br = analogRead(A0);
  Serial.println(br);
   //Really bright is about 30.
   //Makerspace 'ambient light' is around 60.
   //Thumb over it is about 350
   if (br<70) display.setBrightness(0xFF);  // daylight
   else if (br < 150)  display.setBrightness(0xFF - (br)); //it's dull ish
   else display.setBrightness(25);  //It's really quite dark
 }

void setup() {
  Serial.begin(57600);
  display.begin();

  pinMode(D0, INPUT);
  pinMode(D5, INPUT);

  //Can't have serial debug output and use these pins as inputs for the buttons
  pinMode(1, INPUT);
  pinMode(3, INPUT);

  Wire.begin(D2,D1);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  //Set time here.
  //rtc.adjust(DateTime(2018,9, 19, 18, 5, 0));

  ArduinoOTA.onStart([]() {
    display.clear();
    Serial.println("Begin");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percent = progress / (total / 100);
      display.setTubeChar(1, percent/ 10);
      display.setTubeChar(0, percent %10);
      display.refreshDisplay();
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.begin();

  display.setBrightness(0xFF);
}

int lastSec = -1;

enum TIME_MODE {
  NORMAL_MODE,
  EPOCH_MODE,
};

TIME_MODE displayMode = NORMAL_MODE;

void loop() {
  //OTA update handler
  ArduinoOTA.handle();
   DateTime t = rtc.now();
   //If the time has moved forward, we will update the display:
   if (t.second() != lastSec) {
     lastSec = t.second(); 
     /*
     //EPOCH demo mode!
     if (t.second()%10 == 0) {
        display.clear();
        if (displayMode == NORMAL_MODE) displayMode = EPOCH_MODE;
        else displayMode = NORMAL_MODE;
     }
     */
     switch (displayMode) {
       case NORMAL_MODE:
        display.setTubeChar(7,t.hour()/10);
        display.setTubeChar(6,t.hour()%10);
        display.setTubeChar(4, t.minute()/10);
        display.setTubeChar(7,t.hour()/10);
        display.setTubeChar(6,t.hour()%10);
        display.setTubeChar(4, t.minute()/10);
        display.setTubeChar(3, t.minute()%10);
        display.setTubeChar(1, t.second()/10);
        display.setTubeChar(0, t.second()%10);
        break;
       case EPOCH_MODE:
        time_t epochtime = t.unixtime(); //!
        for (int i=0; i<8; i++) {
          display.setTubeChar(i, (epochtime>>(4*i)) & 0x0F);
        }
       break;
     }
     display.refreshDisplay();
     updateBrightness();
   }
   //process any outstanding wifi events etc.
   display.refreshLEDs();

   delay(100);
}
