#include "CiV.h"

#define MSG_PREAMBLE 0xFE //two preambles start a CI-V message
#define MSG_END_CODE 0xFD

#define BROADCAST_ADDR 0x00

#define NO_SC 0xFF

#define RESPONSE_FB 0xFB
#define RESPONSE_FA 0xFA

CCiV::CCiV(uint8_t radioAddr, uint8_t controllerAddr, uint16_t baudRate)
  : mRadioAddr(radioAddr),
    mControllerAddr(controllerAddr),
    mBaudRate(baudRate) {

}

CCiV::~CCiV() {

}

void CCiV::sendRequest(uint16_t cmd, uint8_t* data, uint8_t size) {
  gConsole.println("[CiV] Sending request");

  uint8_t cn = cmd >> 8;
  uint8_t sc = cmd & 0xFF;

  gConsole.print("Cn: ");
  gConsole.println(cn);
  gConsole.print("Sc: ");
  gConsole.println(sc);

  uint8_t reqSize = size + (sc == NO_SC ? 1 : 2);
  uint8_t req[reqSize];

  uint8_t i = 0;
  req[i++] = cn;
  if (sc != NO_SC) req[i++] = sc;
  memcpy(req + i, data, size);

  send(req, reqSize);
}

bool CCiV::isResponseReady() {
  return mRecvState == RECV_STATE_READY;
}

uint8_t CCiV::getResponse() {
  mRecvState = RECV_STATE_IDLE;
  return mRecvMsg[0];
}

void CCiV::send(uint8_t* payload, uint8_t size) {
  uint8_t dataSize = size + 5;
  uint8_t data[dataSize];
  data[0] = MSG_PREAMBLE;
  data[1] = MSG_PREAMBLE;
  data[2] = mRadioAddr;
  data[3] = mControllerAddr;

  memcpy(data + 4, payload, size);

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
        gConsole.println("recv IDLE");
        if (rb == MSG_PREAMBLE) mRecvState = RECV_STATE_PREAMBLE_HALF;
        break;
      case RECV_STATE_PREAMBLE_HALF:
        gConsole.println("recv PREAMBLE_HALF");
        if (rb == MSG_PREAMBLE) {
          mRecvState = RECV_STATE_RECEIVING;
          mRecvSize = 0;
        }
        break;
      case RECV_STATE_RECEIVING:
        gConsole.println("recv RECEIVING");
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
  if (mRecvState != RECV_STATE_FILTERING) return;
  gConsole.println("[CiV] filt");

  uint8_t dst = mRecvMsg[0];
  uint8_t src = mRecvMsg[1];

  gConsole.print("DST: ");
  gConsole.println(dst);
  gConsole.print("SRC: ");
  gConsole.println(src);

  if (dst == mRadioAddr && src == mControllerAddr) { //echo of sended message
    gConsole.println("Echo message discarded");
    //TODO: implement handling of echo check, send jammer msg in case of jam
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  } else if (dst == mControllerAddr && src == mRadioAddr) {
    gConsole.println("Filtering...");
    for (int i = 0; i < (mRecvSize - 2); i++) {
      mRecvMsg[i] = mRecvMsg[i + 2];
    }
    mRecvSize -= 2;
    mRecvState = RECV_STATE_READY;
  } else if (dst == BROADCAST_ADDR) {
    gConsole.println("Broadcast message discarded");
    //TODO: implement handling of broadcast msgs
    //Discard msg `
    mRecvState = RECV_STATE_IDLE;
  } else {
    gConsole.println("Unknown message discarded");
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  }
}
