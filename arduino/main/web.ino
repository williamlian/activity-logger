#include <stdio.h>
#include <stdlib.h>
#include "web.h"
#include "logger.h"

using namespace activity;

const char *startPath = "POST /activity/start HTTP/1.1";
const char *endPath = "POST /activity/end HTTP/1.1";
const char *lastActivityPath = "GET /activity/last?userID=%d HTTP/1.1";
const char *requestBody = "{\"userID\":%d,\"typeID\":%d}";

int WebClient::getLastActivityType() {
  sprintf(path, lastActivityPath, USER_ID);
  if(!request()) {
    log("failed to request last activity");
    return -1;
  }
  if(!readResponse()) {
    log("failed to read response from last activity");
    return -1;
  }
  return atoi(bodyBuf);
}

bool WebClient::startActivity(int type) {
  sprintf(path, startPath);
  if(!request()) {
    log("failed to start activity");
    return false;
  }
  
  if(!readResponse()) {
    log("failed to get response from start activity");
    return false;
  }
  return statusCode == 200;
}

bool WebClient::endActivity(int type) {
  sprintf(path, endPath);
  if(!request()) {
    log("failed to end activity");
    return false;
  }
  if(!readResponse()) {
    log("failed to get response from end activity");
    return false;
  }
  return statusCode == 200;
}

bool WebClient::request() {
  bool connected = client.connect(ACTIVITY_SECRET_SERVER, 443);
  if(!connected) {
    log("failed to connect server");
    return false;
  }
  client.println(path);
  client.println(host);
  client.println("Content-Type: text/json");
  client.println("Connection: close");
  client.println();
  return true;
}

bool WebClient::readResponse() {
  int p = 0;
  if(!client.connected() || !client.available()) {
    return false;
  }
  // read status code
  while(client.connected()) {
    if(client.available()) {
      char c = client.read();
      p++;
      if(p < 9) {
        continue;
      } else if(p <= 11) {
        statusCodeBuf[p-9] = c;
      } else {
        break;
      }
    }
  }
  statusCodeBuf[p] = 0;
  statusCode = atoi(statusCodeBuf);

  // skip headers
  int newLineCount = 0;
  while(client.connected()) {
    if(client.available()) {
      char c = client.read();
      if(c == '\n') {
        newLineCount++;
      } else {
        newLineCount = 0;
      }
      if(newLineCount == 2) {
        break;
      }
    }
  }

  // read body
  p = 0;
  while(client.connected()) {
    if(client.available()) {
      char c = client.read();
      bodyBuf[p] = c;
      p++;
      if(p == BUF_SIZE-1) {
        break;
      }
    }
  }
  bodyBuf[p] = 0;
  return true;
}
