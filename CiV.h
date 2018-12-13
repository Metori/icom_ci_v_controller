#ifndef _CIV_H
#define _CIV_H

#include "Arduino.h"

#define MSG_MAX_SIZE 12 //Including dst and src bytes

typedef enum {
  RECV_STATE_IDLE,
  RECV_STATE_PREAMBLE_HALF,
  RECV_STATE_RECEIVING,
  RECV_STATE_FILTERING,
  RECV_STATE_READY
} ERecvState;

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

  size_t update();
  size_t getMsg(uint8_t* buffer);

private:
  void send(uint8_t* payload, uint8_t size);
  void recv();
  void filt();

  uint8_t mRadioAddr;
  uint8_t mControllerAddr; //this device addr
  uint16_t mBaudRate;

  ERecvState mRecvState = RECV_STATE_IDLE;
  uint8_t mRecvMsg[MSG_MAX_SIZE];
  uint8_t mRecvSize = 0;

};

#endif // _CIV_H
