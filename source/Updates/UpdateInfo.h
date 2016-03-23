//
// Created by cpasjuste on 11/01/16.
//

#ifndef PLAISYSUPDATER_UPDATEINFO_H
#define PLAISYSUPDATER_UPDATEINFO_H

#include <string>
#include <vector>
#include "UpdateItem.h"

class UpdateInfo {

public:
    std::string model;
    std::string region;
    std::string version;
    std::vector<UpdateItem> items;
};


#endif //PLAISYSUPDATER_UPDATEINFO_H
