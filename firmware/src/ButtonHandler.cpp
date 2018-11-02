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
  buttonA->read();
  buttonB->read();
  buttonC->read();
  buttonD->read();

  if (buttonA->pressedFor(1000)) {
    return  BUTTON_A_LONGPRESS;
  }
  if (buttonA->wasPressed()) {
    return BUTTON_A_SHORTPRESS;
  }
  if (buttonB->pressedFor(1000)) {
    return  BUTTON_B_LONGPRESS;
  }
  if (buttonB->wasPressed()) {
    return BUTTON_B_SHORTPRESS;
  }
  if (buttonC->pressedFor(1000)) {
    return  BUTTON_C_LONGPRESS;
  }
  if (buttonC->wasPressed()) {
    return BUTTON_C_SHORTPRESS;
  }
  if (buttonD->pressedFor(1000)) {
    return  BUTTON_D_LONGPRESS;
  }
  if (buttonD->wasPressed()) {
    return BUTTON_D_SHORTPRESS;
  }

  return NO_PRESS;
}

tmElements_t ButtonHandler::getDate(DateTime *t ) {
  display->displayDate(*t, DDMMYY_MODE); //FIXME
  int tubecount = 7;

  int last_blinked = millis();

  while(1) {
    buttonA->read();
    buttonB->read();
    buttonC->read();

    int now = millis();

    if (now > last_blinked+1000) {
      char b = display->getTubeChar(tubecount);
      display->setTubeByte(tubecount, 0x00);
      display->refreshDisplay();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->refreshDisplay();
      last_blinked = now;
    }

    if (buttonA->wasPressed()) {
       int val = display->getTubeChar(tubecount);
       val ++;
       //assuming DDMMYY....//FIXME

       if (tubecount == 7 && val>3) val = 0;
       else if (tubecount==6 && val > 1 && display->getTubeChar(7) ==3) val = 0; //Can't go above 31.
       else if (tubecount == 4 && val>1) val = 1;
       else if (tubecount == 3 && val >2 && display->getTubeChar(4) == 1) val = 0; //Can't go above 12.
       else if (val>9) val = 0;

       display->setTubeChar(tubecount, val);
       display->refreshDisplay();
    }

    if (buttonB->wasPressed()) {
      //skip over the 'empty' tubes here.
      tubecount--;
      switch (tubecount) {
        case 5:
          tubecount = 4;
          break;
        case 2:
          tubecount = 1;
          break;
        case -1:
          tubecount = 7;
          break;
      }
    }

    if (buttonC->wasPressed()) {
      //Build the time we have made and return it
      tmElements_t e;
      e.Day = display->getTubeChar(7)*10  + display->getTubeChar(6);
      e.Month = display->getTubeChar(4)*10 + display->getTubeChar(3);
      e.Year = 30 +  display->getTubeChar(1)*10 + display->getTubeChar(0); //30 is because we are starting at year 2000, not 1970
      return e;
    }
    delay(10);
  }
}

tmElements_t ButtonHandler::getTime(DateTime *t) {
  display->displayTime(*t, TWENTYFOURHR_MODE); //FIXME
  //Start with hour.
  int tubecount = 7;
  //Set the seconds to zero.
  display->setTubeChar(0,0);
  display->setTubeChar(1,0);

  int last_blinked = millis();
  while(1) {
    buttonA->read();
    buttonB->read();
    buttonC->read();

    int now = millis();

    if (now > last_blinked+1000) {
      char b = display->getTubeChar(tubecount);
      display->setTubeByte(tubecount, 0x00);
      display->refreshDisplay();
      delay(200);
      display->setTubeChar(tubecount, b);
      display->refreshDisplay();
      last_blinked = now;
    }

    //Button A increments the digit
    if (buttonA->wasPressed()) {
      int val = display->getTubeChar(tubecount);
      val ++;
      //24 hour clock mode.
      //Hour can be 0, 1, 2 only.
      if (tubecount == 7 && val == 3) val = 0;
      //If someone chaages the hour 'ten' digit to 2, make sure this wouldn't create a hour > 23
      else if (tubecount == 7 && val == 2 && display->getTubeChar(6) >3) display->setTubeChar(6, 0);
      //Don't allow hours > 23
      else if (tubecount == 6 && display->getTubeChar(7) == 2 && val == 4) val = 0;
      //Don't allow mins/secs 'tens' to be greater than 5.
      else if ((tubecount == 4 ) && val == 6) val = 0;
      //everything can go up to 9.
      else if (val>9) val = 0;
      display->setTubeChar(tubecount, val);
      display->refreshDisplay();
    };

    //Button B changes digit
    if (buttonB->wasPressed()) {
      //skip over the 'empty' tubes here.
      tubecount--;
      switch (tubecount) {
        case 5:
          tubecount = 4;
          break;
        case 2:
          tubecount = 7;
          break;
      }
    }

    if (buttonC->wasPressed()) {
      //Build the time we have made and return it
      tmElements_t e;
      e.Hour = display->getTubeChar(7)*10  + display->getTubeChar(6);
      e.Minute = display->getTubeChar(4)*10 + display->getTubeChar(3);
      e.Second = 0;
      return e;
    }
    delay(10);
  }
}
