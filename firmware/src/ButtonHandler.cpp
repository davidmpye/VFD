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
  Button *button[]={buttonA, buttonB, buttonC, buttonD};

  BUTTON_EVENT be=NO_PRESS;
  BUTTON_EVENT beArray[]={BUTTON_A_SHORTPRESS, BUTTON_A_LONGPRESS,
      BUTTON_B_SHORTPRESS, BUTTON_B_LONGPRESS,
      BUTTON_C_SHORTPRESS, BUTTON_C_LONGPRESS,
      BUTTON_D_SHORTPRESS, BUTTON_D_LONGPRESS};

  for(int i=0; i<4; i++){ // Number of buttons
    button[i]->read();
    if(button[i]->wasPressed())
      msButtonLastChange=button[i]->lastChange(); // Store timestamp of last button change
    else if(button[i]->wasReleased()
        && (button[i]->lastChange()-msButtonLastChange)<LONG_BUTTON_PRESS_TIME)
      be=beArray[i*2]; // Short press
    else if(button[i]->pressedFor(LONG_BUTTON_PRESS_TIME))
      be=beArray[i*2+1]; // Long press
  }

  return be;
}

tmElements_t ButtonHandler::getDate(DateTime *t ) {
  display->setDateMode(DDMMYY_MODE);
  display->displayDate(*t);
  display->update();

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
      display->update();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->update();
      last_blinked = now;
    }

    if (buttonB->wasPressed()) {
       int val = display->getTubeChar(tubecount);
       val ++;
       //assuming DDMMYY....//FIXME
       if (NUM_TUBES == 6) {
         if (tubecount == 0 && val>3) val = 0;
         else if (tubecount == 1 && val > 1 && display->getTubeChar(0) ==3) val = 0; //Can't go above 31.
         else if (tubecount == 2 && val>1) val = 0;
         else if (tubecount == 3 && val >2 && display->getTubeChar(2) == 1) val = 0; //Can't go above 12.
         else if (val>9) val = 0;
       }
       else if (NUM_TUBES == 8) {
         if (tubecount == 0 && val>3) val = 0;
         else if (tubecount==1 && val > 1 && display->getTubeChar(0) ==3) val = 0; //Can't go above 31.
         else if (tubecount == 3 && val>1) val = 0;
         else if (tubecount == 4 && val >2 && display->getTubeChar(3) == 1) val = 0; //Can't go above 12.
         else if (val>9) val = 0;
       }
       display->setTubeChar(tubecount, val);
       display->update();
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
        e.Day = display->getTubeChar(0)*10  + display->getTubeChar(1);
        e.Month = display->getTubeChar(2)*10 + display->getTubeChar(3);
        e.Year = 30 +  display->getTubeChar(4)*10 + display->getTubeChar(5); //30 is because we are starting at year 2000, not 1970
      }
      else if (NUM_TUBES == 8) {
        e.Day = display->getTubeChar(0)*10  + display->getTubeChar(1);
        e.Month = display->getTubeChar(3)*10 + display->getTubeChar(4);
        e.Year = 30 +  display->getTubeChar(6)*10 + display->getTubeChar(7); //30 is because we are starting at year 2000, not 1970
      }
      return e;
    }
    delay(10);
  }
}

tmElements_t ButtonHandler::getTime(DateTime *t) {
  display->setTimeMode(TWENTYFOURHR_MODE);
  display->displayTime(*t);
  display->update();
  //Start with hour.
  int tubecount = 0;
  //Set the seconds to zero.
  if (NUM_TUBES == 6) {
  display->setTubeChar(4,0);
  display->setTubeChar(5,0);
  }
  else if(NUM_TUBES == 8) {
    display->setTubeChar(6,0);
    display->setTubeChar(7,0);
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
      display->update();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->update();
      last_blinked = now;
    }

    //Button A increments the digit
    if (buttonB->wasPressed()) {
      int val = display->getTubeChar(tubecount);
      val ++;
      //24 hour clock mode.
      if (NUM_TUBES ==6) {
        //Hour can be 0, 1, 2 only.
         if (tubecount == 0 && val == 3) val = 0;
         //If someone chaages the hour 'ten' digit to 2, make sure this wouldn't create a hour > 23
         else if (tubecount == 0 && val == 2 && display->getTubeChar(1) >3) display->setTubeChar(1, 0);
         //Don't allow hours > 23
         else if (tubecount == 1 && display->getTubeChar(0) == 2 && val == 4) val = 0;
         //Don't allow mins/secs 'tens' to be greater than 5.
         else if ((tubecount == 2 ) && val == 6) val = 0;
         //everything can go up to 9.
         else if (val>9) val = 0;
      }
      else if (NUM_TUBES == 8) {
       //Hour can be 0, 1, 2 only.
        if (tubecount == 0 && val == 3) val = 0;
        //If someone chaages the hour 'ten' digit to 2, make sure this wouldn't create a hour > 23
        else if (tubecount == 0 && val == 2 && display->getTubeChar(1) >3) display->setTubeChar(1, 0);
        //Don't allow hours > 23
        else if (tubecount == 1 && display->getTubeChar(0) == 2 && val == 4) val = 0;
        //Don't allow mins/secs 'tens' to be greater than 5.
        else if ((tubecount == 3 ) && val == 6) val = 0;
        //everything can go up to 9.
        else if (val>9) val = 0;
      }
      display->setTubeChar(tubecount, val);
      display->update();
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
        e.Hour = display->getTubeChar(0)*10  + display->getTubeChar(1);
        e.Minute = display->getTubeChar(2)*10 + display->getTubeChar(3);
        e.Second = 0;
      }
      else if (NUM_TUBES == 8) {
        e.Hour = display->getTubeChar(0)*10  + display->getTubeChar(1);
        e.Minute = display->getTubeChar(3)*10 + display->getTubeChar(4);
        e.Second = 0;
      }
      return e;
    }
    delay(10);
  }
}
