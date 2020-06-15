#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

char buf[1024];

void activity::log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    Serial.println(buf);
}
