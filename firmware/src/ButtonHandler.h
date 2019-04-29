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

#ifndef __BUTTONHANDLER__
#define __BUTTONHANDLER__

#include <JC_Button.h>

#include "Display.h"

typedef enum BUTTON_EVENT {
  NO_PRESS,
  BUTTON_A_SHORTPRESS,
  BUTTON_A_LONGPRESS,
  BUTTON_B_SHORTPRESS,
  BUTTON_B_LONGPRESS,
  BUTTON_C_SHORTPRESS,
  BUTTON_C_LONGPRESS,
  BUTTON_D_SHORTPRESS,
  BUTTON_D_LONGPRESS,
};


class ButtonHandler {
  public:
    ButtonHandler();
    ~ButtonHandler();

    void begin(int a, int b, int c, int d, Display *e); //four pin numbers.

    BUTTON_EVENT poll();

    tmElements_t getTime(DateTime*);
    tmElements_t getDate(DateTime*);

    void setConfigManager(ConfigManager *);


  private:
    Button *buttonA, *buttonB, *buttonC, *buttonD;
    Display *display;
    ConfigManager *configManager;

};

#endif
