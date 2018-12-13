#include "CiV.h"
#include "Controls.h"
#include <SoftwareSerial.h>

#define DEVICE_NAME "Icom IC-706MkIIG controller"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.1"
#define DEVICE_AUTHOR "Artem Pinchuk"

/* HW history
 * 0.1 - Initial HW for testing
 */

/* SW history
 * 0.1 - Initial code
 */

// ***** CONFIG *****
// Hardware configuration
#define PIN_S_METER NOT_A_PIN

#define PIN_BUTTON_FAGC NOT_A_PIN
#define PIN_BUTTON_VFO_A NOT_A_PIN
#define PIN_BUTTON_VFO_B NOT_A_PIN
#define PIN_BUTTON_MEMO NOT_A_PIN

#define PIN_DEBUG_MON_RX NOT_A_PIN
#define PIN_DEBUG_MON_TX NOT_A_PIN

// Software configuration
#define CI_V_RADIO_ADDR 0x58
#define CI_V_CONTROLLER_ADDR 0xE0 //this device addr
#define CI_V_BAUD_RATE 19200

#define DEBOUNCE_MS 25
// ***** END OF CONFIG *****

SoftwareSerial console(PIN_DEBUG_MON_RX, PIN_DEBUG_MON_TX);

CCiV mCiV(CI_V_RADIO_ADDR, CI_V_CONTROLLER_ADDR, CI_V_BAUD_RATE);

SButton mButtons[] = {{PIN_BUTTON_FAGC, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_A, DEBOUNCE_MS},
                      {PIN_BUTTON_VFO_B, DEBOUNCE_MS},
                      {PIN_BUTTON_MEMO, DEBOUNCE_MS}};
CControls mControls(mButtons, sizeof(mButtons));

void setup(void) {
  console.begin(9600);
  console.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);
  console.println("By " DEVICE_AUTHOR);

}

void loop(void) {
  mControls.update();
}

