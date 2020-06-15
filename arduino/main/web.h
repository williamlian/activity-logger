#ifndef ACTIVITY_WEB_H
#define ACTIVITY_WEB_H
#include <WiFiSSLClient.h>
#include <stdio.h>
#include "secrets.h"

namespace activity {
    class WebClient {
        public:
            static const int USER_ID = 0;
            static const int BUF_SIZE = 5;
            WebClient() {
                sprintf(host, "Host: %s", ACTIVITY_SECRET_SERVER);
            }
            int getLastActivityType();
            bool startActivity(int type);
            bool endActivity(int type);
        protected:
            WiFiSSLClient client;
            int statusCode;
            char path[256];
            char host[256];
            char statusCodeBuf[BUF_SIZE];
            char bodyBuf[BUF_SIZE];
            bool request();
            bool readResponse();
    };
}

#endif
