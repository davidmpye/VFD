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

#include "Display.h"

extern "C" {
  void stopWaveform(int);
}

Display::Display() {

}

Display::~Display() {

}

void Display::begin() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  //Setting the PWM frequency to ~50Hz makes the inductor in the boost converter hum far less.
  analogWriteFreq(50);
  //PWM range 0-255.
  analogWriteRange(256);

  //Set all tubes to OFF.
  memset(_displayData, 0x00, NUM_TUBES);
  //Set up the LEDs
  LEDS.addLeds<APA106,LED_PIN,RGB>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);;

  //Full brightness.
  brightness = 255;

  update();
}

void Display::displayTime(DateTime t) {
  clear();
  if (configManager->data.disp_timeformat == EPOCH_MODE) {
       time_t epochtime = t.unixtime(); //!
       for (int i=0; i<8; i++) {
         setTubeNumber(NUM_TUBES - 1 - i, (epochtime>>(4*i)) & 0x0F);
       }
  }
  else {
     //Normal modes...
     if (configManager->data.disp_timeformat == AMPM_MODE && t.hour() > 12) {
       //If it's PM, dont display a 0 as first digit.
       if ((t.hour() - 12 )/10 == 0) setTubeByte(0,0x00);
       else setTubeNumber(0,1);
       setTubeDP(0, true);
       setTubeNumber(1, (t.hour()-12)%10);
     }
     else {
       //Display as 24 hour mode.
       setTubeNumber(0,t.hour()/10);
       setTubeNumber(1,t.hour()%10);
     }

     if (NUM_TUBES == 6) {
       //These are the same for either ampm/24 hr mode
        setTubeNumber(2, t.minute()/10);
        setTubeNumber(3, t.minute()%10);
        setTubeNumber(4, t.second()/10);
        setTubeNumber(5, t.second()%10);
     }
     else if (NUM_TUBES == 8) {
     //These are the same for either ampm/24 hr mode
      setTubeNumber(3, t.minute()/10);
      setTubeNumber(4, t.minute()%10);
      setTubeNumber(6, t.second()/10);
      setTubeNumber(7, t.second()%10);
    }
   }
   if (false && NUM_TUBES == 8 && ( configManager->data.disp_timeformat == TWENTYFOURHR_MODE || configManager->data.disp_timeformat == AMPM_MODE)) {
     if (t.second() %2 == 0) {
       byte b;
       /*
       if (_separators == DASHES ) {
         b = 0x40;
       }
       else b = 0x48;
       setTubeByte(2, b);
       setTubeByte(5, b);
       */
     }
     else {
     //clear() will set these to blank anyway.
     }
   }
}

void Display::displayDate(DateTime t) {
  clear();
  if (NUM_TUBES == 6) {
    switch (configManager->data.disp_dateformat) {
      case DDMMYY_MODE:
        setTubeNumber(0, t.day()/10);
        setTubeNumber(1, t.day()%10);
        setTubeNumber(2, t.month()/10);
        setTubeNumber(3, t.month()%10);
        break;
      case MMDDYY_MODE:
        setTubeNumber(2, t.day()/10);
        setTubeNumber(3, t.day()%10);
        setTubeNumber(0, t.month()/10);
        setTubeNumber(1, t.month()%10);
      break;
    }
    setTubeNumber(4, (t.year()-2000)/10);
    setTubeNumber(5, (t.year()-2000)%10);
  }
  else if (NUM_TUBES ==8) {
    switch (configManager->data.disp_dateformat) {
      case DDMMYY_MODE:
        setTubeNumber(0, t.day()/10);
        setTubeNumber(1, t.day()%10);
        setTubeNumber(3, t.month()/10);
        setTubeNumber(4, t.month()%10);
        break;
      case MMDDYY_MODE:
        setTubeNumber(3, t.day()/10);
        setTubeNumber(4, t.day()%10);
        setTubeNumber(0, t.month()/10);
        setTubeNumber(1, t.month()%10);
      break;
    }
    setTubeNumber(6, (t.year()-2000)/10);
    setTubeNumber(7, (t.year()-2000)%10);
  }
}

void Display::displayInt(int x) {
  displayInt(x, 10);
}

void Display::displayInt(int x, int base) {
  clear();
  for (int i=0; i<NUM_TUBES; ++i) {
      setTubeNumber(NUM_TUBES - 1 -i, x%base);
      x /= base;
      if (x == 0) break;;
  }
}

void Display::scrollMessage(const char *message, int speed) {
  clear();
  size_t length = strlen(message);
  for (int i=0; i<NUM_TUBES+length; i++) {
    for (int t=0; t<NUM_TUBES; t++)
    if (i+1-(NUM_TUBES-t)<0 or i+1-(NUM_TUBES-t)>=length) {
      setTubeByte(t, 0x00);
    }
    else {
      setTubeChar(t, message[i+1-(NUM_TUBES-t)]);
    }
    update();
    delay(1000 / speed);
  }
}

void Display::clear() {
    memset(_displayData, 0x00, NUM_TUBES);
}

void Display::blank() {
  digitalWrite(LATCH_PIN, LOW);
  for (int i=0; i<NUM_TUBES; ++i) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x00);
  }
  digitalWrite(LATCH_PIN, HIGH);
}


void Display::fillLEDData() {
  static uint16_t rainbow_counter = 0;
  //LEDs are enabled, generate the appropriate colour patterns.
  switch (configManager->data.led_color_mode) {
    case 0:
      rainbow_counter++;
      fill_rainbow(leds, NUM_LEDS, rainbow_counter, 255/NUM_LEDS);
      break;
    case COL_PER_NUM_MODE:
      for (int i = 0; i < NUM_TUBES; ++i) {
        leds[i].setHue(25 * getTubeNumber(i)); //255 / 10 digits - sorry, no extra colors for hex...
      }
      break;
    case COL_BY_TIME_MODE:
      if (NUM_TUBES == 6) {
        uint8_t hue;
        // 24hr based colour regardless of time display mode
        hue  = (getTubeNumber(0) * 10 + getTubeNumber(1)) * 10;
        leds[0].setHue(hue);
        leds[1].setHue(hue);
        hue = (getTubeNumber(2) * 10 + getTubeNumber(3)) * 4;
        leds[2].setHue(hue);
        leds[3].setHue(hue);
        hue = (getTubeNumber(4) * 10 + getTubeNumber(5)) * 4;
        leds[4].setHue(hue);
        leds[5].setHue(hue);
      }
      else if (NUM_TUBES == 8) {
        uint8_t hue;
        hue  = (getTubeNumber(0) * 10 + getTubeNumber(1)) * 10;
        leds[0].setHue(hue);
        leds[1].setHue(hue);
        leds[2] = CRGB::Black;
        hue = (getTubeNumber(3) * 10 + getTubeNumber(4)) * 4;
        leds[3].setHue(hue);
        leds[4].setHue(hue);
        leds[5] = CRGB::Black;
        hue = (getTubeNumber(6) * 10 + getTubeNumber(7)) * 4;
        leds[6].setHue(hue);
        leds[7].setHue(hue);
      }
      break;
    default:
      break;
  }
}

void Display::updateTubes() {
  //Push out the new digit data to the shift registers
  digitalWrite(LATCH_PIN, LOW);
  for (int i=0; i<NUM_TUBES; ++i) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, _displayData[NUM_TUBES - 1 - i]);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

void Display::updateLEDs() {
  //This little dance is needed to make the LEDs not flicker.
  //Basically, we disable the PWM that does the dimming of the VFDs for the brief time of writing out
  //the LED data, so FastLED doesn't get interrupted.
  stopWaveform(OE_PIN);
  FastLED.show();
  analogWrite(OE_PIN, 255 - brightness);
}

void Display::update() {

  static unsigned long lastUpdate = -1;
  // Calculate the new data for the LEDs.

  //If we're due an LED update, update the animation.
  if (millis() > (lastUpdate + LED_ANIMATION_STEPTIME)) {
    lastUpdate = millis();

    fillLEDData();
    updateBrightness();

    updateLEDs();
    updateTubes();
  }

}

void Display::setConfigManager(ConfigManager * mgr) {
  configManager = mgr;
}

void Display::updateBrightness() {
  int b = analogRead(A0) / 4;
  if (b > 255) b = 255;  //In case b is 1024...
  uint8_t requestedBrightness = 255 - b;

  static uint8_t lastBrightness = 0;

  if (requestedBrightness>200) requestedBrightness = 255; //full brightness
  if (requestedBrightness<40)  requestedBrightness = 6; //uber dim.

  if (requestedBrightness > lastBrightness + 20 || requestedBrightness < lastBrightness - 20) {
    //Significant change.
    lastBrightness = requestedBrightness;
  }
  else {
    //Insignificant change, continue to target the last brightness level
    requestedBrightness = lastBrightness;
  }

  //If we've requested a large change in brightness, start making small steps towards it
  if (requestedBrightness >= brightness + 5) {
    brightness += 5;
  }
  else if (requestedBrightness <= brightness - 5) {
    brightness -= 5;
  }
  else {
    brightness = requestedBrightness;
  }

  if (brightness > LEDS_OFF_BRIGHTNESS_CUTOFF) LEDS.setBrightness(brightness);
  else {
    LEDS.setBrightness(0);
  }
  analogWrite(OE_PIN, 255 - brightness);
}

void Display::test() {
  clear();
  brightness = 255;
  for (int i = 0; i < NUM_TUBES; ++i) {
    for (int j=0; j<8; ++j) {
      setTubeByte(i, 0x01<<j);
      update();
      delay(100);
    }
    setTubeByte(i, 0xFF);
    update();
  }
  delay(1000);
}

void Display::setTubeByte(int tube, uint8_t b) {
    _displayData[tube] = b;
}

void Display::setTubeNumber(int tube, uint8_t n ) {
  if (n < 10) {
    setTubeChar(tube, n+'0');
  }
  else
  {
    setTubeChar(tube, n-10+'A');
  }
}

void Display::setTubeChar(int tube, char c) {
  _displayData[tube] = _fontTable[c-' '];
}

void Display::setTubeDP(int tube, bool enabled) {
  if (enabled ) _displayData[tube] |= 0x80;
  else _displayData[tube] &= ~0x80;
}

const uint8_t Display::getTubeByte(int tube) {
  return _displayData[tube];
}

const uint8_t Display::getTubeNumber(int tube) {
  uint8_t b = getTubeChar(tube) - '0';
  return b;
}

const char Display::getTubeChar(int tube) {
  uint8_t b = getTubeByte(tube) & ~0x80;
  for (int i=0; i<=sizeof(_fontTable) ; ++i) {
    if (b == _fontTable[i]) return i + ' ';
  }
  return 0x00;
}
