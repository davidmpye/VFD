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
