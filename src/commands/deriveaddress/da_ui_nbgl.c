#ifdef HAVE_NBGL

#include "da_ui.h"
#include <os.h>
#include <ux.h>
#include <glyphs.h>
#include <base58.h>
#include <ux.h>
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

    if (app_access_token != 0) {
        // TO-DO
        // ui_add_screen(ui_application_id_screen(app_access_token, ctx->app_id), &screen);
    }

    if (send) {
        // Confirm Send Address
        nbgl_useCaseAddressReview(ctx->address,
                                  NULL,
                                  &WHEEL_ICON,
                                  "Confirm Send Address",
                                  "",
                                  ui_display_address_confirm);
    } else {
        // Confirm Address
        nbgl_useCaseAddressReview(ctx->address,
                                  NULL,
                                  &INFO_I_ICON,
                                  "Confirm Address",
                                  "",
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