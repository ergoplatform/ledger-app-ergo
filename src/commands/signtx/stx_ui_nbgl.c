#ifdef HAVE_NBGL

#include <os.h>
#include <nbgl_use_case.h>
#include <string.h>
#include <glyphs.h>
#include <format.h>
#include <base58.h>

#include "stx_ui.h"
#include "stx_ui_common.h"
#include "stx_response.h"

#include "../../context.h"
#include "../../common/bip32_ext.h"
#include "../../common/macros_ext.h"
#include "../../common/safeint.h"
#include "../../helpers/response.h"
#include "../../sw.h"
#include "../../ergo/address.h"
#include "../../ui/ui_bip32_path.h"
#include "../../ui/ui_application_id.h"
#include "../../ui/ui_menu.h"
#include "../../ui/ui_main.h"
#include "../../ui/display.h"

static NOINLINE void ui_stx_operation_approve_action(bool approved) {
    set_flow_response(approved);
}

static NOINLINE void quit_callback(void) {
    set_flow_response(true);
}

bool ui_stx_add_operation_approve_screens(sign_transaction_ui_aprove_ctx_t* ctx,
                                          uint8_t* screen,
                                          uint32_t app_access_token,
                                          bool is_known_application,
                                          sign_transaction_ctx_t* sign_tx) {
    if (MAX_NUMBER_OF_SCREENS - *screen < 3) return false;

    int n_pairs = 0;

    if (!is_known_application && app_access_token != 0) {
        pairs_global[n_pairs++] = ui_application_id_screen(app_access_token, ctx->app_token);
    }

    ctx->app_token_value = app_access_token;
    ctx->sign_tx_context = sign_tx;
    ctx->is_known_application = is_known_application;

    pair_list.nbPairs = n_pairs;

    ui_stx_operation_approve_reject(true, ctx);

    return true;
}

bool ui_stx_add_output_screens(sign_transaction_ui_output_confirm_ctx_t* ctx,
                               uint8_t* screen,
                               const sign_transaction_output_info_ctx_t* output,
                               sign_transaction_bip32_path_t* last_approved_change,
                               uint8_t network_id) {
    if (MAX_NUMBER_OF_SCREENS - *screen < 6) return false;

    memset(ctx, 0, sizeof(sign_transaction_ui_output_confirm_ctx_t));
    memset(last_approved_change, 0, sizeof(sign_transaction_bip32_path_t));

    uint8_t info_screen_count = 1;  // Address screen
    if (stx_output_info_type(output) != SIGN_TRANSACTION_OUTPUT_INFO_TYPE_BIP32) {
        uint8_t tokens_count = stx_output_info_used_tokens_count(output);
        info_screen_count += 1 + (2 * tokens_count);  // value screen + tokens (2 for each)
    }

    if (MAX_NUMBER_OF_SCREENS - *screen < 2) return false;

    ctx->network_id = network_id;
    ctx->output = output;
    ctx->last_approved_change = last_approved_change;

    int pair_index = pair_list.nbPairs;

    for (int i = 0; i < info_screen_count; i++) {
        pairs_global[pair_index].item = pair_mem_title[pair_index];
        pairs_global[pair_index].value = pair_mem_text[pair_index];

        ui_stx_display_output_state(i,
                                    pair_mem_title[pair_index],
                                    pair_mem_text[pair_index],
                                    (void*) ctx);
        pair_index++;
    }

    pair_list.nbPairs += info_screen_count;

    explicit_bzero(ctx->last_approved_change, sizeof(sign_transaction_bip32_path_t));

    // store last approved change address
    if (stx_output_info_type(ctx->output) == SIGN_TRANSACTION_OUTPUT_INFO_TYPE_BIP32) {
        memmove(ctx->last_approved_change,
                &ctx->output->bip32_path,
                sizeof(sign_transaction_bip32_path_t));
    }
    res_ok();

    return true;
}

bool ui_stx_add_transaction_screens(sign_transaction_ui_sign_confirm_ctx_t* ctx,
                                    uint8_t* screen,
                                    const sign_transaction_amounts_ctx_t* amounts,
                                    uint8_t op_screen_count,
                                    ui_sign_transaction_operation_show_screen_cb screen_cb,
                                    ui_sign_transaction_operation_send_response_cb response_cb,
                                    void* cb_context) {
    if (MAX_NUMBER_OF_SCREENS - *screen < 6) return false;

    memset(ctx, 0, sizeof(sign_transaction_ui_sign_confirm_ctx_t));

    // p2pk
    sign_transaction_operation_p2pk_ctx_t* base_ctx =
        (sign_transaction_operation_p2pk_ctx_t*) cb_context;
    bool res = ui_bip32_path_screen(
        base_ctx->bip32.path,
        base_ctx->bip32.len,
        base_ctx->ui_approve.bip32_path,
        MEMBER_SIZE(sign_transaction_operation_p2pk_ui_approve_data_ctx_t, bip32_path));

    if (!res) {
        res_deny();
        app_set_current_command(CMD_NONE);
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, quit_callback);
    }

    uint8_t tokens_count = stx_amounts_non_zero_tokens_count(amounts);

    ctx->op_screen_count = op_screen_count;
    ctx->op_screen_cb = screen_cb;
    ctx->op_response_cb = response_cb;
    ctx->op_cb_context = cb_context;
    ctx->amounts = amounts;

    int pairs_count = op_screen_count + 1 + (2 * tokens_count);
    int pair_index = pair_list.nbPairs;

    for (int i = 0; i < pairs_count; i++) {
        pairs_global[pair_index].item = pair_mem_title[pair_index];
        pairs_global[pair_index].value = pair_mem_text[pair_index];

        ui_stx_display_tx_state(i,
                                pair_mem_title[pair_index],
                                pair_mem_text[pair_index],
                                (void*) ctx);

        pair_index++;
    }

    pair_list.nbMaxLinesForValue = 0;
    pair_list.pairs = pairs_global;
    pair_list.nbPairs = pair_index;
    pair_list.startIndex = 0;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                       &pair_list,
                       &C_app_logo_64px,
                       "Start Signing",
                       base_ctx->ui_approve.bip32_path,
                       "Sign transaction",
                       ui_stx_operation_approve_action);
    bool approved = io_ui_process();

    if (approved) {
        ctx->op_response_cb(ctx->op_cb_context);
        app_set_current_command(CMD_NONE);
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, quit_callback);
    } else {
        res_deny();
        app_set_current_command(CMD_NONE);
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, quit_callback);
    }

    io_ui_process();
    ui_menu_main();

    return true;
}

void ui_stx_operation_approve_reject(bool approved, sign_transaction_ui_aprove_ctx_t* ctx) {
    sign_transaction_ctx_t* sign_tx = (sign_transaction_ctx_t*) ctx->sign_tx_context;

    app_set_ui_busy(false);

    if (approved) {
        app_set_connected_app_id(ctx->app_token_value);
        sign_tx->state = SIGN_TRANSACTION_STATE_APPROVED;
        send_response_sign_transaction_session_id(sign_tx->session);
    } else {
        app_set_current_command(CMD_NONE);
        res_deny();

        ui_menu_main();
    }
}

#endif