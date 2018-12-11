#include "Controls.h"

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
