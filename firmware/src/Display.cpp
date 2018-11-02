#include "Display.h"

Display::Display() {
}

Display::~Display() {
}

void Display::begin() {
  analogWriteFreq(200);

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  //Set all tubes to OFF.
  memset(_displayData, 0x00, NUM_TUBES);
  refreshDisplay();
  //Set up the LEDs
  LEDS.addLeds<WS2812B,LED_PIN,RGB>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);;

  //All LEDs off.
  FastLED.clear();
  LEDS.setBrightness(255);
  FastLED.show();
  //Full brightness.
  brightness = 1024;
  analogWrite(OE_PIN, brightness);
}

void Display::setBrightness (uint8_t b) {
  brightness = b;
  LEDS.setBrightness(brightness);
  analogWrite(OE_PIN, 1024 -  (brightness*4));
}

void Display::setTubeByte(int tube, uint8_t b) {
    _displayData[tube] = b;
}

void Display::setTubeChar(int tube, char c) {
  _displayData[tube] = _fontTable[c];
}

uint8_t Display::getTubeByte(int tube) {
  return _displayData[tube];
}

char Display::getTubeChar(int tube) {
  uint8_t b = getTubeByte(tube) & ~0x80;
  for (int i=0; i<=16 ; ++i) {
    if (b == _fontTable[i]) return i;
  }
  return 0x00;
}


void Display::refreshDisplay() {
  //Update the VFD displays
  digitalWrite(LATCH_PIN, LOW);
  for (int i=0; i<NUM_TUBES; ++i) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, _displayData[i]);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

void Display::refreshLEDs() {
  rainbow_counter++;

  fill_rainbow(leds, NUM_LEDS, rainbow_counter, 255/NUM_LEDS);

  //Disable the Woftware PWM on OE pin as it interrupts fastled.
  analogWrite(OE_PIN, 0x00);

  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, HIGH);
  FastLED.show();
  //Re-enable the software PWM.
  analogWrite(OE_PIN, 1024 -  (brightness*4));
}

void Display::setTubeDP(int tube, bool enabled) {
  if (enabled ) _displayData[tube] |= 0x80;
  else _displayData[tube] &= ~0x80;
}

void Display::clear() {
  //Set all tubes to OFF.
  memset(_displayData, 0x00, NUM_TUBES);
  refreshDisplay();
}

void Display::displayDate(DateTime t, DATE_MODE mode) {
  clear();
  switch (mode) {
    case DDMMYY_MODE:
      setTubeChar(7, t.day()/10);
      setTubeChar(6, t.day()%10);
      setTubeChar(4, t.month()/10);
      setTubeChar(3, t.month()%10);
      break;
    case MMDDYY_MODE:
      setTubeChar(4, t.day()/10);
      setTubeChar(3, t.day()%10);
      setTubeChar(7, t.month()/10);
      setTubeChar(6, t.month()%10);
    break;
  }
  setTubeByte(5, 0x40);
  setTubeByte(2, 0x40);
  setTubeChar(1, (t.year()-2000)/10);
  setTubeChar(0, (t.year()-2000)%10);
  refreshDisplay();
}

void Display::displayTime(DateTime t, TIME_MODE mode) {
  clear();
  if (mode == EPOCH_MODE) {
       time_t epochtime = t.unixtime(); //!
       for (int i=0; i<8; i++) {
         setTubeChar(i, (epochtime>>(4*i)) & 0x0F);
       }
     }
     else {
       //Normal modes...
       if (mode == AMPM_MODE) {
         if (t.hour() > 12) {
           //If it's PM, dont display a 0 as first digit.
           if ((t.hour() - 12 )/10 == 0) setTubeByte(7,0x00);
           else setTubeChar(7,1);
           setTubeDP(7, true);
           setTubeChar(6, (t.hour()-12)%10);
         }
         else {
           setTubeChar(7,t.hour()/10);
           setTubeChar(6,t.hour()%10);
         }
       }
       else if (mode == TWENTYFOURHR_MODE) {
         setTubeChar(7,t.hour()/10);
         setTubeChar(6,t.hour()%10);

       }
       //These are the same for either ampm/24 hr mode
       setTubeChar(4, t.minute()/10);
       setTubeChar(3, t.minute()%10);
       setTubeChar(1, t.second()/10);
       setTubeChar(0, t.second()%10);
     }
     refreshDisplay();
}
