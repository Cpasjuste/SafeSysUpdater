#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <algorithm>

#include "cfgutables.h"
#include "libmd5-rfc/md5.h"
#include "SuperUserLib3DS/libsu.h"
#include "fs.h"
#include "misc.h"
#include "title.h"

#ifndef CITRA
#include "ctr_shell.h"
#endif

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
        printf("%s can't be opened.\n", file);
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
    //printf("%s - %s\n", md5, rmd5);
    return strcasecmp(md5, rmd5);
}

int getAMu() {
    // try to get arm11
    suInit();
    // verify am:u access
    Handle amHandle = 0;
    srvGetServiceHandleDirect(&amHandle, "am:u");
    if(amHandle) {
        svcCloseHandle(amHandle);
        return 0;
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

void installUpdates(bool downgrade) {

    std::vector<fs::DirEntry> filesDirs = fs::listDirContents(u"/updates", u".cia;"); // Filter for .cia files
    printf("Please wait...\n\n");
    std::vector<TitleInfo> installedTitles = getTitleInfos(MEDIATYPE_NAND);
    std::vector<TitleInstallInfo> titles;

    Buffer<char> tmpStr(256);
    Result res;
    TitleInstallInfo installInfo;
    AM_TitleEntry ciaFileInfo;
    fs::File f;

    char updateFW[64], updateModel[64], updateRegion[64];

    // get system info
    SysInfo *sysInfo = getSysInfo();
    if (sysInfo == NULL) {
        printf("can't get system information...\n");
        waitExitKey();
    }

    // open update configuration file
    printf("check config file -> ");
    char cfg[256];
    sprintf(cfg, "/updates/%s_%s.cfg",
            CFGU_MODEL_TABLE[(int) sysInfo->model], CFGU_REGION_TABLE[(int) sysInfo->region]);
    FILE *fp = fopen(cfg, "rb");
    if (fp == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("could not open %s\n", cfg);
        waitExitKey();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check first line for update info
    printf("check update infos -> ");
    char line[128];
    fgets(line, 128, fp);
    if(strlen(line) <= 0) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("invalid configuration\n");
        waitExitKey();
    }
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check if model match
    printf("check model -> ");
    char *token = strtok (line, " ");
    if(token == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("invalid configuration\n");
        waitExitKey();
    } else if(strcasecmp(token, CFGU_MODEL_TABLE[(int)sysInfo->model]) != 0) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("model does not match: update:%s/device:%s\n", token, CFGU_MODEL_TABLE[(int)sysInfo->model]);
        waitExitKey();
    }
    strncpy(updateModel, token, 64);
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check if region match
    printf("check region -> ");
    token = strtok (NULL, " ");
    if(token == NULL) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("invalid configuration\n");
        waitExitKey();
    } else if(strcasecmp(token, CFGU_REGION_TABLE[(int)sysInfo->region]) != 0) {
        printf("\x1b[31mFAIL\x1b[0m\n");
        printf("region does not match: update:%s/device:%s\n", token, CFGU_REGION_TABLE[(int)sysInfo->region]);
        waitExitKey();
    }
    strncpy(updateRegion, token, 64);
    printf("\x1b[32mGOOD\x1b[0m\n");

    // check update firmware version
    token = strtok (NULL, " ");
    if(token != NULL) {
        strncpy(updateFW, token, 64);
    }

    // check md5/add files
    printf("\nChecking update integrity...\n");
    char *md5 = (char*)malloc(sizeof(char)*33);
    char *cia = (char*)malloc(sizeof(char)*65);

    while(fgets(line, 128, fp) != NULL) {
        if(strlen(line) <= 16) continue;
        token = strtok (line, " ");
        memset(md5, 0, 33); strncpy(md5, token, 32);
        token = strtok (NULL, " ");
        memset(cia, 0, 65); snprintf(cia, 64, "/updates/%s", token);
        cia[strlen(cia)-1] = '\0';
        printf("MD5: %s -> ", cia);
        if(checkMD5(cia, md5) != 0) {
            printf("\x1b[31mFAIL\x1b[0m\n");
            waitExitKey();
        }
        printf("\x1b[32mGOOD\x1b[0m\n");

        FS_Path filePath = fsMakePath(PATH_ASCII, cia);
        f.open(filePath, FS_OPEN_READ);
        if (AM_GetCiaFileInfo(MEDIATYPE_NAND, &ciaFileInfo, f.getFileHandle())) {
            printf("can't get cia information (hax didn't succeed?)\n");
            waitExitKey();
        }

        int cmpResult = versionCmp(installedTitles, ciaFileInfo.titleID, ciaFileInfo.version);
        if ((downgrade && cmpResult != 0) || (cmpResult > 0)) {
            strncpy(installInfo.path, cia, 128);
            installInfo.entry = ciaFileInfo;
            installInfo.requiresDelete = downgrade && cmpResult < 0;
            titles.push_back(installInfo);
        }
    }
    free(md5); free(cia);
    fclose(fp);

    std::sort(titles.begin(), titles.end(), downgrade ? sortTitlesLowToHigh : sortTitlesHighToLow);

    consoleClear();
    printf("\ndevice: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_MODEL_TABLE[(int) sysInfo->model], updateModel);
    printf("region: %s, update: %s -> \x1b[32mGOOD\x1b[0m\n\n", CFGU_REGION_TABLE[(int) sysInfo->region], updateRegion);
    printf("downgrade to: \x1b[32m%s\x1b[0m\n", updateFW);
    printf("\n\x1b[32mSEEMS GOOD\x1b[0m\n\n");
    printf("press (Y) to downgrade...\n");
    printf("press (A) to cancel...\n");
    waitY();
    consoleClear();

    for (auto it : titles) {
        bool nativeFirm = it.entry.titleID == 0x0004013800000002LL || it.entry.titleID == 0x0004013820000002LL;
        if (nativeFirm) {
            printf("NATIVE_FIRM -> ");
        } else {
            printf("%s -> ", it.path);
        }

        if (it.requiresDelete) deleteTitle(MEDIATYPE_NAND, it.entry.titleID);
        installCia(it.path, MEDIATYPE_NAND);
        if (nativeFirm && (res = AM_InstallFirm(it.entry.titleID))) {
            //    throw titleException("main.cpp", __LINE__, res, "Failed to install NATIVE_FIRM!");
            printf("\x1b[31mFAIL ... trying again\x1b[0m\n");
            if (nativeFirm && (res = AM_InstallFirm(it.entry.titleID))) {
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

    printf("executing memchunkhax2...\n");
    if(getAMu() != 0) {
        printf("can't get am:u service...\n");
        waitExitKey();
    }
    consoleClear();
    installUpdates(true);
    waitExitKey();

#ifndef CITRA
    ctr_shell_exit();
#endif
    return 0;
}
