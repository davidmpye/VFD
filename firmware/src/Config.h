#ifndef __CONFIG__H__
#define __CONFIG__H__//Pin definitions for Wemos->data lines
#define LED_PIN D6
#define NUM_TUBES 8
#define NUM_LEDS 8

#define OE_PIN D7
#define LATCH_PIN D8
#define DATA_PIN D4
#define CLOCK_PIN D3

//For the button ButtonHandler
#define LONG_BUTTON_PRESS_TIME 1000 //how many ms needed to register as a long press

//If brightness is set below this cutoff, the LEDS will go off.
#define LEDS_OFF_BRIGHTNESS_CUTOFF 15

#endif
