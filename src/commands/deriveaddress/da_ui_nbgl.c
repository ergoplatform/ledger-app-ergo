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

char addr_buf[P2PK_ADDRESS_STRING_MAX_LEN];
char app_id_buf[APPLICATION_ID_STR_LEN];

void ui_display_address_confirm(bool approved) {
    set_flow_response(approved);
}

void ui_address_flow_end(void) {
    set_flow_response(true);
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

    int n_pairs = 0;

    pairs_global[n_pairs].item = "Derivation path";
    pairs_global[n_pairs].value = ctx->bip32_path;
    n_pairs++;

    if (app_access_token != 0) {
        pairs_global[n_pairs++] = ui_application_id_screen(app_access_token, app_id_buf);
    }

    if (!send) {
        strncpy(addr_buf, ctx->address, MEMBER_SIZE(derive_address_ctx_t, address));
    }

    pair_list.nbMaxLinesForValue = 0;
    pair_list.nbPairs = n_pairs;
    pair_list.pairs = pairs_global;

    if (send) {
        // Confirm Send Address
        nbgl_useCaseReviewLight(STATUS_TYPE_ADDRESS_VERIFIED,
                                &pair_list,
                                &WHEEL_ICON,
                                "Address Export",
                                NULL,
                                "Confirm address export",
                                ui_display_address_confirm);
    } else {
        // Confirm Address
        nbgl_useCaseAddressReview(addr_buf,
                                  &pair_list,
                                  &C_app_logo_64px,
                                  "Verify Ergo address",
                                  NULL,
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
        nbgl_useCaseReviewStatus(STATUS_TYPE_ADDRESS_VERIFIED, ui_address_flow_end);
    } else {
        app_set_current_command(CMD_NONE);
        res_deny();
        nbgl_useCaseReviewStatus(STATUS_TYPE_ADDRESS_REJECTED, ui_address_flow_end);
    }

    io_ui_process();

    ui_menu_main();

    return 0;
}

#endif