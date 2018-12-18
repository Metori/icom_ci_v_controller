#include "Controls.h"

CControls::CControls(SButton* buttons, uint8_t count) {
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
CControls::~CControls() {
  delete[] mPins;
  delete[] mDebouncers;
}

int CControls::update() {
  int ret = NOT_A_PIN;

  for(int i = 0; i < mCount; i++) {
    mDebouncers[i].update();
    if (mDebouncers[i].fell()) {
      ret = mPins[i];
    }
  }
  
  return ret;
}
