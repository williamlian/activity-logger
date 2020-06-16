#include <Scheduler.h>
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

// this use to store HIGH/LOW if ledState == BLINK
const int BLINK = -1;
int blinkState[BUTTON_COUNT];

// store the last state of the button, purely internal for button up detection
bool pressed[BUTTON_COUNT];

// hard coded user ID
const int USER_ID = 0;

// update to adjust blink speed
const int BLINK_SPEED = 100;

activity::WebClient webClient;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(i, INPUT_PULLUP);
    pinMode(LED_BASE_PIN + i, OUTPUT);

    ledState[i] = BLINK;
    blinkState[i] = LOW;
    pressed[i] = false;
  }
  activity::log("[main] Initalized");
  Scheduler.startLoop(blinkLoop);
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
      activity::log("[main] [%d] Pressed", i);
      onclick(i);
    }
  }
  if(!activity::isConnected()) {
    blinkAll();
    activity::connectWifi();
  }
  delay(DEBOUNCE);
}

// fetch last active type from server and sync the light state
void syncState() {
  int activeType = webClient.getLastActivityType(USER_ID);
  for(int i = 0; i < BUTTON_COUNT; i++) {
    ledState[i] = (i == activeType) ? HIGH : LOW;
    updateLed(i);
  }
  activity::log("[main] state sync'ed. active type = %d", activeType);
}

void onclick(int button) {
  int currentState = ledState[button];
  blink(button, HIGH);
  if(currentState == HIGH) {
    webClient.endActivity(USER_ID, button);
  } else {
    webClient.startActivity(USER_ID, button);
  }
  syncState();
}

void updateLed(int button) {
  digitalWrite(LED_BASE_PIN + button, ledState[button]);
}

void blinkAll() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    blinkState[i] = LOW;
    ledState[i] = BLINK;
  }
}

void blink(int button, int initalState) {
  blinkState[button] = initalState;
  ledState[button] = BLINK;
}

void blinkLoop() {
  for(int i = 0; i < BUTTON_COUNT; i++) {
    if(ledState[i] == BLINK) {
      blinkState[i] = blinkState[i] == HIGH ? LOW : HIGH;
      digitalWrite(LED_BASE_PIN + i, blinkState[i]);
    }
  }
  delay(BLINK_SPEED);
}
