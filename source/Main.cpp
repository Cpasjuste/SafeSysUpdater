#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <algorithm>
#include <malloc.h>

#include "cfgutables.h"
#include "Updates/UpdateInfo.h"
#include "Updates/UpdateInfoEur.h"
#include "Updates/UpdateInfoUsa.h"
#include "Updates/UpdateInfoJpn.h"
#include "Updates/UpdateInfoKor.h"
#include "Utility.h"
#include "Debug.h"

#define MODE_EXIT 0
#define MODE_TITLES_CHECK 1
#define MODE_TITLES_DUMP 2 // needs am
#define MODE_TITLES_DOWNGRADE 3 // needs am
int mode = MODE_EXIT;
Debug *debug;

void _gfxInit() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
}

void appExit() {
    amExit();
    cfguExit();
    Utility::sdmcArchiveExit();
    fsExit();
    gfxExit();
    hidExit();
    aptExit();
    srvExit();
    delete debug;
}

int quit() {
    printf("press (A) to exit...\n");
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

int checkMode() {
    while (aptMainLoop()) {
        hidScanInput();
        switch (hidKeysDown()) {
            case KEY_X:
                return MODE_TITLES_DUMP;
            case KEY_Y:
                return MODE_TITLES_DOWNGRADE;
            case KEY_A:
                return MODE_TITLES_CHECK;
            case KEY_B:
                return MODE_EXIT;
            default:
                break;
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
        case 5: // KOR
            // Not support KOR N3DS
            if (model == 2 || model == 4) {
                return NULL;
            }
            return (UpdateInfo *) new UpdateInfoKor(model);
        default:
            return NULL;
    }
    return NULL;
}

std::vector<TitleInfo> listTitles() {
    debug->write("\n##### NAND TITLES #####\n");
    std::vector<TitleInfo> titles = Utility::getTitles();
    debug->write("Found %i titles:\n", titles.size());
    for (std::vector<TitleInfo>::iterator it = titles.begin(); it != titles.end(); ++it) {
        debug->write("  id: %016llx - ver: %i\n", it->titleID, it->version);
    }
    debug->write("\n##### NAND TITLES #####\n");
    return titles;
}

void downgrade() {

    debug->print("\nInit -> ");
    std::vector<TitleInfo> titlesToInstall;
    std::vector<TitleInfo> titlesInstalled = listTitles();
    debug->printg("GOOD\n");

    if (mode == MODE_TITLES_DUMP) {
        quit();
    }

    // get system info
    debug->print("Check device -> ");
    Utility::SysInfo *sysInfo = Utility::getSysInfo();
    if (sysInfo == NULL) {
        debug->printr("FAIL\n");
        debug->printr("Can't get system information...\n");
        quit();
    }
    debug->printg("%s %s\n",
                  CFGU_MODEL_TABLE[(int) sysInfo->model],
                  CFGU_REGION_TABLE[(int) sysInfo->region]);

    // find update information based on device model/region
    debug->print("Check update info -> ");
    UpdateInfo *update = getUpdateInfo(sysInfo->model, sysInfo->region);
    if (update == NULL) {
        debug->printr("FAIL\n");
        debug->printr("Can't find update config for your system...\n");
        quit();
    }
    debug->printg("%s %s\n",
                  update->model.c_str(), update->region.c_str());

    // check md5/add files to update list
    debug->print("\nCheck update integrity...\n\n");
    for (std::vector<UpdateItem>::iterator it = update->items.begin(); it != update->items.end(); ++it) {
        if (mode == MODE_TITLES_DOWNGRADE) {
            TitleInfo title = Utility::getTitleInfo(it->getPath());
            if (title.titleID < 1) {
                debug->printr("Can't get cia information (hax didn't succeed?)\n");
                quit();
            }
            int res = Utility::cmp(titlesInstalled, title.titleID, title.version);
            if (res != 0) {
                debug->print("MD5 -> id: %016llx - ver: %i -> ", title.titleID, title.version);
                if (Utility::checkMD5(it->getPath().c_str(), it->getMD5().c_str()) != 0) {
                    debug->printr("FAIL\n");
                    quit();
                }
                debug->printg("GOOD\n");
                title.deleteNeeded = res < 0;
                titlesToInstall.push_back(title);
            }
        } else {
            debug->print("MD5 -> %s -> ", it->getPath().c_str());
            if (Utility::checkMD5(it->getPath().c_str(), it->getMD5().c_str()) != 0) {
                debug->printr("FAIL\n");
                quit();
            }
            debug->printg("GOOD\n");
        }
    }
    update->items.clear();

    debug->printg("\n\n-> DOWNGRADE FILES OK <-\n");
    if (mode == MODE_TITLES_CHECK) {
        quit();
    }

    std::sort(titlesToInstall.begin(), titlesToInstall.end(), Utility::sortTitles);

    // downgrade !
    debug->printr("\n-> DOWNGRADING <-\n\n");
    for (auto it : titlesToInstall) {
        bool nativeFirm = it.titleID == 0x0004013800000002LL
                          || it.titleID == 0x0004013820000002LL;

        debug->print("%016llx ", it.titleID);
        if (nativeFirm) {
            debug->print("(NFIRM) ");
        }
        debug->print(": v.%i -> v.%i -> ", Utility::version(titlesInstalled, it.titleID), it.version);
        if (it.deleteNeeded) Utility::deleteTitle(it.titleID);
        Utility::installTitle(it.path);
        if (nativeFirm && AM_InstallFirm(it.titleID)) {
            debug->printr("FAIL... try again\n");
            if (AM_InstallFirm(it.titleID)) {
                debug->printr("FAIL\n");
                debug->printr("You should be able to use recovery to fix...\n");
                quit();
            }
        }
        debug->printg("DONE\n");
    }
    free(sysInfo);
    free(update);

    debug->print("\n\nDowngrade completed. Trying to reboot in 10 sec...\n");
    debug->print("PowerOff your device if it doesn't...\n");
    svcSleepThread(10000000000LL);
    debug->print("Trying to reboot...\n");
    while (aptInit() != 0) { };
    aptOpenSession();
    while (APT_HardwareResetAsync() != 0) { };
    aptCloseSession();
}

int main(int argc, char *argv[]) {

    _gfxInit();
    debug = new Debug();

    debug->print("\nSafeSysUpdater @ Cpasjuste\n\n");
    printf("Press (X) to dump titles list...\n");
    printf("Press (Y) to downgrade...\n");
    printf("Press (A) to check update files...\n");
    printf("Press (B) to exit...\n");

    mode = checkMode();
    if (mode == MODE_EXIT) {
        quit();
    }

    consoleClear();
    printf("HAX INIT...\n");

    if (mode > MODE_TITLES_CHECK) { // needs AM
        if (Utility::getAMu() != 0) {
            debug->printr("FAIL\n");
            debug->printr("Can't get am:u service ... try again :x\n");
            quit();
        }
        debug->printg("HAX SUCCESS\n");
    }

    // late init
    srvInit();
    fsInit();
    Utility::sdmcArchiveInit();
    cfguInit();
    hidInit();
    amInit();

    downgrade();

    return 0;
}
