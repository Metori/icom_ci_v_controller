#include "CiV.h"

#define MSG_PREAMBLE 0xFE //two preambles start a CI-V message
#define MSG_END_CODE 0xFD

#define BROADCAST_ADDR 0x00

void CCiV::send(uint8_t* payload, uint8_t size) {
  uint8_t dataSize = size + 5;
  uint8_t data[dataSize];
  data[0] = MSG_PREAMBLE;
  data[1] = MSG_PREAMBLE;
  data[2] = mRadioAddr;
  data[3] = mControllerAddr;

  for (int i = 0; i < size; i++) {
    data[i + size] = payload[i];
  }

  data[dataSize - 1] = MSG_END_CODE;

  Serial.write(data, dataSize);
}

size_t CCiV::update() {
  size_t ret = 0;

  recv();
  filt();

  if (mRecvState == RECV_STATE_READY) ret = mRecvSize;

  return ret;
}

size_t CCiV::getMsg(uint8_t* buffer) {
  memcpy(buffer, mRecvMsg, mRecvSize);
  return mRecvSize;
}

void CCiV::recv() {
  uint8_t rb;

  while (Serial.available() && mRecvState <= RECV_STATE_RECEIVING) {
    rb = Serial.read();
    switch (mRecvState) {
      case RECV_STATE_IDLE:
        if (rb == MSG_PREAMBLE) mRecvState = RECV_STATE_PREAMBLE_HALF;
        break;
      case RECV_STATE_PREAMBLE_HALF:
        if (rb == MSG_PREAMBLE) {
          mRecvState = RECV_STATE_RECEIVING;
          mRecvSize = 0;
        }
        break;
      case RECV_STATE_RECEIVING:
        if (rb == MSG_END_CODE) mRecvState = RECV_STATE_FILTERING;
        else {
          //Overflow condition: discard current msg
          if (mRecvSize >= MSG_MAX_SIZE) mRecvState = RECV_STATE_IDLE;
          else mRecvMsg[mRecvSize++] = rb;
        }
        break;
    }
  }
}

void CCiV::filt() {
  if (mRecvState < RECV_STATE_FILTERING) return;

  uint8_t dst = mRecvMsg[0];
  uint8_t src = mRecvMsg[1];

  if (dst == mRadioAddr && src == mControllerAddr) { //echo of sended message
    //TODO: implement handling of echo check, send jammer msg in case of jam
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  } else if (dst == mControllerAddr && src == mRadioAddr) {
    for (int i = 0; i < (mRecvSize - 2); i++) {
      mRecvMsg[i] = mRecvMsg[i + 2];
    }
    mRecvSize -= 2;
    mRecvState = RECV_STATE_READY;
  } else if (dst == BROADCAST_ADDR) {
    //TODO: implement handling of broadcast msgs
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  } else {
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  }
}
