#include <os.h>
#include <string.h>
#include <format.h>
#include <base58.h>

#include "stx_ui_common.h"

#include "../../ergo/address.h"

uint16_t ui_stx_display_output_state(uint8_t screen, char* title, char* text, void* context) {
    sign_transaction_ui_output_confirm_ctx_t* ctx =
        (sign_transaction_ui_output_confirm_ctx_t*) context;
    uint8_t title_len = MEMBER_SIZE(sign_transaction_ui_output_confirm_ctx_t, title);
    uint8_t text_len = MEMBER_SIZE(sign_transaction_ui_output_confirm_ctx_t, text);
    memset(title, 0, title_len);
    memset(text, 0, text_len);

    switch (screen) {
        case 0:  // Output Address Info
            return output_info_print_address(ctx->output,
                                             ctx->network_id,
                                             title,
                                             title_len,
                                             text,
                                             text_len);
        case 1: {  // Output Value
            strncpy(title, "Output Value", title_len);
            if (!format_erg_amount(ctx->output->value, text, text_len)) {
                return SW_BUFFER_ERROR;
            }
            break;
        }
        default: {        // Tokens
            screen -= 2;  // Decrease index for info screens
            uint8_t token_idx = stx_output_info_used_token_index(ctx->output, screen / 2);
            if (!IS_ELEMENT_FOUND(token_idx)) {  // error. bad index state
                return SW_BAD_TOKEN_INDEX;
            }
            if (screen % 2 == 0) {  // Token ID
                snprintf(title, title_len, "Token [%d]", (int) (screen / 2) + 1);
                if (!format_hex_id(ctx->output->tokens_table->tokens[token_idx],
                                   ERGO_ID_LEN,
                                   text,
                                   text_len)) {
                    return SW_ADDRESS_FORMATTING_FAILED;
                }
            } else {  // Token Value
                snprintf(title, title_len, "Token [%d] Value", (int) (screen / 2) + 1);
                format_u64(text, text_len, ctx->output->tokens[token_idx]);
            }
            break;
        }
    }
    return SW_OK;
}

// Callback for TX UI rendering
uint16_t ui_stx_display_tx_state(uint8_t screen, char* title, char* text, void* context) {
    sign_transaction_ui_sign_confirm_ctx_t* ctx = (sign_transaction_ui_sign_confirm_ctx_t*) context;
    uint8_t title_len = MEMBER_SIZE(sign_transaction_ui_sign_confirm_ctx_t, title);
    uint8_t text_len = MEMBER_SIZE(sign_transaction_ui_sign_confirm_ctx_t, text);
    memset(title, 0, title_len);
    memset(text, 0, text_len);

    if (screen < ctx->op_screen_count) {  // Showing operation screen
        return ctx->op_screen_cb(screen, title, title_len, text, text_len, ctx->op_cb_context);
    }
    screen -= ctx->op_screen_count;
    switch (screen) {
        case 0: {  // TX Value
            strncpy(title, "Transaction Amount", title_len);
            if (!format_erg_amount(ctx->amounts->value, text, text_len)) {
                return SW_BUFFER_ERROR;
            }
            break;
        }
        case 1: {  // TX Fee
            strncpy(title, "Transaction Fee", title_len);
            if (!format_erg_amount(ctx->amounts->fee, text, text_len)) {
                return SW_BUFFER_ERROR;
            }
            break;
        }
        default: {        // Tokens
            screen -= 2;  // Decrease index for info screens
            uint8_t token_idx = stx_amounts_non_zero_token_index(ctx->amounts, screen / 2);
            if (!IS_ELEMENT_FOUND(token_idx)) {  // error. bad index state
                return SW_BAD_TOKEN_INDEX;
            }
            if (screen % 2 == 0) {  // Token ID
                snprintf(title, title_len, "Token [%d]", (int) (screen / 2) + 1);
                if (!format_hex_id(ctx->amounts->tokens_table.tokens[token_idx],
                                   ERGO_ID_LEN,
                                   text,
                                   text_len)) {
                    return SW_ADDRESS_FORMATTING_FAILED;
                }
            } else {  // Token Value
                snprintf(title, title_len, "Token [%d] Value", (int) (screen / 2) + 1);
                int64_t value = ctx->amounts->tokens[token_idx];
                if (value < 0) {  // output > inputs
                    STRING_ADD_STATIC_TEXT(text, text_len, "Minting: ");
                    format_u64(text, text_len, -value);
                } else {  // inputs > outputs
                    STRING_ADD_STATIC_TEXT(text, text_len, "Burning: ");
                    format_u64(text, text_len, value);
                }
            }
            break;
        }
    }
    return SW_OK;
}