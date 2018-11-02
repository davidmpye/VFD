#ifndef __BUTTONHANDLER__
#define __BUTTONHANDLER__
#include <JC_Button.h>
#include "Display.h"
#include "Config.h"
#include <Time.h>
#include <Timezone.h>
#include <RTClib.h>

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

    void begin(int a, int b, int c, int d, Display *); //four pin numbers.

    BUTTON_EVENT poll();

    tmElements_t getTime(DateTime*);
    tmElements_t getDate(DateTime*);
  private:
    Button *buttonA, *buttonB, *buttonC, *buttonD;
    Display *display;

}
;

#endif
