//
// Created by cpasjuste on 11/01/16.
//

#include "UpdateItem.h"

UpdateItem::UpdateItem(const std::string& md5, const std::string& path) {
    this->path = path;
    this->md5 = md5;
}

std::string UpdateItem::getPath() {
    return this->path;
}

std::string UpdateItem::getMD5() {
    return this->md5;
}
