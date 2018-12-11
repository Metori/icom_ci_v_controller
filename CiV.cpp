#include "CiV.h"

#define MSG_PREAMBLE 0xFF //two preambles start a CI-V message
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

size_t CCiV::recv(uint8_t* buffer, uint8_t size) {
  size_t ret = 0;

  if (Serial.available()) {
    while (Serial.available()) {

    }
  }

  return ret;
}

