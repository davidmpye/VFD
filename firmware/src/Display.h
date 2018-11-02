#ifndef __DISPLAY_H__

#include <FastLED.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>
#include "Config.h"

enum TIME_MODE {
  TWENTYFOURHR_MODE,
  AMPM_MODE,
  EPOCH_MODE,
};

enum DATE_MODE {
  DDMMYY_MODE,
  MMDDYY_MODE,
};

class Display {
  public:

    Display();
    ~Display();

    void begin();

    //Higher level functions
    void displayTime(DateTime t, TIME_MODE);
    void displayDate(DateTime d, DATE_MODE);

    void clear();

    void refreshDisplay();
    void refreshLEDs();
    void setBrightness(uint8_t);

    //Lower level manipulations.
    void setTubeByte(int tube, uint8_t b) ;
    void setTubeChar(int tube, char c);
    void setTubeDP(int tube, bool p);

    uint8_t getTubeByte(int tube);
    char getTubeChar(int tube);


  private:
  uint8_t brightness;
  uint16_t rainbow_counter;
  uint8_t _displayData[NUM_TUBES];

  CRGB leds[NUM_LEDS];

  //Now with added hex...
  uint8_t _fontTable[16] = {
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F,
    0x77,
    0x7C,
    0x39,
    0x5E,
    0x79,
    0x71
  };
};

#define __DISPLAY_H__
#endif
