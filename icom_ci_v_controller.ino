#include "CiV.h"
#include "Controls.h"
#include <SoftwareSerial.h>

#define DEVICE_NAME "Icom IC-706MkIIG controller"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.2"
#define DEVICE_AUTHOR "Artem Pinchuk"

/* HW history
 * 0.1 - Initial HW for testing
 */

/* SW history
 * 0.1 - Initial code
 * 0.2 - Main part of CI-V protocol implemented
 */

// ***** CONFIG *****
// Hardware configuration
#define PIN_S_METER NOT_A_PIN

#define PIN_BUTTON_FAGC NOT_A_PIN
#define PIN_BUTTON_VFO_A NOT_A_PIN
#define PIN_BUTTON_VFO_B NOT_A_PIN
#define PIN_BUTTON_MEMO NOT_A_PIN

#define PIN_DEBUG_MON_RX 2
#define PIN_DEBUG_MON_TX 3

// Software configuration
#define CI_V_RADIO_ADDR 0x58
#define CI_V_CONTROLLER_ADDR 0xE0 //this device addr
#define CI_V_BAUD_RATE 19200

#define DEBOUNCE_MS 25
// ***** END OF CONFIG *****

SoftwareSerial gConsole(PIN_DEBUG_MON_RX, PIN_DEBUG_MON_TX);

CCiV mCiV(CI_V_RADIO_ADDR, CI_V_CONTROLLER_ADDR, CI_V_BAUD_RATE);

SButton mButtons[] = {{PIN_BUTTON_FAGC, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_A, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_B, DEBOUNCE_MS},
                      {PIN_BUTTON_MEMO, DEBOUNCE_MS}};
CControls mControls(mButtons, sizeof(mButtons) / sizeof(mButtons[0]));

unsigned long mLastTry = 0;
bool mTried = false;

void setup(void) {
  gConsole.begin(9600);
  gConsole.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);
  gConsole.println("By " DEVICE_AUTHOR);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
}

void loop(void) {
  mControls.update();

  unsigned long m = millis();
  if (!mTried && ((m - mLastTry) > 10000)) {
    digitalWrite(13, HIGH);
    gConsole.println("Sending...");
    uint8_t d[] = {0x02};
    mCiV.sendRequest(0x1612, d, 1);
    mTried = true;
    gConsole.println("Request sent");
  }

  mCiV.update();
}

