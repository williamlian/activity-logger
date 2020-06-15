#include "wifi.h"
#include "logger.h"
#include "secrets.h"

char ssid[] = ACTIVITY_SECRET_SSID;
char pass[] = ACTIVITY_SECRET_PASS;

int status = WL_IDLE_STATUS;

void activity::initWifi() {
  pinMode(activity::WIFI_LED, OUTPUT);
  digitalWrite(activity::WIFI_LED, LOW);

  if (WiFi.status() == WL_NO_SHIELD) {
    activity::log("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    activity::log("Please upgrade the firmware");
  }
  checkWifi();
  // you're connected now, so print out the data:
  activity::log("You're connected to the network");
}

void activity::checkWifi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    digitalWrite(activity::WIFI_LED, LOW);
    activity::log("Attempting to connect to WPA SSID: %s", ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  digitalWrite(activity::WIFI_LED, HIGH);
}
