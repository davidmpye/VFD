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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>

#include "Config.h"
#include "ConfigManager.h"

class Display {
  public:

    Display();
    ~Display();

    void begin();

    void setConfigManager(ConfigManager *);

    //Higher level functions
    //Displays the current time on the tubes, using TIME_MODE format
    void displayTime(DateTime t);
    //Displays the current date on the tubes, using the DATE_MODE format
    void displayDate(DateTime d);
    void displayInt(int val, int base);
    //defaults to base 10
    void displayInt(int val);

    void hello(); //displays the word hello as a welcome message.
    void scrollMessage(const char *message, int speed);

    void clear(); //clears display and data
    void blank(); //clears the display, but doesnt clear data, so calling refreshDisplay will unblank it.

    void setBrightness(); //Sets the brightness of the tubes (and LEDs) - for the tubes, it uses PWM, for the LEDs it uses Fastled's setBrightness.

    void update(); //Updates the tubes and LEDs.

    void test(); //Lights all segments on all tubes

    //Lower level manipulations.
    void setTubeByte(int tube, uint8_t b) ;
    void setTubeNumber(int tube, uint8_t n ) ;
    void setTubeChar(int tube, char c);
    void setTubeDP(int tube, bool p);

    const uint8_t getTubeNumber(int tube);
    const uint8_t getTubeByte(int tube);
    const char getTubeChar(int tube);

  private:
    void fillLEDData();
    void updateTubes();
    void updateLEDs();
    void updateBrightness();

  ConfigManager *configManager;

  uint8_t brightness;
  uint8_t _displayData[NUM_TUBES];

  CRGB leds[NUM_LEDS];

  uint8_t _fontTable[95] = {
    0x00,  // space
    0x82,  // !
    0x22,  // "
    0x00,  // #
    0x00,  // $
    0x00,  // %
    0x00,  // &
    0x02,  // '
    0x39,  // (
    0x0F,  // )
    0x00,  // *
    0x00,  // +
    0x04,  // ,
    0x40,  // -
    0x80,  // .
    0x00,  // /
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F,  // 9
    0x00,  // :
    0x00,  // ;
    0x00,  // <
    0x09,  // =
    0x00,  // >
    0x53,  // ?
    0x00,  // @
    0x77,  // A
    0x7C,  // B
    0x39,  // C
    0x5E,  // D
    0x79,  // E
    0x71,  // F
    0x3D,  // G
    0x76,  // H
    0x06,  // I
    0x1E,  // J
    0x00,  // K
    0x38,  // L
    0x00,  // M
    0x37,  // N
    0x3F,  // O
    0x73,  // P
    0x67,  // Q
    0x50,  // R
    0x6D,  // S
    0x78,  // T
    0x3E,  // U
    0x00,  // V
    0x00,  // W
    0x00,  // X
    0x6E,  // Y
    0x00,  // Z
    0x39,  // [
    0x01,  //
    0x0F,  // ]
    0x00,  // ^
    0x08,  // _
    0x02,  // `
    0x5F,  // a
    0x7C,  // b
    0x58,  // c
    0x5E,  // d
    0x7B,  // e
    0x71,  // f
    0x6F,  // g
    0x74,  // h
    0x04,  // i
    0x1E,  // j
    0x00,  // k
    0x38,  // l
    0x00,  // m
    0x54,  // n
    0x5C,  // o
    0x73,  // p
    0x67,  // q
    0x50,  // r
    0x6D,  // s
    0x78,  // t
    0x1C,  // u
    0x00,  // v
    0x00,  // w
    0x00,  // x
    0x6E,  // y
    0x00,  // z
    0x39,  // {
    0x30,  // |
    0x0F,  // }
  };
};

#endif
