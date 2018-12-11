#ifndef _CIV_H
#define _CIV_H

#include "Arduino.h"

class CCiV {
public:
  CCiV(uint8_t radioAddr, uint8_t controllerAddr, uint16_t baudRate)
    : mRadioAddr(radioAddr),
      mControllerAddr(controllerAddr),
      mBaudRate(baudRate) {
    Serial.begin(baudRate);
  }
  ~CCiV() {

  }

private:
  void send(uint8_t* payload, uint8_t size);

  uint8_t mRadioAddr;
  uint8_t mControllerAddr;
  uint16_t mBaudRate;

};

#endif // _CIV_H
