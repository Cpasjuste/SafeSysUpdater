#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <algorithm>

#include "Updates/UpdateInfo.h"
#include "cfgutables.h"
#include "libmd5-rfc/md5.h"
#include "SuperUserLib3DS/libsu.h"
#include "fs.h"
#include "misc.h"
#include "title.h"
#include "Updates/UpdateInfoN3dsEur.h"
#include "Updates/UpdateInfoN3dsUsa.h"
#include "Updates/UpdateInfoO3dsUsa.h"
#include "Updates/UpdateInfoO3dsEur.h"
#include "memchunkhax2/source/memchunkhax2.h"

#ifndef CITRA
#include "ctr_shell.h"
#endif

bool simulation = false;

typedef struct {
    char path[128];
    AM_TitleEntry entry;
    bool requiresDelete;
} TitleInstallInfo;

// Ordered from highest to lowest priority.
static const u32 titleTypes[7] = {
        0x00040138, // System Firmware
        0x00040130, // System Modules
        0x00040030, // Applets
        0x00040010, // System Applications
        0x0004001B, // System Data Archives
        0x0004009B, // System Data Archives (Shared Archives)
        0x000400DB, // System Data Archives
};

extern u32 getTitlePriority(u64 id);
extern bool sortTitlesHighToLow(const TitleInstallInfo &a, const TitleInstallInfo &b);
extern bool sortTitlesLowToHigh(const TitleInstallInfo &a, const TitleInstallInfo &b);
extern int versionCmp(std::vector<TitleInfo> &installedTitles, u64 &titleID, u16 version);

struct SysInfo {
    u8 model;
    u8 region;
};

void appInit() {
    // Initialize services
    srvInit();
    aptInit();
    gfxInitDefault();
    hidInit();
    fsInit();
    sdmcArchiveInit();
    cfguInit();
    amInit();
    consoleInit(GFX_TOP, NULL);
}

void appExit() {
    // Exit services
    amExit();
    cfguExit();
    sdmcArchiveExit();
    fsExit();
    hidExit();
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
        sprintf(rmd5 + 2*i, "%02x", digest[i]);
    }
    return strcmp(md5, rmd5);
}

int getAMu() {

    // try to get arm11
    //suInit();

    // verify am:u access
    Handle amHandle = 0;
    srvGetServiceHandleDirect(&amHandle, "am:u");
    if(amHandle) {
        svcCloseHandle(amHandle);
        return 0;
    }

    // no am:u access, try memchunkhax2
    u8 ret = execute_memchunkhax2();
    if(ret == 1) {
        srvGetServiceHandleDirect(&amHandle, "am:u");
        if(amHandle) {
            svcCloseHandle(amHandle);
            return 0;
        }
    }
    return 1;
}

int waitExitKey() {
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

void waitY() {
    bool quit = false;
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_Y) {
            break;
        } else if (hidKeysDown() & KEY_A) {
            quit = true; break;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    if(quit) {
        appExit();
        exit(EXIT_FAILURE);
    }
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
    switch(region) {
        case 0: { // JPN
            if(model == 2 || model == 4) { // n3DS
                //TODO
                break;
            } else {
                //TODO
                break;
            }
        }
        case 1: { // USA
            if(model == 2 || model == 4) { // n3DS
                return (UpdateInfo*)new UpdateInfoN3dsUsa();
            } else {
                return (UpdateInfo*)new UpdateInfoO3dsUsa();
            }
        }
        case 2: { // EUR
            if(model == 2 || model == 4) { // n3DS
                return (UpdateInfo*)new UpdateInfoN3dsEur();
            } else {
                return (UpdateInfo*)new UpdateInfoO3dsEur();
            }
        }
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
        waitExitKey();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    printf("check update info -> ");
    UpdateInfo *update = getUpdateInfo(sysInfo->model, sysInfo->region);
    //UpdateInfo *update = getUpdateInfo(2, 2);
    if(update == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("can't find update config for your system...\n");
        waitExitKey();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check md5/add files
    printf("\nChecking update integrity...\n\n");
    for (std::vector<UpdateItem>::iterator it = update->items.begin() ; it != update->items.end(); ++it) {
        printf("MD5: %s -> ", it->getPath().c_str());
        if(checkMD5(it->getPath().c_str(), it->getMD5().c_str()) != 0) {
            printf("\x1b[31mFAIL\x1b[0m\n");
            waitExitKey();
        }
        printf("\x1b[32mGOOD\x1b[0m\n");

        if(!simulation) {
            // add to titles list
            char path[128];
            strncpy(path, it->getPath().c_str(), 128); // fsMakePath doesn't like std::string ?!
            FS_Path filePath = fsMakePath(PATH_ASCII, path);
            f.open(filePath, FS_OPEN_READ);
            if (AM_GetCiaFileInfo(MEDIATYPE_NAND, &ciaFileInfo, f.getFileHandle())) {
                printf("can't get cia information (hax didn't succeed?)\n");
                waitExitKey();
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

    consoleClear();
    printf("\ndevice: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_MODEL_TABLE[(int) sysInfo->model], update->model.c_str());
    printf("region: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_REGION_TABLE[(int) sysInfo->region], update->region.c_str());
    printf("downgrade to: \x1b[32m%s\x1b[0m\n", update->version.c_str());
    printf("\n\x1b[32mSEEMS GOOD\x1b[0m\n\n");

    if(simulation) {
        printf("\x1b[32m-> UPDATE FILES ARE GOOD <-\x1b[0m\n\n");
        waitExitKey();
    }

    printf("press (Y) to downgrade...\n");
    printf("press (A) to cancel...\n");
    waitY();
    consoleClear();

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
                waitExitKey();
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
#ifndef CITRA
    ctr_shell_init(NULL, 3333);
#endif
    appInit();
    osSetSpeedupEnable(false); // disable speedup for stability ?

    printf("\nSafeSysUpdater @ Cpasjuste\n");
    printf("\nSysUpdater @ profi200\n");
    printf("\nmemchunkhax2 @ Steveice10\n\n");

    printf("press (Y) to downgrade...\n");
    printf("press (A) to check update files...\n");
    u32 key = waitKeyYA();
    if(key == KEY_A) simulation = true;
    consoleClear();

    if(!simulation) {
        printf("getting am:u access -> ");
        if (getAMu() != 0) {
            printf("\x1b[31mFAIL\x1b[0m\n");
            printf("can't get am:u service...try again :x\n");
            waitExitKey();
        }
        printf("\x1b[32mGOOD\x1b[0m\n");
    }

    downgrade();
    waitExitKey();

#ifndef CITRA
    ctr_shell_exit();
#endif
    return 0;
}
