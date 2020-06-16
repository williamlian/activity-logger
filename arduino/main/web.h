#ifndef ACTIVITY_WEB_H
#define ACTIVITY_WEB_H
#include <WiFiSSLClient.h>
#include <ArduinoHttpClient.h>
#include <stdio.h>

#include "secrets.h"

namespace activity {
  // https server port
  const int SERVER_PORT = 443;
  // buffer size for request path (e.g "/activity/get")
  const int PATH_BUF_SIZE = 256;
  // buffer size for request body (e.g. JSON payload)
  const int DATA_BUF_SIZE = 1024;

  class WebClient {
  public:
    WebClient() {
      http.connectionKeepAlive();
    }
    int getLastActivityType(int userID);
    bool startActivity(int userID, int type);
    bool endActivity(int userID, int type);

  protected:
    WiFiSSLClient client;
    HttpClient http = HttpClient(client, ACTIVITY_SECRET_SERVER, SERVER_PORT);
    char path[PATH_BUF_SIZE];
    char data[DATA_BUF_SIZE];

    void waitForResponse();
  };
}  // namespace activity

#endif
