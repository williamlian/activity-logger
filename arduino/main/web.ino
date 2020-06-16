#include <stdio.h>
#include <stdlib.h>
#include <Arduino_JSON.h>
#include <Scheduler.h>

#include "web.h"
#include "logger.h"

using namespace activity;

const int WAIT_DELAY = 100;

const char *startPath = "/activity/start";
const char *endPath = "/activity/end";
const char *lastActivityPath = "/activity/get?type=last&userID=%d";
const char *requestBody = "{\"UserID\":%d,\"TypeID\":%d}";
const char *jsonContentType = "text/json";

int WebClient::getLastActivityType(int userID) {
  snprintf(path, PATH_BUF_SIZE, lastActivityPath, userID);
  if(int code = http.get(path)) {
    log("[web] failed to send request for last activity, error code %d", code);
    return -1;
  };
  waitForResponse();
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();
  if(statusCode != 200) {
    log("[web] bad resposne code from last activity: %d", statusCode);
    return -1;
  }
  JSONVar jsonResponse = JSON.parse(response);
  int activittType = (int)jsonResponse["Type"]["ID"];
  if(jsonResponse["EndedAt"] == null) {
    return activittType;
  }
  log("[web] last activity %d ended at: %d", activittType, (int)jsonResponse["EndedAt"]);
  return -1;
}

bool WebClient::startActivity(int userID, int type) {
  snprintf(data, DATA_BUF_SIZE, requestBody, userID, type);
  log("[web] 1. start request");
  if(int code = http.post(startPath, jsonContentType, data)) {
    log("[web] failed to send request for start, error code %d", code);
    return false;
  }
  log("[web] 2. end request");
  int statusCode = http.responseStatusCode();
  waitForResponse();
  log("[web] 3. get resposne code");
  String response = http.responseBody();
  log("[web] 4. get body");
  if(statusCode != 200) {
    log("[web] bad resposne code from start: [%d] %s", statusCode, response.c_str());
    return false;
  }
  return true;
}

bool WebClient::endActivity(int userID, int type) {
  snprintf(data, DATA_BUF_SIZE, requestBody, userID, type);
  if(int code = http.post(endPath, jsonContentType, data)) {
    log("[web] failed to send request for end, error code %d", code);
    return false;
  }
  waitForResponse();
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();
  if(statusCode != 200) {
    log("[web] bad resposne code from end: [%d] %s", statusCode, response.c_str());
    return false;
  }
  return true;
}

void WebClient::waitForResponse() {
  while(!http.available()) {
    yield();
  }
}
