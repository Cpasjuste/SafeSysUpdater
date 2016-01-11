//
// Created by cpasjuste on 11/01/16.
//

#ifndef SAFESYSUPDATER_UPDATEITEM_H
#define SAFESYSUPDATER_UPDATEITEM_H

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

#endif //SAFESYSUPDATER_UPDATEITEM_H
