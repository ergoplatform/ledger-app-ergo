#ifdef HAVE_NBGL

#include <os.h>
#include <glyphs.h>
#include <base58.h>
#include <nbgl_use_case.h>

#include "da_ui.h"
#include "da_response.h"
#include "da_context.h"
#include "../../sw.h"
#include "../../context.h"
#include "../../common/macros_ext.h"
#include "../../ergo/address.h"
#include "../../helpers/response.h"
#include "../../ui/ui_bip32_path.h"
#include "../../ui/ui_application_id.h"
#include "../../ui/ui_approve_reject.h"
#include "../../ui/ui_menu.h"
#include "../../ui/ui_main.h"
#include "../../ui/display.h"

#define PK_APPID_ADDR_SIZE \
    MAX_BIP32_PATH + APPLICATION_ID_STR_LEN + 13 + 1 + P2PK_ADDRESS_STRING_MAX_LEN + 9 + 1
char pk_appid_addr[PK_APPID_ADDR_SIZE];

void ui_display_address_confirm(bool approved) {
    set_flow_response(approved);
}

int ui_display_address(derive_address_ctx_t* ctx,
                       bool send,
                       uint32_t app_access_token,
                       uint32_t* bip32_path,
                       uint8_t bip32_path_len,
                       uint8_t raw_address[static P2PK_ADDRESS_LEN]) {
    if (!bip32_path_validate(bip32_path,
                             bip32_path_len,
                             BIP32_HARDENED(44),
                             BIP32_HARDENED(BIP32_ERGO_COIN),
                             BIP32_PATH_VALIDATE_ADDRESS_GE5)) {
        return send_error(SW_BIP32_BAD_PATH);
    }

    ctx->app_token_value = app_access_token;
    ctx->send = send;

    memset(ctx->address, 0, MEMBER_SIZE(derive_address_ctx_t, address));
    if (!send) {
        int result = base58_encode(raw_address,
                                   P2PK_ADDRESS_LEN,
                                   ctx->address,
                                   MEMBER_SIZE(derive_address_ctx_t, address));

        if (result == -1 || result >= P2PK_ADDRESS_STRING_MAX_LEN) {
            return send_error(SW_ADDRESS_FORMATTING_FAILED);
        }
    }

    if (!ui_bip32_path_screen(bip32_path,
                              bip32_path_len,
                              ctx->bip32_path,
                              MEMBER_SIZE(derive_address_ctx_t, bip32_path))) {
        return res_error(SW_BIP32_BAD_PATH);
    }

    memset(pk_appid_addr, 0, PK_APPID_ADDR_SIZE);
    strncpy(pk_appid_addr, ctx->bip32_path, MAX_BIP32_PATH);
    int offset = 0;
    if (app_access_token != 0) {
        pk_appid_addr[MAX_BIP32_PATH] = '\n';
        offset += APPLICATION_ID_STR_LEN + 13;
        snprintf(*(&pk_appid_addr) + MAX_BIP32_PATH + 1,
                 APPLICATION_ID_STR_LEN + 13,
                 "Application: 0x%08x",
                 app_access_token);
    }

    if (!send) {
        pk_appid_addr[MAX_BIP32_PATH + offset] = '\n';
        strncpy(*(&pk_appid_addr) + MAX_BIP32_PATH + offset + 1, "Address: ", 9);
        strncpy(*(&pk_appid_addr) + MAX_BIP32_PATH + offset + 1 + 9, ctx->address, MEMBER_SIZE(derive_address_ctx_t, address));
    }

    if (send) {
        // Confirm Send Address
        nbgl_useCaseChoice(&WHEEL_ICON,
                           "Address Export",
                           pk_appid_addr,
                           "Confirm",
                           "Cancel",
                           ui_display_address_confirm);
    } else {
        // Confirm Address
        nbgl_useCaseChoice(&INFO_I_ICON,
                           "Confirm Address",
                           pk_appid_addr,
                           "Confirm",
                           "Cancel",
                           ui_display_address_confirm);
    }

    memmove(ctx->raw_address, raw_address, P2PK_ADDRESS_LEN);

    bool approved = io_ui_process();

    if (approved) {
        app_set_connected_app_id(ctx->app_token_value);
        if (ctx->send) {
            send_response_address(ctx->raw_address);
        } else {
            app_set_current_command(CMD_NONE);
            res_ok();
        }
    } else {
        app_set_current_command(CMD_NONE);
        res_deny();
    }

    ui_menu_main();

    return 0;
}

#endif