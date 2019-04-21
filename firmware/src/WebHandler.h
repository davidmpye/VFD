#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "ConfigManager.h"
#include "Display.h"
#ifndef __WEBHANDLER_H__
#define __WEBHANDLER_H__

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
    String generateInfo();

    ESP8266WebServer httpServer;
    WebSocketsServer *webSocketServer;

    ConfigManager *configManager;

    void handleParamChange(String param, String value);
};

#endif