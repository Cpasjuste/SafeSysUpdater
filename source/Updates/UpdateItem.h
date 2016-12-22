//
// Created by cpasjuste on 11/01/16.
//

#ifndef PLAISYSUPDATER_UPDATEITEM_H
#define PLAISYSUPDATER_UPDATEITEM_H

#include <string>

class UpdateItem {

public:
    UpdateItem(const std::string& md5, const std::string& path);
    std::string getPath();
    std::string getMD5();

private:
    std::string path;
    std::string md5;

};

#endif //PLAISYSUPDATER_UPDATEITEM_H
