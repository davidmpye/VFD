#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ButtonHandler.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>
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

ButtonHandler buttonHandler;

void updateBrightness() {
  //Brightness updates are done based on an average of the pevious 5 seconds worth of brightness data
  static int brightness_vals[5];
  static int brightness_count = 0;

  brightness_vals[brightness_count] = analogRead(A0);
  int sum;
  for (int i=0; i<5; ++i) sum += brightness_vals[i];
  sum /= 5;
    //With 22K resistor in series with LDR:
  uint8_t brightness_val = 255 - (sum / 4);
  if (brightness_val < 30) brightness_val = 5;
  if (brightness_val > 150) brightness_val = 254;
  display.setBrightness(brightness_val);

  brightness_count++;
  if (brightness_count == 5) brightness_count = 0;
}

void setupOTA() {
  //These are the OTA callbacks.
  ArduinoOTA.onStart([]() {
    display.clear();
  });
  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percent = progress / (total / 100);
      display.setTubeChar(1, percent/ 10);
      display.setTubeChar(0, percent %10);
      display.refreshDisplay();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.begin();
}


void setRTC() {
  DateTime t = rtc.now(); //This will get the default time from above.
  tmElements_t y = buttonHandler.getDate(&t);
  tmElements_t x  = buttonHandler.getTime(&t);
  //Set the time
  rtc.adjust(DateTime(y.Year + 1970, y.Month, y.Day, x.Hour, x.Minute, x.Second));
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

  display.begin();
  Wire.begin(D2,D1);

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  setupOTA();

  buttonHandler.begin(D0, D5, 3, 1, &display);
  display.setBrightness(0xFF);

  if (! rtc.begin()) {
    //Display error code 0xFF if RTC not available.
    display.clear();
    display.setTubeChar(1, 'F');
    display.setTubeChar(0, 'F');
    while(1) {
      display.refreshDisplay();
      delay(100);
    }
  }
  //Check to see if the RTC has lost time - if so, set the time to midday jan 2001
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2001, 1, 1, 12, 0, 0));
  }
}


void handleButtonEvent(BUTTON_EVENT e) {
  int br = analogRead(A0);

  switch(e) {

    case BUTTON_A_SHORTPRESS:
      display.displayDate(rtc.now());
      delay(2000);
      break;

    case BUTTON_A_LONGPRESS:
      setRTC();
      break;

    case BUTTON_B_SHORTPRESS:
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

      case BUTTON_C_SHORTPRESS:
        switch(display.getLEDMode()) {
          case RAINBOW_MODE:
            display.setLEDMode(COL_PER_NUM_MODE);
            break;
          case COL_PER_NUM_MODE:
            display.setLEDMode(RAINBOW_MODE);
            break;
          }
          break;

      case BUTTON_C_LONGPRESS:
        display.clear();
        for (int i=0; i<4; ++i) {
          display.setTubeChar(i, br>>(4*i) & 0x0F);
        }
        display.refreshDisplay();
        delay(1000);
        break;

      case BUTTON_D_LONGPRESS:
        display.test();
        delay(1000);
        break;

        default:
          break;

    }
}

void loop() {
  static int lastSec = -1;
  DateTime t = rtc.now();
   //If the time has moved forward, we will update the display:
   if (t.second() != lastSec) {
     lastSec = t.second();
     display.displayTime(t);
     updateBrightness();
   }
   //Advance the LED effects.
   display.refreshLEDs();
   //Handle any button presses.
   handleButtonEvent(buttonHandler.poll());
   //process any outstanding OTA events
   ArduinoOTA.handle();
   delay(100);
}
