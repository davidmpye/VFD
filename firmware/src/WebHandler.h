#ifndef __WEBHANDLER_H__
#define __WEBHANDLER_H__

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include "Config.h"
#include "ConfigManager.h"


class WebHandler
{
    public:

       WebHandler();
       ~WebHandler();
      void begin();
      void setConfigManager(ConfigManager*);
      void handleEvents();

   private:
    String getContentType(String filename);
    bool handleFileRead(String path);
    void handleFileList();
    void  webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) ;
    String generateResponse(int);

    ESP8266WebServer httpServer;
    WebSocketsServer *webSocketServer;
    ESP8266HTTPUpdateServer httpUpdater;

    ConfigManager *configManager;

    void handleParamChange(String param, String value);
};

#endif
