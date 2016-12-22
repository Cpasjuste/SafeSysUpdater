//
// Created by cpasjuste on 15/01/16.
//

#ifndef PLAISYSUPDATER_DEBUG_H
#define PLAISYSUPDATER_DEBUG_H

#include <stdio.h>

class Debug {

public:

    Debug();

    ~Debug();

    void write(const char *format, ...);

    void print(const char *format, ...);

    void printr(const char *format, ...);

    void printg(const char *format, ...);

private:

    FILE *fp;

};


#endif //PLAISYSUPDATER_DEBUG_H
