#ifndef _CONTROLS_H
#define _CONTROLS_H

#include "Arduino.h"
#include <Bounce2.h>

typedef struct {
  int pin;
  uint16_t debounceMs;
} SButton;

class CControls {
public:
  CControls(SButton* buttons, uint8_t count) {
    mPins = new int[count];
    mDebouncers = new Bounce[count];
    mCount = count;
    
    for(int i=0; i<count; i++) {
      SButton btn = buttons[i];
      
      pinMode(btn.pin, INPUT_PULLUP);
      mDebouncers[i].attach(btn.pin);
      mDebouncers[i].interval(btn.debounceMs);
    }
  }
  ~CControls() {
    delete[] mPins;
    delete[] mDebouncers;
  }

  void init();
  int update();

private:
  int* mPins;
  Bounce* mDebouncers;
  uint8_t mCount;
};

#endif // _CONTROLS_H
