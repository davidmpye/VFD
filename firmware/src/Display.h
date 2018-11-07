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

enum LED_MODE {
  RAINBOW_MODE,
  COL_PER_NUM_MODE,
};

class Display {
  public:

    Display();
    ~Display();

    void begin();

    //Higher level functions
    //Displays the current time on the tubes, using TIME_MODE format
    void displayTime(DateTime t);
    //Displays the current date on the tubes, using the DATE_MODE format
    void displayDate(DateTime d);

    void clear(); //clears display and data
    void blank(); //clears the display, but doesnt clear data, so calling refreshDisplay will unblank it.

    void refreshDisplay(); //Updates the contents of the display with the results of the settubebyte/char/dp functions.
    void refreshLEDs(); //Updates the LED colours as per  LED_MODE

    void setBrightness(uint8_t); //Sets the brightness of the tubes (and LEDs) - for the tubes, it uses PWM, for the LEDs it uses Fastled.

    void setTimeMode(TIME_MODE m);
    void setDateMode(DATE_MODE m);
    void setLEDMode(LED_MODE m);

    const TIME_MODE getTimeMode();
    const DATE_MODE getDateMode();
    const LED_MODE getLEDMode();

    //Lower level manipulations.
    void setTubeByte(int tube, uint8_t b) ;
    void setTubeChar(int tube, char c);
    void setTubeDP(int tube, bool p);

    const uint8_t getTubeByte(int tube);
    const char getTubeChar(int tube);

  private:
  uint8_t brightness;
  uint16_t rainbow_counter;
  uint8_t _displayData[NUM_TUBES];

  CRGB leds[NUM_LEDS];

  //Now with added hex... This contains the font data for 0-9, A-F
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

  DATE_MODE _dateMode = DDMMYY_MODE;
  TIME_MODE _timeMode = TWENTYFOURHR_MODE;
  LED_MODE _ledMode = RAINBOW_MODE;
};

#define __DISPLAY_H__
#endif
