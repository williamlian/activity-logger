#include "logger.h"
#include "wifi.h"
#include "web.h"

/*
Signal Pins: 0 ... BUTTON_COUNT-1
Led Pins: LED_BASE_PIN ... LED_BASE_PIN + BUTTON_COUNT - 1
Maximum support 7 LEDs based on the board. Don't change.
Do not use ZERO - it is reserved for "Other"
*/
const int BUTTON_COUNT = 7;
const int LED_BASE_PIN = 7;
// debound ms
const int DEBOUNCE = 50;

// store the ledState for each button
int ledState[BUTTON_COUNT];

// store the last state of the button
bool pressed[BUTTON_COUNT];

activity::WebClient webClient;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(i, INPUT_PULLUP);
    pinMode(LED_BASE_PIN + i, OUTPUT);
    ledState[i] = LOW;
    pressed[i] = false;
  }
  activity::log("Initalized");
  activity::initWifi();
  syncState();
}

void loop() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(i) == LOW) {
      if (!pressed[i]) {
        pressed[i] = true;
      }
    } else if (pressed[i]) {
      pressed[i] = false;
      activity::log("[%d] Pressed", i);
      onclick(i);
    }
  }
  activity::checkWifi();
  delay(DEBOUNCE);
}

// fetch last active type from server and sync the light state
void syncState() {
  int activeType = webClient.getLastActivityType();
  for(int i = 0; i < BUTTON_COUNT; i++) {
    ledState[i] = (i == activeType) ? HIGH : LOW;
    updateLed(i);
  }
  activity::log("state sync'ed. active type = %d", activeType);
}

void onclick(int button) {
  int currentState = ledState[button];
  // Immediate button feedback. In case of web call fail, 
  // the getLastActivity will revert the button state
  ledState[button] = ledState[button] == HIGH ? LOW : HIGH;
  updateLed(button);

  if(currentState == HIGH) {
    webClient.endActivity(button);
  } else {
    webClient.startActivity(button);
  }
  

  syncState();
}

void updateLed(int button) {
  digitalWrite(LED_BASE_PIN + button, ledState[button]);
}
