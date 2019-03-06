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
  if (_timeMode == EPOCH_MODE) {
       time_t epochtime = t.unixtime(); //!
       for (int i=0; i<8; i++) {
         setTubeNumber(NUM_TUBES - 1 - i, (epochtime>>(4*i)) & 0x0F);
       }
  }
  else {
     //Normal modes...
     if (_timeMode == AMPM_MODE) {
       if (t.hour() > 12) {
         //If it's PM, dont display a 0 as first digit.
         if ((t.hour() - 12 )/10 == 0) setTubeByte(7,0x00);
         else setTubeNumber(7,1);
         setTubeDP(0, true);
         setTubeNumber(1, (t.hour()-12)%10);
       }
       else {
         //24 HR clock mode.
         setTubeNumber(0,t.hour()/10);
         setTubeNumber(1,t.hour()%10);
       }
     }
     else if (_timeMode == TWENTYFOURHR_MODE) {
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
   if (_dashes && NUM_TUBES == 8 && ( _timeMode == TWENTYFOURHR_MODE || _timeMode == AMPM_MODE )) {
     if (t.second() %2 == 0) {
       setTubeByte(2, 0x40);
       setTubeByte(5, 0x40);
     }
     else {
     //clear() will set these to blank anyway.
     }
   }
}

void Display::displayDate(DateTime t) {
  clear();
  if (NUM_TUBES == 6) {
    switch (_dateMode) {
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
    switch (_dateMode) {
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

void Display::scrollMessage(char *message, int speed) {
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

void Display::update() {
  // Calculate the new data for the LEDs.
  static long lastLEDUpdate = millis();

  bool ledRefreshNeeded = false;

  //If we're due an LED update, update the animation.
  if (millis() > lastLEDUpdate + LED_ANIMATION_STEPTIME) {

    lastLEDUpdate = millis();
    ledRefreshNeeded = true;


    switch (_ledMode) {
      case RAINBOW_MODE:
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
      case STEALTH_MODE:
        if (FastLED.getBrightness() > 0) {
          LEDS.setBrightness(0); //All LEDs off
          FastLED.show();
        }
        ledRefreshNeeded = false;
        break;
      default:
        break;

      //not implemented.
    }
  }

  //Push out the new digit data to the shift registers
  digitalWrite(LATCH_PIN, LOW);
  for (int i=0; i<NUM_TUBES; ++i) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, _displayData[NUM_TUBES - 1 - i]);
  }
  digitalWrite(LATCH_PIN, HIGH);

  if (ledRefreshNeeded) FastLED.show();
}

void Display::setBrightness (uint8_t requestedBrightness) {
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

  if (_ledMode != STEALTH_MODE) {
    if (brightness > LEDS_OFF_BRIGHTNESS_CUTOFF) LEDS.setBrightness(brightness);
    else LEDS.setBrightness(0);
  }

  analogWrite(OE_PIN, 255 - brightness);
}

void Display::setTimeMode(TIME_MODE m) {
  _timeMode = m;
}

void Display::setDateMode(DATE_MODE m) {
  _dateMode = m;
}

void Display::setLEDMode(LED_MODE m) {
  _ledMode = m;
}

const TIME_MODE Display::getTimeMode() {
  return _timeMode;
}

const DATE_MODE Display::getDateMode() {
  return _dateMode;
}

const LED_MODE Display::getLEDMode() {
  return _ledMode;
}

void Display::test() {
  //Set all tubes to display 8 to test all segments
  memset(_displayData, 0xFF, NUM_TUBES);
  brightness = 255;
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

void Display::enableDashes(bool b) {
  _dashes = b;
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
