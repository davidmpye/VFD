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

#include <FS.h> // (SPIFFS) This needs to be first, or it all crashes and burns...
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
#include <NtpClientLib.h>
#include <DNSServer.h> // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> // Local WebServer used to serve the configuration portal
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson for storing configuration data

char led_mode[sizeof(LED_MODE)];

WiFiServer server(80); // Set web server port number to 80

const char *ota_hostname="espvfd";
const char *ota_password="";

Display display;
RTC_DS3231 rtc;

ButtonHandler buttonHandler;

void updateBrightness() {
  int b = analogRead(A0) / 4;
  if (b > 255) b = 255;  //In case b is 1024...
  display.setBrightness(255 - b);
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
      display.update();
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

void setupWifi() {
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("VFD-Clock"); // Name of temporary access point
  // Display IP
  if(WiFi.status()==WL_CONNECTED){
    IPAddress localIP=WiFi.localIP();
    for(int i=0; i<4; i++){
      int digitTO=666, digitGapTO=111, groupGapTO=444;
      display.clear();display.update();
      display.setTubeChar(0, localIP[i]/100);display.update();delay(digitTO);
      display.clear();display.update();delay(digitGapTO);
      display.setTubeChar(0, (localIP[i]%100)/10);display.update();delay(digitTO);
      display.clear();display.update();delay(digitGapTO);
      display.setTubeChar(0, (localIP[i]%100)%10);display.update();delay(digitTO);
      display.clear();display.update();delay(groupGapTO);
    }
    server.begin();
  }
}

void loadConfig(){ // Load the config from SPIFFS
  if(SPIFFS.begin()){
    if(SPIFFS.exists("/config.json")){
      // File exists, reading and loading
      File configFile=SPIFFS.open("/config.json","r");
      if(configFile){
        size_t size=configFile.size();
        std::unique_ptr<char[]> buf(new char[size]); // Buffer for config
        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument doc(1024);
        DeserializationError error=deserializeJson(doc, buf.get());
        if(error){
          // Failed to load json config
        }else{
          // List of params to load
          int time_mode=doc["time_mode"];
          int led_mode=doc["led_mode"];
          display.setTimeMode((TIME_MODE)time_mode);
          display.setLEDMode((LED_MODE)led_mode);
        }
        configFile.close();
      }
    }else{
    // Failed to mount FS
    }
  }
}

void saveConfig(){ // Save the config to SPIFFS
  DynamicJsonDocument doc(1024);

  // List of params to save
  TIME_MODE time_mode=display.getTimeMode();
  LED_MODE led_mode=display.getLEDMode();
  doc["time_mode"]=(int)time_mode;
  doc["led_mode"]=(int)led_mode;

  // Save params
  File configFile=SPIFFS.open("/config.json", "w");
  if(configFile){
    serializeJson(doc, configFile);
    configFile.close();
  }
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

  loadConfig(); // Needs to happen before the display is started
  display.begin();
  Wire.begin(D2,D1);

  //Do some wifi magic config
  setupWifi();
  //If we have some defined networks, try to connect to them.
  if (WiFi.status()==WL_CONNECTED) {
    setupOTA();
  }

  buttonHandler.begin(D0, D5, 3, 1, &display);
  display.setBrightness(0xFF);

  if (! rtc.begin()) {
    //Display error code 0xFF if RTC not available.
    display.clear();
    display.setTubeChar(1, 0xF);
    display.setTubeChar(0, 0xF);
    display.update();
    while(1) {
      //loop forever
      delay(1000);
    }
  }
  //Check to see if the RTC has lost time - if so, set the time
  if (rtc.lostPower()) {
    time_t timeT=0;
    //If Wifi is connected, try to get time via NTP
    if (WiFi.status() == WL_CONNECTED) {
      int retryCount=3;
      // Connect to NTP server
      for(int i=0; i<retryCount; i++){
        if(NTP.begin("pool.ntp.org", 0, true, 0)) break;
        delay(999);
      }
      NTP.setInterval(63);
      for(int i=0; i<retryCount; i++){
        timeT=NTP.getTime();
        // Set time by NTP
        if(timeT){
          rtc.adjust(DateTime(year(timeT), month(timeT), day(timeT),
            hour(timeT), minute(timeT), second(timeT)));
          break;
        }
        delay(999);
      }
    }
    // Set the time to midday jan 2001
    if(!timeT) rtc.adjust(DateTime(2001, 1, 1, 12, 0, 0));
  }

  display.hello();
  display.update();
  delay(1000);
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

void handleWebServer(){
  WiFiClient client=server.available();
  if(client){ // If a new client connects
    String header="";
    String currentLine=""; // make a String to hold incoming data from the client
    while(client.connected()){ // loop while the client's connected
      if(client.available()){ // if there's bytes to read from the client,
        char c=client.read(); // read a byte
        header+=c;
        if(c=='\n'){ // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row
          // that's the end of the client HTTP request, so send a response
          if(currentLine.length()==0){
            // act on http gets
            if (header.indexOf("GET /opt1/on") >= 0) {
            } else if (header.indexOf("GET /opt2/on") >= 0) {
            } else if (header.indexOf("GET /opt3/on") >= 0) {
            } else if (header.indexOf("GET /opt4/on") >= 0) {
            } else if (header.indexOf("GET /opt5/on") >= 0) {
            }
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Display the HTML web page
            client.println("<!DOCTYPE html><html><body><h1>ESP8266 Web Server</h1>");
            client.println("<p>Option1</p><p><a href=\"/opt1/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Option2</p><p><a href=\"/opt2/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Option3</p><p><a href=\"/opt3/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Option4</p><p><a href=\"/opt4/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Option5</p><p><a href=\"/opt5/on\"><button class=\"button\">ON</button></a></p>");
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          }else{ // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }else if(c!='\r'){ // if you got anything else but a carriage return character
          currentLine+=c; // add it to the end of the currentLine
        }
      }
    }
    header = ""; // Clear the header variable
    client.stop(); // Close the connection
  }
}

byte dash=false;
void loop() {
  static int lastSec = -1;

  DateTime t = rtc.now();
  //If the time has moved forward, we will update the display:
  if (t.second() != lastSec) {
     lastSec = t.second();
     display.displayTime(t);
  }

  updateBrightness();
  display.update();
  //Handle any button presses.
  handleButtonEvent(buttonHandler.poll());
  //process any outstanding OTA events
  ArduinoOTA.handle();
  //Handle web server requests
  handleWebServer();
  delay(100);
}
