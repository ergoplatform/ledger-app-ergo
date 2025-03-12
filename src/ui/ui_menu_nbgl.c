#ifdef HAVE_NBGL

#include "ui_menu.h"
#include <ux.h>
#include <nbgl_use_case.h>
#include <os.h>
#include <context.h>

#define APPTAGLINE   "This app enables signing\ntransactions on the Ergo\nnetwork."
#define APPCOPYRIGHT "Ergo App (c) 2024"

// info definition
#define SETTING_INFO_NB 2
static const char* const INFO_TYPES[SETTING_INFO_NB] = {"Version", "Copyright"};
static const char* const INFO_CONTENTS[SETTING_INFO_NB] = {APPVERSION, APPCOPYRIGHT};
static const nbgl_contentInfoList_t infoList = {
    .nbInfos = SETTING_INFO_NB,
    .infoTypes = INFO_TYPES,
    .infoContents = INFO_CONTENTS,
};

// settings switches definitions
enum { BLIND_SIGNING_SWITCH_TOKEN = FIRST_USER_TOKEN };
enum { BLIND_SIGNING_SWITCH_ID = 0, SETTINGS_SWITCHES_NB };

static nbgl_contentSwitch_t switches[SETTINGS_SWITCHES_NB] = {0};

static void controls_callback(int token, uint8_t index, int page);

// settings definition
#define SETTING_CONTENTS_NB 1
static const nbgl_content_t contents[SETTING_CONTENTS_NB] = {
    {.type = SWITCHES_LIST,
     .content.switchesList.nbSwitches = SETTINGS_SWITCHES_NB,
     .content.switchesList.switches = switches,
     .contentActionCallback = controls_callback}};

static const nbgl_genericContents_t settingContents = {.callbackCallNeeded = false,
                                                       .contentsList = contents,
                                                       .nbContents = SETTING_CONTENTS_NB};

void app_quit(void) {
    // exit app here
    os_sched_exit(-1);
}

static void controls_callback(int token, uint8_t index, int page) {
    UNUSED(index);

    uint8_t switch_value;
    if (token == BLIND_SIGNING_SWITCH_TOKEN) {
        // toggle the switch value
        switch_value = !N_storage.blind_signing_enabled;
        switches[BLIND_SIGNING_SWITCH_ID].initState = (nbgl_state_t) switch_value;
        // store the new setting value in NVM
        nvm_write((void*) &N_storage.blind_signing_enabled, &switch_value, 1);
    }
}

void ui_menu_main() {
    switches[BLIND_SIGNING_SWITCH_ID].initState = (nbgl_state_t) N_storage.blind_signing_enabled;
    switches[BLIND_SIGNING_SWITCH_ID].text = "Blind signing";
    switches[BLIND_SIGNING_SWITCH_ID].subText = "Enable transaction blind\nsigning.";
    switches[BLIND_SIGNING_SWITCH_ID].token = BLIND_SIGNING_SWITCH_TOKEN;
#ifdef HAVE_PIEZO_SOUND
    switches[BLIND_SIGNING_SWITCH_ID].tuneId = TUNE_TAP_CASUAL;
#endif

    nbgl_useCaseHomeAndSettings(APPNAME,
                                &C_app_logo_64px,
                                APPTAGLINE,
                                INIT_HOME_PAGE,
                                &settingContents,
                                &infoList,
                                NULL,
                                app_quit);
}

void ui_menu_about() {
}

#endif