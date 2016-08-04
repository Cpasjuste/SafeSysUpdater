//
// Created by cpasjuste on 14/01/16.
//

#include <string.h>
#include <stdio.h>
#include "Utility.h"
#include "libmd5-rfc/md5.h"

#define BUFSIZE 131072
static FS_Archive sdmcArchive;

extern void _gfxInit();
extern bool isNew3DS;
extern "C" {
    void svchax_init();
}

static const u32 titleTypes[7] = {
    0x00040138, // System Firmware
    0x00040130, // System Modules
    0x00040030, // Applets
    0x00040010, // System Applications
    0x0004001B, // System Data Archives
    0x0004009B, // System Data Archives (Shared Archives)
    0x000400DB, // System Data Archives
};

static const u64 titleHomeMenu[3] = {
    0x0004003000008F02, // USA	Home Menu
    0x0004003000009802, // EUR	Home Menu
    0x0004003000008202, // JPN	Home Menu
};

static const u64 titleBrowser[6] = {
    0x0004003020009402, // USA	New3DS Internet Browser
    0x0004003020009D02, // EUR	New3DS Internet Browser
    0x0004003020008802, // JPN	New3DS Internet Browser
    0x0004003000009402, // USA Internet Browser
    0x0004003000009D02, // EUR Internet Browser
    0x0004003000008802, // JPN Internet Browser
};

u32 Utility::getTitlePriority(u64 id) {

    // nfirm last
    if( id == 0x0004013800000002LL
        || id == 0x0004013820000002LL) {
        return 0;
    }

    // downgrade browser and homemenu last
    for (u32 i = 0; i < 6; i++) {
        if (id == titleBrowser[i]) {
            return 2;
        }
    }
    for (u32 i = 0; i < 3; i++) {
        if (id == titleHomeMenu[i]) {
            return 1;
        }
    }
    u32 type = (u32) (id >> 32);
    for (u32 i = 0; i < 7; i++) {
        if (type == titleTypes[i]) {
            return i+3;
        }
    }
    return 0;
}

bool Utility::sortTitles(const TitleInfo &a, const TitleInfo &b) {
    bool aSafe = (a.titleID & 0xFF) == 0x03;
    bool bSafe = (b.titleID & 0xFF) == 0x03;
    if (aSafe != bSafe) {
        return aSafe;
    }
    return getTitlePriority(a.titleID) > getTitlePriority(b.titleID);
}

int Utility::cmp(std::vector<TitleInfo> &titles, u64 &titleID, u16 version) {
    for (auto it : titles) {
        if (it.titleID == titleID) {
            return (version - it.version);
        }
    }
    return 1;
}

int Utility::version(std::vector<TitleInfo> &titles, u64 &titleID) {
    for (auto it : titles) {
        if (it.titleID == titleID) {
            return it.version;
        }
    }
    return 0;
}

std::vector<TitleInfo> Utility::getTitles() {

    std::vector<TitleInfo> titles;

    u32 count = 0;
    if (AM_GetTitleCount(MEDIATYPE_NAND, &count)) {
        return titles;
    }

    u64 titlesId[count];
    
    {
        u32 throwaway;
        if (AM_GetTitleList(&throwaway, MEDIATYPE_NAND, count, titlesId)) {
            return titles;
        }
    }

    AM_TitleEntry titleList[count];
    if (AM_GetTitleInfo(MEDIATYPE_NAND, count, titlesId, titleList)) {
        return titles;
    }

    for (int i = 0; i < count; i++) {
        TitleInfo title("NA", titleList[i].titleID, titleList[i].version);
        titles.push_back(title);
    }

    std::sort(titles.begin(), titles.end(), sortTitles);

    return titles;
}

TitleInfo Utility::getTitleInfo(const std::string &path) {

    TitleInfo title;
    AM_TitleEntry amTitle;

    Handle fileHandle = openFileHandle(path);
    if (AM_GetCiaFileInfo(MEDIATYPE_NAND, &amTitle, fileHandle)) {
        return title;
    }
    closeFileHandle(fileHandle);

    title.path = path;
    title.titleID = amTitle.titleID;
    title.version = amTitle.version;

    return title;
}

bool Utility::deleteTitle(u64 titleID) {

    if (titleID >> 32 & 0xFFFF) {
        if (AM_DeleteTitle(MEDIATYPE_NAND, titleID)) {
            return false;
        }
    } else if (AM_DeleteAppTitle(MEDIATYPE_NAND, titleID)) {
        return false;
    }
    return true;
}

bool Utility::installTitle(std::string path) {

    Handle fileHandle, ciaHandle = 0;
    u64 size, bytesToRead, i = 0;
    char *ciaBuffer;
    u32 bytes;

    fileHandle = openFileHandle(path);
    FSFILE_GetSize(fileHandle, &size);

    if (AM_StartCiaInstall(MEDIATYPE_NAND, &ciaHandle)) {
        return false;
    }

    ciaBuffer = (char *) malloc(BUFSIZE);
    memset(ciaBuffer, 0, BUFSIZE);

    while (i < size) {

        bytesToRead = i + BUFSIZE > size ? size - i : BUFSIZE;

        if (FSFILE_Read(fileHandle, &bytes, i, ciaBuffer, bytesToRead)) {
            AM_CancelCIAInstall(ciaHandle);
            FSFILE_Close(fileHandle);
            free(ciaBuffer);
            return false;
        }

        if (FSFILE_Write(ciaHandle, &bytes, i, ciaBuffer, bytesToRead, FS_WRITE_FLUSH)) {
            AM_CancelCIAInstall(ciaHandle);
            FSFILE_Close(fileHandle);
            free(ciaBuffer);
            return false;
        }

        i += bytesToRead;
    }

    if (AM_FinishCiaInstall(ciaHandle)) {
        AM_CancelCIAInstall(ciaHandle);
        FSFILE_Close(fileHandle);
        free(ciaBuffer);
        return false;
    }

    FSFILE_Close(fileHandle);
    free(ciaBuffer);

    return 0;
}

Utility::SysInfo *Utility::getSysInfo() {
    SysInfo *sysInfo = (SysInfo *) malloc(sizeof(struct SysInfo));
    Result ret = CFGU_GetSystemModel(&sysInfo->model);
    if (R_FAILED(ret)) return NULL;
    ret = CFGU_SecureInfoGetRegion(&sysInfo->region);
    if (R_FAILED(ret)) return NULL;
    return sysInfo;
}

int Utility::getAMu() {

    Handle amHandle = 0;

    // verify am:u access
    srvGetServiceHandleDirect(&amHandle, "am:u");
    if (amHandle) {
        svcCloseHandle(amHandle);
        return 0;
    }

    // try to get arm11
    svchax_init();
    aptInit();

    srvGetServiceHandleDirect(&amHandle, "am:u");
    if (amHandle) {
        svcCloseHandle(amHandle);
        return 0;
    }

    return 1;
}

int Utility::checkMD5(const char *file, const char *md5) {

    md5_state_t state;
    md5_byte_t digest[16];
    int bytes;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        return 1;
    }

    unsigned char *data = (unsigned char *) malloc(1024 * 32);
    md5_init(&state);
    while ((bytes = fread(data, 1, 1024 * 32, fp)) != 0) {
        md5_append(&state, (const md5_byte_t *) data, bytes);
    }
    md5_finish(&state, digest);
    free(data);
    fclose(fp);

    char rmd5[32];
    for (int i = 0; i < 16; i++) {
        sprintf(rmd5 + 2 * i, "%02x", digest[i]);
    }

    return strcmp(md5, rmd5);
}

Handle Utility::openFileHandle(const std::string &path) {

    char tmp[128];
    Handle fileHandle = 0;

    strncpy(tmp, path.c_str(), 128); // fsMakePath doesn't like std::string ?!
    FS_Path filePath = fsMakePath(PATH_ASCII, tmp);
    if (FSUSER_OpenFile(&fileHandle, sdmcArchive, filePath, FS_OPEN_READ & 3, 0)) {
        FSUSER_OpenFile(&fileHandle, sdmcArchive, filePath, FS_OPEN_READ, 0);
    }
    return fileHandle;
}

void Utility::closeFileHandle(const Handle &handle) {
    FSFILE_Close(handle);
}

void Utility::sdmcArchiveInit() {
    FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, (FS_Path) {PATH_EMPTY, 1, (u8 *) ""});
}

void Utility::sdmcArchiveExit() {
    FSUSER_CloseArchive(sdmcArchive);
}
