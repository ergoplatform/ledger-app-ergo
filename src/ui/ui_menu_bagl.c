/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#ifdef HAVE_BAGL

#include <os.h>
#include <ux.h>
#include <glyphs.h>

#include "ui_menu.h"
#include "ui_main.h"

UX_STEP_NOCB(ux_menu_ready_step, pnn, {&C_app_logo_16px, APPNAME, "is ready"});
UX_STEP_CB(ux_menu_settings_step, pb, ui_menu_settings(), {&C_icon_coggle, "Settings"});
UX_STEP_CB(ux_menu_about_step, pb, ui_menu_about(), {&C_icon_certificate, "About"});
UX_STEP_CB(ux_menu_exit_step, pb, os_sched_exit(-1), {&C_icon_dashboard_x, "Quit"});

void ui_menu_main() {
    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }

    uint8_t screen = 0;
    ui_add_screen(&ux_menu_ready_step, &screen);
    ui_add_screen(&ux_menu_settings_step, &screen);
    ui_add_screen(&ux_menu_about_step, &screen);
    ui_add_screen(&ux_menu_exit_step, &screen);
    ui_display_screens(&screen);

    app_set_ui_busy(false);
}

UX_STEP_NOCB(ux_menu_info_step, bn, {APPNAME " App", "(c) 2024 Ergo"});
UX_STEP_NOCB(ux_menu_version_step, bn, {"Version", APPVERSION});
UX_STEP_CB(ux_menu_back_step, pb, ui_menu_main(), {&C_icon_back, "Back"});

void ui_menu_about() {
    uint8_t screen = 0;
    ui_add_screen(&ux_menu_info_step, &screen);
    ui_add_screen(&ux_menu_version_step, &screen);
    ui_add_screen(&ux_menu_back_step, &screen);
    ui_display_screens(&screen);

    app_set_ui_busy(false);
}

static void toggle_blind_signing() {
    uint8_t switch_value = !N_storage.blind_signing_enabled;
    nvm_write((void *) &N_storage.blind_signing_enabled, &switch_value, 1);
    ui_menu_settings();
}

UX_STEP_CB(ux_menu_bs_enabled_step,
           bnnn,
           toggle_blind_signing(),
           {"Blind signing", "Enable transaction", "blind signing.", "Enabled"});
UX_STEP_CB(ux_menu_bs_disabled_step,
           bnnn,
           toggle_blind_signing(),
           {"Blind signing", "Enable transaction", "blind signing.", "Disabled"});

void ui_menu_settings() {
    uint8_t screen = 0;
    if (N_storage.blind_signing_enabled) {
        ui_add_screen(&ux_menu_bs_enabled_step, &screen);
    } else {
        ui_add_screen(&ux_menu_bs_disabled_step, &screen);
    }
    ui_add_screen(&ux_menu_back_step, &screen);
    ui_display_screens(&screen);

    app_set_ui_busy(false);
}

#endif