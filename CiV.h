#ifndef _CIV_H
#define _CIV_H

#include "Arduino.h"

#define MSG_MAX_SIZE 12 //Including dst and src bytes

/* Messages */
/* Select VFO */
#define MSG_VFO 0x07FF
#define MSG_VFO_A 0x0700
#define MSG_VFO_B 0x0701
#define MSG_VFO_EQ 0x07A0
#define MSG_VFO_SWAP 0x07B0

/* Select MEMO */
#define MSG_MEMO 0x08FF

/* Set parameters */
#define MSG_SET_PREAMP 0x1602
#define MSG_SET_AGC 0x1612
#define MSG_SET_NB 0x1622
#define MSG_SET_TONE 0x1642
#define MSG_SET_TSQL 0x1643
#define MSG_SET_COMP 0x1644
#define MSG_SET_VOX 0x1646
#define MSG_SET_BKIN 0x1647

/* Parameters */
#define OFF 0x00
#define ON 0x01

#define PREAMP_OFF 0x00
#define PREAMP_1 0x01
#define PREAMP_2 0x02

#define AGC_FAST 0x01
#define AGC_MID 0x02
#define AGC_SLOW 0x03 //Not supported by IC-706MkIIG

#define BKIN_OFF 0x00
#define BKIN_SEMI 0x01
#define BKIN_FULL 0x02

typedef enum {
  RECV_STATE_IDLE,
  RECV_STATE_PREAMBLE_HALF,
  RECV_STATE_RECEIVING,
  RECV_STATE_FILTERING,
  RECV_STATE_READY
} ERecvState;

class CCiV {
public:
  CCiV(uint8_t radioAddr, uint8_t controllerAddr, uint16_t baudRate);
  ~CCiV();

  void init();

  bool sendRequest(uint16_t cmd);
  bool sendRequest(uint16_t cmd, uint8_t data);
  bool sendRequest(uint16_t cmd, uint8_t data, bool waitResponse);
  bool sendRequest(uint16_t cmd, uint8_t* data, uint8_t size);
  bool sendRequest(uint16_t cmd, uint8_t* data, uint8_t size, bool waitResponse);
  bool isResponseReady();
  uint8_t getResponse();

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

  uint8_t mPendingReq[MSG_MAX_SIZE];
  uint8_t mPendingReqSize = 0;
  unsigned long mPendingReqStartTimeMs = 0;
  uint8_t mTry = 0;
};

#endif // _CIV_H
