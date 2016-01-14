#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <algorithm>
#include <malloc.h>

#include "fs.h"
#include "misc.h"
#include "title.h"
#include "cfgutables.h"
#include "libmd5-rfc/md5.h"
#include "SuperUserLib3DS/libsu.h"
#include "Updates/UpdateInfo.h"
#include "Updates/UpdateInfoEur.h"
#include "Updates/UpdateInfoUsa.h"
#include "Updates/UpdateInfoJpn.h"

bool checkOnly = false;

typedef struct {
    char path[128];
    AM_TitleEntry entry;
    bool requiresDelete;
} TitleInstallInfo;

extern u32 getTitlePriority(u64 id);
extern bool sortTitlesLowToHigh(const TitleInstallInfo &a, const TitleInstallInfo &b);
extern int versionCmp(std::vector<TitleInfo> &installedTitles, u64 &titleID, u16 version);

struct SysInfo {
    u8 model;
    u8 region;
};

void gfxInit() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
}

void appExit() {
    amExit();
    cfguExit();
    sdmcArchiveExit();
    fsExit();
    gfxExit();
    hidExit();
    aptExit();
    srvExit();
}

SysInfo *getSysInfo() {
    SysInfo *sysInfo = (SysInfo *) malloc(sizeof(struct SysInfo));
    Result ret = CFGU_GetSystemModel(&sysInfo->model);
    if (R_FAILED(ret)) return NULL;
    ret = CFGU_SecureInfoGetRegion(&sysInfo->region);
    if (R_FAILED(ret)) return NULL;
    return sysInfo;
}

int checkMD5(const char *file, const char *md5) {

    md5_state_t state;
    md5_byte_t digest[16];
    unsigned char data[1024 * 16];
    int bytes;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        return 1;
    }

    md5_init(&state);
    while ((bytes = fread(data, 1, 1024 * 16, fp)) != 0)
        md5_append(&state, (const md5_byte_t *) data, bytes);
    md5_finish(&state, digest);
    fclose(fp);

    char rmd5[32];
    for (int i = 0; i < 16; i++) {
        sprintf(rmd5 + 2 * i, "%02x", digest[i]);
    }
    return strcmp(md5, rmd5);
}

int getAMu() {

    Handle amHandle = 0;

    // verify am:u access
    srvGetServiceHandleDirect(&amHandle, "am:u");
    if (amHandle) {
        svcCloseHandle(amHandle);
        return 0;
    }

    // try to get arm11
    if (suInit() == 0) {
        // verify am:u access
        srvGetServiceHandleDirect(&amHandle, "am:u");
        if (amHandle) {
            svcCloseHandle(amHandle);
            return 0;
        }
    }
    return 1;
}

int quit() {
    printf("press A key to exit...\n");
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_A) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    appExit();
    exit(EXIT_FAILURE);
}

bool isPressedY() {
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_Y) {
            return true;
        } else if (hidKeysDown() & KEY_A) {
            return false;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    return false;
}

UpdateInfo *getUpdateInfo(int model, int region) {
    switch (region) {
        case 0: // JPN
            return (UpdateInfo *) new UpdateInfoJpn(model);
        case 1: // USA
            return (UpdateInfo *) new UpdateInfoUsa(model);
        case 2: // EUR
            return (UpdateInfo *) new UpdateInfoEur(model);
        default:
            return NULL;
    }
    return NULL;
}

void downgrade() {

    printf("\ninit -> ");
    std::vector<TitleInstallInfo> titles;
    TitleInstallInfo installInfo;
    AM_TitleEntry ciaFileInfo;
    fs::File f;
    std::vector<TitleInfo> installedTitles = getTitleInfos(MEDIATYPE_NAND);
    printf("\x1b[32mGOOD\x1b[0m\n");

    // get system info
    printf("check system -> ");
    SysInfo *sysInfo = getSysInfo();
    if (sysInfo == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("Can't get system information...\n");
        quit();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // find right update information based on model/region
    printf("check update info -> ");
    UpdateInfo *update = getUpdateInfo(sysInfo->model, sysInfo->region);
    if (update == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("Can't find update config for your system...\n");
        quit();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check md5/add files to update list
    printf("\nChecking update integrity...\n\n");
    for (std::vector<UpdateItem>::iterator it = update->items.begin(); it != update->items.end(); ++it) {
        printf("MD5: %s -> ", it->getPath().c_str());
        if (checkMD5(it->getPath().c_str(), it->getMD5().c_str()) != 0) {
            printf("\x1b[31mFAIL\x1b[0m\n");
            quit();
        }
        printf("\x1b[32mGOOD\x1b[0m\n");

        if (!checkOnly) {
            char path[128];
            strncpy(path, it->getPath().c_str(), 128); // fsMakePath doesn't like std::string ?!
            FS_Path filePath = fsMakePath(PATH_ASCII, path);
            f.open(filePath, FS_OPEN_READ);
            if (AM_GetCiaFileInfo(MEDIATYPE_NAND, &ciaFileInfo, f.getFileHandle())) {
                printf("Can't get cia information (hax didn't succeed?)\n");
                quit();
            }
            f.close();
            int cmpResult = versionCmp(installedTitles, ciaFileInfo.titleID, ciaFileInfo.version);
            if (cmpResult != 0) {
                strncpy(installInfo.path, path, 128);
                installInfo.entry = ciaFileInfo;
                installInfo.requiresDelete = cmpResult < 0;
                titles.push_back(installInfo);
            }
        }
    }
    update->items.clear();
    std::sort(titles.begin(), titles.end(), sortTitlesLowToHigh);

    consoleClear();
    printf("\ndevice: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_MODEL_TABLE[(int) sysInfo->model],
           update->model.c_str());
    printf("region: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_REGION_TABLE[(int) sysInfo->region],
           update->region.c_str());
    printf("downgrade to: \x1b[32m%s\x1b[0m\n", update->version.c_str());
    printf("\n\x1b[32mSEEMS GOOD -> \x1b[0m");
    if (checkOnly) {
        printf("\n\n\x1b[32m-> UPDATE FILES ARE GOOD <-\x1b[0m\n\n");
        quit();
    }

    // downgrade !
    printf("\x1b[31mDOWNGRADING !!\x1b[0m\n\n");
    for (auto it : titles) {
        bool nativeFirm = it.entry.titleID == 0x0004013800000002LL || it.entry.titleID == 0x0004013820000002LL;
        if (nativeFirm) {
            printf("NATIVE_FIRM -> ");
        } else {
            printf("%s -> ", it.path);
        }
        if (it.requiresDelete) deleteTitle(MEDIATYPE_NAND, it.entry.titleID);
        installCia(it.path, MEDIATYPE_NAND);
        if (nativeFirm && AM_InstallFirm(it.entry.titleID)) {
            printf("\x1b[31mFAIL ... trying again\x1b[0m\n");
            if (AM_InstallFirm(it.entry.titleID)) {
                printf("\x1b[31mFAIL.\nYOU MAY BE BRICKED.\nPOWER OFF YOUR DEVICE AND HOLD L + R + A + DPAD UP + POWER TO BOOT RECOVERY AND TRY A UPDATE.\x1b[0m\n");
                quit();
            }
        }
        printf("\x1b[32mINSTALLED\x1b[0m\n");
    }
    free(sysInfo);
    free(update);

    printf("\n\nDowngrade completed. Trying to reboot in 10 sec...\nPower off your device manually if it doesn't...\n");
    svcSleepThread(10000000000LL);
    printf("Trying to reboot...\n");
    while(aptInit()!=0) {};
    aptOpenSession();
    while(APT_HardwareResetAsync()!=0) {};
    aptCloseSession();
}

int main(int argc, char *argv[]) {

    gfxInit();
    printf("\nSafeSysUpdater @ Cpasjuste\n\nSysUpdater @ profi200\n\nmemchunkhax2 @ Steveice10\n\nPress (Y) to downgrade...\nPress (A) to check update files (recommended)...\n");
    checkOnly = !isPressedY();
    consoleClear();

    if (!checkOnly) {
        gfxExit();
        if (getAMu() != 0) {
            gfxInit();
            printf("\x1b[31mFAIL\x1b[0m\nCan't get am:u service... Try again :x\n");
            quit();
        }
        gfxInit();
        printf("\x1b[32mHAX SUCCESS !\x1b[0m\n");
    }

    // late init
    srvInit();
    fsInit();
    sdmcArchiveInit();
    cfguInit();
    hidInit();
    amInit();

    downgrade();

    return 0;
}
