#include "CiV.h"
#include "Controls.h"
#include "Debug.h"

#define DEVICE_NAME "Icom IC-706MkIIG controller"
#define DEVICE_HW_VERSION "1.0"
#define DEVICE_SW_VERSION "0.3"
#define DEVICE_AUTHOR "Artem Pinchuk"

/* HW history
 * 0.1 - Initial HW for testing
 * 1.0 - Complete device
 */

/* SW history
 * 0.1 - Initial code
 * 0.2 - Main part of CI-V protocol implemented
 * 0.3 - Buttons handling added
 */

// ***** CONFIG *****
// Hardware configuration
#define PIN_S_METER NOT_A_PIN

#define PIN_BUTTON_FAGC 4
#define PIN_BUTTON_VFO_A 5
#define PIN_BUTTON_VFO_B 6
#define PIN_BUTTON_MEMO 7

#define PIN_DEBUG_MON_RX 2
#define PIN_DEBUG_MON_TX 3

// Software configuration
#define CI_V_RADIO_ADDR 0x58
#define CI_V_CONTROLLER_ADDR 0xE0 //this device addr
#define CI_V_BAUD_RATE 19200

#define DEBOUNCE_MS 25
// ***** END OF CONFIG *****

#ifdef DEBUG
SoftwareSerial gConsole(PIN_DEBUG_MON_RX, PIN_DEBUG_MON_TX);
#endif

CCiV mCiV(CI_V_RADIO_ADDR, CI_V_CONTROLLER_ADDR, CI_V_BAUD_RATE);

SButton mButtons[] = {{PIN_BUTTON_FAGC, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_A, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_B, DEBOUNCE_MS},
                      {PIN_BUTTON_MEMO, DEBOUNCE_MS}};
CControls mControls(mButtons, sizeof(mButtons) / sizeof(mButtons[0]));

uint8_t mCurAgcState = AGC_FAST;

uint8_t toggleAgc() {
  if (mCurAgcState == AGC_FAST) mCurAgcState = AGC_MID;
  else mCurAgcState = AGC_FAST;

  return mCurAgcState;
}

void setup(void) {
#ifdef DEBUG
  gConsole.begin(9600);
#endif

  DEBUG_PRINTLN(F(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION));
  DEBUG_PRINTLN(F("By " DEVICE_AUTHOR));

  mCiV.init();

  //Blink the built-in LED at start
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);

  //Set initial AGC at start
  mCiV.sendRequest(MSG_SET_AGC, mCurAgcState);
}

void loop(void) {
  switch (mControls.update())
  {
    case PIN_BUTTON_FAGC:
      DEBUG_PRINTLN(F("FAGC clicked"));
      mCiV.sendRequest(MSG_SET_AGC, toggleAgc());
      break;
    case PIN_BUTTON_VFO_A:
      DEBUG_PRINTLN(F("VFO_A clicked"));
      mCiV.sendRequest(MSG_VFO_A);
      break;
    case PIN_BUTTON_VFO_B:
      DEBUG_PRINTLN(F("VFO_B clicked"));
      mCiV.sendRequest(MSG_VFO_B);
      break;
    case PIN_BUTTON_MEMO:
      DEBUG_PRINTLN(F("MEMO clicked"));
      mCiV.sendRequest(MSG_MEMO);
      break;
    default:
      break;
  }

  mCiV.update();
}

