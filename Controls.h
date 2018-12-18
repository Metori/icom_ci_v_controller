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
  CControls(SButton* buttons, uint8_t count);
  ~CControls();

  int update();

private:
  int* mPins;
  Bounce* mDebouncers;
  uint8_t mCount;
};

#endif // _CONTROLS_H
