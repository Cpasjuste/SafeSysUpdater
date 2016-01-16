//
// Created by cpasjuste on 15/01/16.
//

#include <stdarg.h>
#include "Debug.h"

Debug::Debug() {
    fp = fopen("/SafeSys.log", "w");
}

Debug::~Debug() {
    if (fp != NULL) {
        fclose(fp);
    }
}

void Debug::write(const char *format, ...) {

    if (fp != NULL) {
        char msg[1024];
        va_list argp;
        va_start(argp, format);
        vsnprintf(msg, 1024, format, argp);
        va_end(argp);
        fputs(msg, fp);
        fflush(fp);
    }
}


void Debug::print(const char *format, ...) {

    char msg[1024];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 1024, format, argp);
    va_end(argp);

    printf(msg);

    if (fp != NULL) {
        fputs(msg, fp);
        fflush(fp);
    }
}

void Debug::printr(const char *format, ...) {

    char msg[1024];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 1024, format, argp);
    va_end(argp);

    printf("\x1b[31m%s\x1b[0m", msg);

    if (fp != NULL) {
        fputs(msg, fp);
        fflush(fp);
    }
}

void Debug::printg(const char *format, ...) {

    char msg[1024];
    va_list argp;
    va_start(argp, format);
    vsnprintf(msg, 1024, format, argp);
    va_end(argp);

    printf("\x1b[32m%s\x1b[0m", msg);

    if (fp != NULL) {
        fputs(msg, fp);
        fflush(fp);
    }
}
