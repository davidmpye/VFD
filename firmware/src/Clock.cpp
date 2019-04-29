#include "Clock.h"





DMPClock::DMPClock() {


}

DMPClock::~DMPClock() {


}

void DMPClock::begin() {
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

    //If you hold down Button 1 and Button 2 at powerup, it will resetToDefaults
    if (digitalRead(D0) == 0 && digitalRead(D5) == 0) {
      configManager.resetToDefaults();
    }

    display.setConfigManager(&configManager);
    display.begin();

    Wire.begin(D2,D1);
    setupWifi();

    webHandler.setConfigManager(&configManager);
    webHandler.begin();

    display.scrollMessage(configManager.data.disp_welcomemsg.c_str(), 4);

    buttonHandler.setConfigManager(&configManager);
    buttonHandler.begin(D0, D5, 3, 1, &display);
}

void DMPClock::loop() {
    static int lastSec = -1;
    DateTime t = rtc.now();
    //If the time has moved forward, we will update the display:
    if (t.second() != lastSec) {
       lastSec = t.second();
       display.displayTime(t);
    }

    display.update();

    handleButtonEvent(buttonHandler.poll());
    webHandler.handleEvents();
    //Handle any other events.
    delay(10);
}


void DMPClock::setRTC() {
  DateTime t = rtc.now(); //This will get the default time from above.
  tmElements_t y = buttonHandler.getDate(&t);
  tmElements_t x  = buttonHandler.getTime(&t);
  //Set the time
  rtc.adjust(DateTime(y.Year + 1970, y.Month, y.Day, x.Hour, x.Minute, x.Second));
}


void DMPClock::setupWifi() {
  IPAddress local_ip(192,168,0,1);
  IPAddress gateway(192,168,0,2);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig (local_ip, gateway, subnet);
  WiFi.softAP("ESP_Clock");
}

void DMPClock::handleButtonEvent(BUTTON_EVENT e) {
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
      if (configManager.data.disp_timeformat == 2) {
        configManager.data.disp_timeformat = 0;
      }
      else configManager.data.disp_timeformat++;

    case BUTTON_B_SHORTPRESS:
        if (configManager.data.led_color_mode == 2)
          configManager.data.led_color_mode = 0;
        else configManager.data.led_color_mode++;
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
  if(e & (BUTTON_B_SHORTPRESS | BUTTON_C_SHORTPRESS)) configManager.saveToFlash();
}
