#ifndef ACTIVITY_WIFI_H
#define ACTIVITY_WIFI_H
#include <SPI.h>
#include <WiFiNINA.h>

namespace activity {
    const int WIFI_LED = 14;

    void initWifi();
    void checkWifi();
}

#endif
