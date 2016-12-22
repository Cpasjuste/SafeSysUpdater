//
// Created by cpasjuste on 14/01/16.
//

#ifndef PLAISYSUPDATER_UTILITY_H
#define PLAISYSUPDATER_UTILITY_H

#include <string>
#include <vector>
#include <algorithm>
#include <3ds.h>

#include "TitleInfo.h"

class Utility {

public:

    struct SysInfo {
        u8 model;
        u8 region;
    };

    static std::vector<TitleInfo> getTitles();

    static TitleInfo getTitleInfo(const std::string &path);

    static bool deleteTitle(u64 titleID);

    static bool installTitle(std::string path);

    static u32 getTitlePriority(u64 id);

    static bool sortTitles(const TitleInfo &a, const TitleInfo &b);

    static int cmp(std::vector<TitleInfo> &installedTitles, u64 &titleID, u16 version);

    static int version(std::vector<TitleInfo> &titles, u64 &titleID);

    static SysInfo *getSysInfo();

    static int getAMu();

    static int checkMD5(const char *file, const char *md5);

    static void sdmcArchiveInit();

    static void sdmcArchiveExit();

private:

    static Handle openFileHandle(const std::string &path);

    static void closeFileHandle(const Handle &handle);
};

#endif //PLAISYSUPDATER_UTILITY_H
