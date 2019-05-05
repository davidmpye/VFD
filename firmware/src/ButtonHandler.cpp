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

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler() {
}

ButtonHandler::~ButtonHandler() {
  delete buttonA;
  delete buttonB;
  delete buttonC;
  delete buttonD;
}

void ButtonHandler::setConfigManager(ConfigManager *mgr) {
  configManager = mgr;
}

void ButtonHandler::begin(int a, int b, int c, int d, Display *disp) {
  buttonA = new Button(a,25,false,true);
  buttonB = new Button(b,25,false,true);
  buttonC = new Button(c,25,false,true);
  buttonD = new Button(d,25,false,true);

  buttonA->begin();
  buttonB->begin();
  buttonC->begin();
  buttonD->begin();

  display = disp;
}

BUTTON_EVENT ButtonHandler::poll() {
  buttonA->read();
  buttonB->read();
  buttonC->read();
  buttonD->read();

  if (buttonA->pressedFor(LONG_BUTTON_PRESS_TIME)) {
    return  BUTTON_A_LONGPRESS;
  }
  if (buttonA->wasPressed()) {
    return BUTTON_A_SHORTPRESS;
  }
  if (buttonB->pressedFor(LONG_BUTTON_PRESS_TIME)) {
    return  BUTTON_B_LONGPRESS;
  }
  if (buttonB->wasPressed()) {
    return BUTTON_B_SHORTPRESS;
  }
  if (buttonC->pressedFor(LONG_BUTTON_PRESS_TIME)) {
    return  BUTTON_C_LONGPRESS;
  }
  if (buttonC->wasPressed()) {
    return BUTTON_C_SHORTPRESS;
  }
  if (buttonD->pressedFor(LONG_BUTTON_PRESS_TIME)) {
    return  BUTTON_D_LONGPRESS;
  }
  if (buttonD->wasPressed()) {
    return BUTTON_D_SHORTPRESS;
  }

  return NO_PRESS;
}

tmElements_t ButtonHandler::getDate(DateTime *t ) {
  configManager->data.disp_dateformat = DDMMYY_MODE;
  display->displayDate(*t);
  display->updateTubes();

  int tubecount = 0;
  int last_blinked = millis();

  //Wait until Button A is released.
  while (buttonA->read()) ;

  while(1) {

    buttonA->read();
    buttonB->read();

    int now = millis();

    if (now > last_blinked+1000) {
      char b = display->getTubeChar(tubecount);
      display->setTubeByte(tubecount, 0x00);
      display->updateTubes();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->updateTubes();
      last_blinked = now;
    }

    if (buttonB->wasPressed()) {
       int val = display->getTubeNumber(tubecount);
       val ++;
       //assuming DDMMYY....//FIXME
       if (NUM_TUBES == 6) {
         if (tubecount == 0 && val>3) val = 0;
         else if (tubecount == 1 && val > 1 && display->getTubeNumber(0) ==3) val = 0; //Can't go above 31.
         else if (tubecount == 2 && val>1) val = 0;
         else if (tubecount == 3 && val >2 && display->getTubeNumber(2) == 1) val = 0; //Can't go above 12.
         else if (val>9) val = 0;
       }
       else if (NUM_TUBES == 8) {
         if (tubecount == 0 && val>3) val = 0;
         else if (tubecount==1 && val > 1 && display->getTubeNumber(0) ==3) val = 0; //Can't go above 31.
         else if (tubecount == 3 && val>1) val = 0;
         else if (tubecount == 4 && val >2 && display->getTubeNumber(3) == 1) val = 0; //Can't go above 12.
         else if (val>9) val = 0;
       }
       display->setTubeNumber(tubecount, val);
       display->updateTubes();
    }

    if (buttonA->wasPressed()) {
      tubecount++;
      if (tubecount == NUM_TUBES) tubecount = 0;
      else if (NUM_TUBES == 8) {
        //Skip over the empty tubes
        if (tubecount == 2) tubecount = 3;
        else if (tubecount == 5) tubecount = 6;
      }
    }

    if (buttonA->pressedFor(LONG_BUTTON_PRESS_TIME)) {
      display->blank();
      while (buttonA->read()) ;

      //Build the time we have made and return it
      tmElements_t e;
      if (NUM_TUBES == 6) {
        e.Day = display->getTubeNumber(0)*10  + display->getTubeNumber(1);
        e.Month = display->getTubeNumber(2)*10 + display->getTubeNumber(3);
        e.Year = 30 +  display->getTubeNumber(4)*10 + display->getTubeNumber(5); //30 is because we are starting at year 2000, not 1970
      }
      else if (NUM_TUBES == 8) {
        e.Day = display->getTubeNumber(0)*10  + display->getTubeNumber(1);
        e.Month = display->getTubeNumber(3)*10 + display->getTubeNumber(4);
        e.Year = 30 +  display->getTubeNumber(6)*10 + display->getTubeNumber(7); //30 is because we are starting at year 2000, not 1970
      }
      return e;
    }
    delay(10);
  }
}

tmElements_t ButtonHandler::getTime(DateTime *t) {
  configManager->data.disp_timeformat = TWENTYFOURHR_MODE;
  display->displayTime(*t);
  display->updateTubes();
  //Start with hour.
  int tubecount = 0;
  //Set the seconds to zero.
  if (NUM_TUBES == 6) {
  display->setTubeNumber(4,0);
  display->setTubeNumber(5,0);
  }
  else if(NUM_TUBES == 8) {
    display->setTubeNumber(6,0);
    display->setTubeNumber(7,0);
  }
  int last_blinked = millis();

    //Wait until Button A is released.
    while (buttonA->read()) ;

  while(1) {
    buttonA->read();
    buttonB->read();

    int now = millis();
    if (now > last_blinked+1000) {
      char b = display->getTubeChar(tubecount);
      display->setTubeByte(tubecount, 0x00);
      display->updateTubes();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->updateTubes();
      last_blinked = now;
    }

    //Button A increments the digit
    if (buttonB->wasPressed()) {
      int val = display->getTubeNumber(tubecount);
      val ++;
      //24 hour clock mode.
      if (NUM_TUBES ==6) {
        //Hour can be 0, 1, 2 only.
         if (tubecount == 0 && val == 3) val = 0;
         //If someone chaages the hour 'ten' digit to 2, make sure this wouldn't create a hour > 23
         else if (tubecount == 0 && val == 2 && display->getTubeNumber(1) >3) display->setTubeNumber(1, 0);
         //Don't allow hours > 23
         else if (tubecount == 1 && display->getTubeNumber(0) == 2 && val == 4) val = 0;
         //Don't allow mins/secs 'tens' to be greater than 5.
         else if ((tubecount == 2 ) && val == 6) val = 0;
         //everything can go up to 9.
         else if (val>9) val = 0;
      }
      else if (NUM_TUBES == 8) {
       //Hour can be 0, 1, 2 only.
        if (tubecount == 0 && val == 3) val = 0;
        //If someone chaages the hour 'ten' digit to 2, make sure this wouldn't create a hour > 23
        else if (tubecount == 0 && val == 2 && display->getTubeNumber(1) >3) display->setTubeNumber(1, 0);
        //Don't allow hours > 23
        else if (tubecount == 1 && display->getTubeNumber(0) == 2 && val == 4) val = 0;
        //Don't allow mins/secs 'tens' to be greater than 5.
        else if ((tubecount == 3 ) && val == 6) val = 0;
        //everything can go up to 9.
        else if (val>9) val = 0;
      }
      display->setTubeNumber(tubecount, val);
      display->updateTubes();
    };

    //Button B changes digit
    if (buttonA->wasPressed()) {
      tubecount++;
      //Cant edit seconds.
      if (NUM_TUBES == 6 && tubecount == 4) tubecount = 0;
      else if (NUM_TUBES == 8) {
        //Skip over the empty tubes
        if (tubecount == 2) tubecount = 3;
        //Cant edit seconds.
        else if (tubecount == 5) tubecount = 0;
      }
    }

    if (buttonA->pressedFor(LONG_BUTTON_PRESS_TIME)) {
      display->blank();
      //Wait until the button is released
      while (buttonA->read()) ;

      //Build the time we have made and return it
      tmElements_t e;
      if (NUM_TUBES == 6) {
        e.Hour = display->getTubeNumber(0)*10  + display->getTubeNumber(1);
        e.Minute = display->getTubeNumber(2)*10 + display->getTubeNumber(3);
        e.Second = 0;
      }
      else if (NUM_TUBES == 8) {
        e.Hour = display->getTubeNumber(0)*10  + display->getTubeNumber(1);
        e.Minute = display->getTubeNumber(3)*10 + display->getTubeNumber(4);
        e.Second = 0;
      }
      return e;
    }
    delay(10);
  }
}
