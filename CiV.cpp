#include "CiV.h"
#include "Debug.h"

#define MSG_PREAMBLE 0xFE //two preambles start a CI-V message
#define MSG_END_CODE 0xFD

#define BROADCAST_ADDR 0x00

#define NO_SC 0xFF

#define RESP_ACK 0xFB
#define RESP_NACK 0xFA

#define RESP_WAIT_TIMEOUT_MS 1000
#define REQ_TRIES 3

CCiV::CCiV(uint8_t radioAddr, uint8_t controllerAddr, uint16_t baudRate)
  : mRadioAddr(radioAddr),
    mControllerAddr(controllerAddr),
    mBaudRate(baudRate) {

}

CCiV::~CCiV() {

}

void CCiV::init() {
  Serial.begin(mBaudRate);
}

bool CCiV::sendRequest(uint16_t cmd) {
  return sendRequest(cmd, nullptr, 0);
}

bool CCiV::sendRequest(uint16_t cmd, uint8_t data) {
  return sendRequest(cmd, &data, 1);
}

bool CCiV::sendRequest(uint16_t cmd, uint8_t* data, uint8_t size) {
  return sendRequest(cmd, data, size, true);
}

bool CCiV::sendRequest(uint16_t cmd, uint8_t* data, uint8_t size, bool waitResponse) {
  if (mPendingReqSize > 0) return false;
  
  DEBUG_PRINTLN(F("[CiV] Sending request"));

  uint8_t cn = cmd >> 8;
  uint8_t sc = cmd & 0xFF;

  DEBUG_PRINT(F("Cn: "));
  DEBUG_PRINTLN(cn);
  DEBUG_PRINT(F("Sc: "));
  DEBUG_PRINTLN(sc);

  uint8_t reqSize = size + (sc == NO_SC ? 1 : 2);
  uint8_t req[reqSize];

  uint8_t i = 0;
  req[i++] = cn;
  if (sc != NO_SC) req[i++] = sc;
  if (data != nullptr && size > 0) memcpy(req + i, data, size);

  send(req, reqSize);

  if (waitResponse) {
    mPendingReqStartTimeMs = millis();
    memcpy(mPendingReq, req, reqSize);
    mPendingReqSize = reqSize;
    mTry = 0;
  }

  return true;
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

  if (mRecvState == RECV_STATE_READY) {
    if (mRecvSize == 1) {
      if (mRecvMsg[0] == RESP_ACK) {
        DEBUG_PRINTLN(F("[CiV] ACK received"));
        //TODO: handle ACK
        mPendingReqSize = 0;
      } else if (mRecvMsg[0] == RESP_NACK) {
        DEBUG_PRINTLN(F("[CiV] NACK received"));
        //TODO: handle NACK
        mPendingReqSize = 0;
      } else {
        DEBUG_PRINTLN(F("[CiV] Unknown response, discard"));
      }
    } else if (mRecvSize > 1) {
      DEBUG_PRINTLN(F("[CiV] Response with data received"));
      //TODO: handle response with data
      mPendingReqSize = 0;
    } else {
      DEBUG_PRINTLN(F("[CiV] Empty msg, discard"));
    }

    mRecvState = RECV_STATE_IDLE;
    ret = mRecvSize;
  } else {
    if ((mPendingReqSize != 0) && (millis() - mPendingReqStartTimeMs > RESP_WAIT_TIMEOUT_MS)) {
      if (++mTry < REQ_TRIES) {
        DEBUG_PRINTLN(F("[CiV] Req wait timeout, retrying..."));
        send(mPendingReq, mPendingReqSize);
        mPendingReqStartTimeMs = millis();
      } else {
        DEBUG_PRINTLN(F("[CiV] Request retries left"));
        //TODO: handle req timeout err
        mPendingReqSize = 0;

        
      }
    }
  }

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
        DEBUG_PRINTLN(F("recv IDLE"));
        if (rb == MSG_PREAMBLE) mRecvState = RECV_STATE_PREAMBLE_HALF;
        break;
      case RECV_STATE_PREAMBLE_HALF:
        DEBUG_PRINTLN(F("recv PREAMBLE_HALF"));
        if (rb == MSG_PREAMBLE) {
          mRecvState = RECV_STATE_RECEIVING;
          mRecvSize = 0;
        }
        break;
      case RECV_STATE_RECEIVING:
        DEBUG_PRINTLN(F("recv RECEIVING"));
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
  DEBUG_PRINTLN(F("[CiV] filt"));

  uint8_t dst = mRecvMsg[0];
  uint8_t src = mRecvMsg[1];

  DEBUG_PRINT(F("DST: "));
  DEBUG_PRINTLN(dst);
  DEBUG_PRINT(F("SRC: "));
  DEBUG_PRINTLN(src);

  if (dst == mRadioAddr && src == mControllerAddr) { //echo of sended message
    DEBUG_PRINTLN(F("Echo msg discarded"));
    //TODO: implement handling of echo check, send jammer msg in case of jam
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  } else if (dst == mControllerAddr && src == mRadioAddr) {
    DEBUG_PRINTLN(F("Filtering..."));
    for (int i = 0; i < (mRecvSize - 2); i++) {
      mRecvMsg[i] = mRecvMsg[i + 2];
    }
    mRecvSize -= 2;
    mRecvState = RECV_STATE_READY;
  } else if (dst == BROADCAST_ADDR) {
    DEBUG_PRINTLN(F("Broadcast msg discarded"));
    //TODO: implement handling of broadcast msgs
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  } else {
    DEBUG_PRINTLN(F("Unknown msg discarded"));
    //Discard msg
    mRecvState = RECV_STATE_IDLE;
  }
}
