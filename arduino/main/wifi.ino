#include "wifi.h"
#include "logger.h"
#include "secrets.h"

const int CHECK_PERIOD = 5000;

char ssid[] = ACTIVITY_SECRET_SSID;
char pass[] = ACTIVITY_SECRET_PASS;

int status = WL_IDLE_STATUS;

void activity::initWifi() {
  pinMode(activity::WIFI_LED, OUTPUT);
  digitalWrite(activity::WIFI_LED, LOW);

  if (WiFi.status() == WL_NO_SHIELD) {
    activity::log("[wifi] Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    activity::log("[wifi] Please upgrade the firmware");
  }
  connectWifi();
  activity::log("[wifi] You're connected to the network");
}

bool activity::isConnected() {
  return status == WL_CONNECTED;
}

void activity::connectWifi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    digitalWrite(activity::WIFI_LED, LOW);
    activity::log("[wifi] Attempting to connect to WPA SSID: %s", ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait x seconds for connection:
    delay(CHECK_PERIOD);
  }
  digitalWrite(activity::WIFI_LED, HIGH);
}
