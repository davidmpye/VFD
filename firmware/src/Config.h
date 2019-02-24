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
#define LED_ANIMATION_STEPTIME 100 //mS

#endif
