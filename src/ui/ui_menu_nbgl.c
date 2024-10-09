#ifdef HAVE_NBGL

#include "ui_menu.h"
#include <ux.h>
#include <nbgl_use_case.h>

#define APPTAGLINE   "Ergo app for ledger"
#define APPCOPYRIGHT "Ergo App (c) 2024"

#define SETTING_INFO_NB 2
static const char* const INFO_TYPES[SETTING_INFO_NB] = {"Version", "Copyright"};
static const char* const INFO_CONTENTS[SETTING_INFO_NB] = {APPVERSION, APPCOPYRIGHT};
static const nbgl_contentInfoList_t infoList = {
    .nbInfos = SETTING_INFO_NB,
    .infoTypes = INFO_TYPES,
    .infoContents = INFO_CONTENTS,
};

void app_quit(void) {
    // exit app here
    os_sched_exit(-1);
}

void ui_menu_main() {
    nbgl_useCaseHomeAndSettings(APPNAME,
                                &C_app_logo_64px,
                                APPTAGLINE,
                                INIT_HOME_PAGE,
                                NULL,
                                &infoList,
                                NULL,
                                app_quit);
}

void ui_menu_about() {
}

#endif