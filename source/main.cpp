#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <algorithm>

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

bool simulation = false;

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

void appInit() {
    gfxInitDefault();
    fsInit();
    sdmcArchiveInit();
    cfguInit();
    consoleInit(GFX_TOP, NULL);
}

void appExit() {
    amExit();
    cfguExit();
    sdmcArchiveExit();
    fsExit();
    gfxExit();
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

u32 waitKeyYA() {
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_Y) {
            return KEY_Y;
        } else if (hidKeysDown() & KEY_A) {
            return KEY_A;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}

UpdateInfo *getUpdateInfo(int model, int region) {
    switch (region) {
        case 0: // JPN
            break;
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
    std::vector<TitleInfo> installedTitles = getTitleInfos(MEDIATYPE_NAND);
    std::vector<TitleInstallInfo> titles;
    TitleInstallInfo installInfo;
    AM_TitleEntry ciaFileInfo;
    fs::File f;
    printf("\x1b[32mGOOD\x1b[0m\n");

    // get system info
    printf("check system -> ");
    SysInfo *sysInfo = getSysInfo();
    if (sysInfo == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("can't get system information...\n");
        quit();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // find right update information based on model/region
    printf("check update info -> ");
    UpdateInfo *update = getUpdateInfo(sysInfo->model, sysInfo->region);
    if (update == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("can't find update config for your system...\n");
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

        if (!simulation) {
            char path[128];
            strncpy(path, it->getPath().c_str(), 128); // fsMakePath doesn't like std::string ?!
            FS_Path filePath = fsMakePath(PATH_ASCII, path);
            f.open(filePath, FS_OPEN_READ);
            if (AM_GetCiaFileInfo(MEDIATYPE_NAND, &ciaFileInfo, f.getFileHandle())) {
                printf("can't get cia information (hax didn't succeed?)\n");
                quit();
            }
            int cmpResult = versionCmp(installedTitles, ciaFileInfo.titleID, ciaFileInfo.version);
            if (cmpResult != 0) {
                strncpy(installInfo.path, path, 128);
                installInfo.entry = ciaFileInfo;
                installInfo.requiresDelete = cmpResult < 0;
                titles.push_back(installInfo);
            }
        }
    }

    // give a way to cancel now ...
    consoleClear();
    printf("\ndevice: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_MODEL_TABLE[(int) sysInfo->model],
           update->model.c_str());
    printf("region: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_REGION_TABLE[(int) sysInfo->region],
           update->region.c_str());
    printf("downgrade to: \x1b[32m%s\x1b[0m\n", update->version.c_str());
    printf("\n\x1b[32mSEEMS GOOD\x1b[0m\n\n");
    if (simulation) {
        printf("\x1b[32m-> UPDATE FILES ARE GOOD <-\x1b[0m\n\n");
        quit();
    }
    printf("press (Y) to downgrade...\n");
    printf("press (A) to cancel...\n");
    if(waitKeyYA() == KEY_A) {
        appExit();
        exit(EXIT_FAILURE);
    }
    consoleClear();

    // downgrade !
    std::sort(titles.begin(), titles.end(), sortTitlesLowToHigh);
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
            if (nativeFirm && AM_InstallFirm(it.entry.titleID)) {
                printf("\x1b[31mFAIL\x1b[0m\n");
                printf("\x1b[31mYou should be able to use recovery to fix...\x1b[0m\n");
                quit();
            }
        }
        printf("\x1b[32mINSTALLED\x1b[0m\n");
    }

    if (sysInfo != NULL) {
        free(sysInfo);
    }
    if (update != NULL) {
        free(update);
    }

    printf("\n\nUpdates installed. Rebooting in 10 seconds...\n");
    svcSleepThread(10000000000LL);
    aptOpenSession();
    APT_HardwareResetAsync();
    aptCloseSession();
}

int main(int argc, char *argv[]) {

    appInit();

    printf("\nSafeSysUpdater @ Cpasjuste\n");
    printf("\nSysUpdater @ profi200\n");
    printf("\nmemchunkhax2 @ Steveice10\n\n");

    printf("press (Y) to downgrade...\n");
    printf("press (A) to check update files...\n");
    u32 key = waitKeyYA();
    if (key == KEY_A) simulation = true;
    consoleClear();

    if (!simulation) {
        gfxExit();
        if (getAMu() != 0) {
            gfxInitDefault();
            consoleInit(GFX_TOP, NULL);
            printf("\x1b[31mFAIL\x1b[0m\n");
            printf("can't get am:u service ... try again :x\n");
            quit();
        }
        gfxInitDefault();
        consoleInit(GFX_TOP, NULL);
        printf("\x1b[32mHAX SUCCESS !\x1b[0m\n");
    }

    srvInit();
    aptInit();
    amInit();
    downgrade();
    quit();

    return 0;
}
