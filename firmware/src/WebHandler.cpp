
#include <WebHandler.h>

WebHandler::WebHandler() {
  configManager = NULL;
}


WebHandler::~WebHandler() {

}

String WebHandler::getContentType(String filename){
  if(httpServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool WebHandler::handleFileRead(String path){
  if(path.endsWith("/") || path == "") path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = httpServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void WebHandler::handleFileList() {
  String path;
  if (!httpServer.hasArg("dir")) path = "/";
  else path = httpServer.arg("dir");

  Dir dir = SPIFFS.openDir(path);
  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  httpServer.send(200, "text/json", output);
}

void WebHandler::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
     //Ensure the datablock is null terminated.
     char datablock[length+1];
     memcpy(datablock, payload, length);
     datablock[length] = 0x00;
     String message(datablock);

     switch(type) {
          case WStype_TEXT:
            // send message to client
             switch (message.substring(0, message.indexOf(':')).toInt()) {
               case 0:
                webSocketServer->sendTXT(num, "{\"type\":\"sv.init.menu\",\"value\": [ \
                  {\"1\": { \"url\" : \"clock.html\", \"title\" : \"Clock\" }}, \
                  {\"2\": { \"url\" : \"leds.html\", \"title\" : \"LEDs\" }},   \
                  {\"3\": { \"url\" : \"extra.html\", \"title\" : \"Extra\" }}, \
			            {\"7\": { \"url\" : \"ups.html\", \"title\" : \"UPS\" }}, \
			            {\"8\": { \"url\" : \"sync.html\", \"title\" : \"Sync\" }}, \
			            {\"4\": { \"url\" : \"presets.html\", \"title\" : \"Presets\" }}, \
			            {\"5\": { \"url\" : \"info.html\", \"title\" : \"Info\" }}, \
			            {\"6\": { \"url\" : \"preset_names.html\", \"title\" : \"Preset Names\", \"noNav\" : true}}	  ] }");
                  break;
               case 1:
                break;

                case 5:
                    webSocketServer->sendTXT(num, String("{\"type\":\"sv.init.info\", \"value\":") +  generateInfo() + "}");
                    break;
                case 9:
                    //Paramter update. Message format is : 9:1:param:result
                    {
                      //find index of second and 3rd colons.
                    int secondColon = message.indexOf(':', message.indexOf(':') + 1);
                    int thirdColon = message.indexOf(':', secondColon+1);
                    handleParamChange(message.substring(secondColon+1, thirdColon), message.substring(thirdColon+1));
                  }
               default:
                break;
             }

            break;
          default:
            //We don't handle binary or other clever message types (yet)
            break;
      }
  }

void WebHandler::handleParamChange(String param, String val) {
  if (!configManager) return;
  //This doesnt work yet, needs to handle params properly!
  if (val == "true" ) {
      configManager->saveParam("led_mode", String(RAINBOW_MODE));
    }
    else  {
      configManager->saveParam("led_mode", String(STEALTH_MODE));
    }
}

String WebHandler::generateInfo() {
  DynamicJsonDocument doc(JSON_CONFIG_FILE_SIZE);
  doc["esp_boot_version"] = ESP.getBootVersion();
  doc["esp_free_heap"] = ESP.getFreeHeap();
  doc["esp_sketch_size"] = ESP.getSketchSize();
  doc["esp_sketch_space"] = ESP.getFreeSketchSpace();
  doc["esp_flash_size"] = ESP.getFlashChipSize();
  doc["esp_chip_id"] = ESP.getFlashChipId();
  doc["wifi_ip_address"] = WiFi.localIP().toString();
  doc["wifi_mac_address"] = WiFi.macAddress();
  doc["wifi_ssid"] = WiFi.SSID();

  if (configManager != NULL) doc["config_dump"] = configManager->dumpConfig();

  long millisecs = millis();
  doc["uptime"] = String( (millisecs / (1000 * 60 * 60 * 24)) % 365)  + " Days, " + String(millisecs / (1000 * 60 * 60) % 24)  + " Hours, " + String(millisecs / (1000 * 60) % 60) + " Mins";

  String returnStr;
  serializeJson(doc, returnStr);
  return returnStr;
}

void WebHandler::setConfigManager (ConfigManager *mgr) {
  configManager = mgr;
}

void WebHandler::begin() {
  httpServer.on("/list", HTTP_GET, [&](){ handleFileList() ; });
  //If not 'declared' above, try to load the file from SPIFFS.
  httpServer.onNotFound([&](){
    if(!handleFileRead(httpServer.uri()))
      httpServer.send(404, "text/plain", "404 Not Found - Have you forgotten to upload the flash filesystem? See wiki <a href='https://github.com/davidmpye/VFD/wiki/Firmware'>here</a>");
  });
  //Set up the http updater
  httpUpdater.setup(&httpServer);

  //Start the webserver
  httpServer.begin(WEBSERVER_PORT);
  //Configure and start the web socket server (this is used to make the UI responsive and receives change messages etc from the client)
  //NB If you change the port, you will need to change it in the javascript in app.html that is sent to the client, otherwise it wont connect
  webSocketServer = new WebSocketsServer(WEBSOCKET_PORT);
  webSocketServer->onEvent([&](uint8_t num, WStype_t type, uint8_t * payload, size_t length){ webSocketEvent(num, type, payload, length) ; });
  webSocketServer->begin();
}

void WebHandler::handleEvents() {
  //Handle events from both httpServer and the WebSocket Server
   httpServer.handleClient();
   webSocketServer->loop();
}
