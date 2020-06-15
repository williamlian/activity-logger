#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

const int BUF_SIZE = 1024;
char buf[BUF_SIZE];

void activity::log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, BUF_SIZE, fmt, args);
    va_end(args);
    Serial.println(buf);
}
