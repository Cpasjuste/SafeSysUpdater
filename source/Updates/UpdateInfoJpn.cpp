//
// Created by cpasjuste on 12/01/16.
//

#include "UpdateInfoJpn.h"

UpdateInfoJpn::UpdateInfoJpn(int deviceType) {

    this->region = "JPN";
    this->version = "9.2.0-20";

    if(deviceType == 2  || deviceType == 4) {

        this->model = "n3DS";


    } else {

        this->model = "o3DS";

    }

}
