#ifdef HAVE_NBGL

#include <os.h>
#include <nbgl_use_case.h>
#include <string.h>
#include <glyphs.h>

#include "epk_ui.h"
#include "epk_response.h"
#include "../../context.h"
#include "../../sw.h"
#include "../../common/bip32_ext.h"
#include "../../common/macros_ext.h"
#include "../../helpers/response.h"
#include "../../ui/ui_bip32_path.h"
#include "../../ui/ui_application_id.h"
#include "../../ui/ui_approve_reject.h"
#include "../../ui/ui_menu.h"
#include "../../ui/ui_main.h"
#include "../../ui/display.h"

void ui_display_account_confirm(bool approved) {
    set_flow_response(approved);
}

#define PK_APPID_SIZE MAX_BIP32_STRING_LEN + APPLICATION_ID_STR_LEN + 13 + 1
char pk_appid[PK_APPID_SIZE];

int ui_display_account(extended_public_key_ctx_t* ctx,
                       uint32_t app_access_token,
                       uint32_t* bip32_path,
                       uint8_t bip32_path_len,
                       uint8_t raw_pub_key[static PUBLIC_KEY_LEN],
                       uint8_t chain_code[static CHAIN_CODE_LEN]) {
    if (!bip32_path_validate(bip32_path,
                             bip32_path_len,
                             BIP32_HARDENED(44),
                             BIP32_HARDENED(BIP32_ERGO_COIN),
                             BIP32_PATH_VALIDATE_ACCOUNT_GE3)) {
        return res_error(SW_BIP32_BAD_PATH);
    }

    if (!ui_bip32_path_screen(bip32_path,
                              bip32_path_len,
                              ctx->bip32_path,
                              MEMBER_SIZE(derive_address_ctx_t, bip32_path))) {
        return res_error(SW_BIP32_BAD_PATH);
    }

    int bip32_str_len = strlen(ctx->bip32_path);

    memset(pk_appid, 0, PK_APPID_SIZE);
    strncpy(pk_appid, ctx->bip32_path, bip32_str_len);
    if (app_access_token != 0) {
        pk_appid[bip32_str_len] = '\n';
        snprintf(*(&pk_appid) + bip32_str_len + 1,
                 APPLICATION_ID_STR_LEN + 13,
                 "Application: 0x%08x",
                 app_access_token);
    }

    ctx->app_token_value = app_access_token;
    memmove(ctx->raw_public_key, raw_pub_key, PUBLIC_KEY_LEN);
    memmove(ctx->chain_code, chain_code, CHAIN_CODE_LEN);

    nbgl_useCaseChoice(&WARNING_ICON,
                       "Ext PubKey Export",
                       pk_appid,
                       "Confirm",
                       "Cancel",
                       ui_display_account_confirm);
    bool approved = io_ui_process();

    if (approved) {
        app_set_connected_app_id(ctx->app_token_value);
        send_response_extended_pubkey(ctx->raw_public_key, ctx->chain_code);
        explicit_bzero(ctx, sizeof(extended_public_key_ctx_t));
    } else {
        explicit_bzero(ctx, sizeof(extended_public_key_ctx_t));
        res_deny();
    }

    app_set_current_command(CMD_NONE);

    ui_menu_main();

    return 0;
}

#endif