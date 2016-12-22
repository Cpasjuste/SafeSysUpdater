//
// Created by cpasjuste on 15/01/16.
//

#ifndef PLAISYSUPDATER_TITLEINFO_H
#define PLAISYSUPDATER_TITLEINFO_H

#include <string>
#include <3ds/types.h>

class TitleInfo {

public:

    TitleInfo() {
        titleID = 0;
    }

    TitleInfo(const std::string &p, u64 t, u16 v) {
        path = p;
        titleID = t;
        version = v;
    }

    std::string path;
    u64 titleID;
    u16 version;
    bool deleteNeeded;
};

#endif //PLAISYSUPDATER_TITLEINFO_H
