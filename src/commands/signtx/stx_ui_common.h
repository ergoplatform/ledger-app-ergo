#pragma once

#include <os.h>
#include <string.h>
#include <format.h>
#include <base58.h>

#include "../../ergo/address.h"

#include "stx_context.h"

#define ERGO_ID_UI_CHARACTERS_HALF 26
#define STRING_ADD_STATIC_TEXT(str, slen, text) \
    strncpy(str, text, slen);                   \
    slen -= sizeof(text) - 1;                   \
    str += sizeof(text) - 1

static inline void id_string_remove_middle(char* str, size_t len) {
    if (len <= 2 * ERGO_ID_UI_CHARACTERS_HALF + 3) return;
    str[ERGO_ID_UI_CHARACTERS_HALF] = str[ERGO_ID_UI_CHARACTERS_HALF + 1] =
        str[ERGO_ID_UI_CHARACTERS_HALF + 2] = '.';
    memmove(str + ERGO_ID_UI_CHARACTERS_HALF + 3,
            str + len - ERGO_ID_UI_CHARACTERS_HALF,
            ERGO_ID_UI_CHARACTERS_HALF);
    str[2 * ERGO_ID_UI_CHARACTERS_HALF + 3] = '\0';
}

static inline bool format_hex_id(const uint8_t* id, size_t id_len, char* out, size_t out_len) {
    int len = format_hex(id, id_len, out, out_len);
    if (len <= 0) return false;
    id_string_remove_middle(out, len - 1);
    return true;
}

static inline bool format_b58_id(const uint8_t* id, size_t id_len, char* out, size_t out_len) {
    int len = base58_encode(id, id_len, out, out_len);
    if (len <= 0) return false;
    return true;
}

static inline bool format_erg_amount(uint64_t amount, char* out, size_t out_len) {
    if (!format_fpu64(out, out_len, amount, ERGO_ERG_FRACTION_DIGIT_COUNT)) {
        return false;
    }
    size_t out_bytes = strlen(out);
    if (out_len < 5 || out_bytes > out_len - 5) return false;
    out_len -= out_bytes;
    out += out_bytes;
    STRING_ADD_STATIC_TEXT(out, out_len, " ERG");
    return true;
}

static inline uint16_t output_info_print_address(const sign_transaction_output_info_ctx_t* ctx,
                                                 uint8_t network_id,
                                                 char* title,
                                                 uint8_t title_len,
                                                 char* address,
                                                 uint8_t address_len) {
    if (!stx_output_info_is_finished(ctx)) return SW_BAD_STATE;
    switch (stx_output_info_type(ctx)) {
        case SIGN_TRANSACTION_OUTPUT_INFO_TYPE_BIP32: {
            strncpy(title, "Change", title_len);
            if (!bip32_path_format(ctx->bip32_path.path,
                                   ctx->bip32_path.len,
                                   address,
                                   address_len)) {
                return SW_BIP32_FORMATTING_FAILED;
            }
            break;
        }
        case SIGN_TRANSACTION_OUTPUT_INFO_TYPE_ADDRESS: {
            uint8_t raw_address[P2PK_ADDRESS_LEN];
            strncpy(title, "Address", title_len);
            if (!ergo_address_from_compressed_pubkey(network_id, ctx->public_key, raw_address)) {
                return SW_ADDRESS_GENERATION_FAILED;
            }
            if (!format_b58_id(raw_address, P2PK_ADDRESS_LEN, address, address_len)) {
                return SW_ADDRESS_FORMATTING_FAILED;
            }
            break;
        }
        case SIGN_TRANSACTION_OUTPUT_INFO_TYPE_SCRIPT:
        case SIGN_TRANSACTION_OUTPUT_INFO_TYPE_SCRIPT_HASH: {
            strncpy(title, "Script Hash", title_len);
            uint8_t raw_address[P2SH_ADDRESS_LEN];
            if (!ergo_address_from_script_hash(network_id, ctx->tree_hash, raw_address)) {
                return SW_ADDRESS_GENERATION_FAILED;
            }
            if (!format_b58_id(raw_address, P2SH_ADDRESS_LEN, address, address_len)) {
                return SW_ADDRESS_FORMATTING_FAILED;
            }
            break;
        }
        case SIGN_TRANSACTION_OUTPUT_INFO_TYPE_MINERS_FEE: {
            strncpy(title, "Fee", title_len);
            strncpy(address, "Miners Fee", address_len);
            break;
        }
        default:
            return SW_BAD_STATE;
    }
    return SW_OK;
}

uint16_t ui_stx_display_output_state(uint8_t screen, char* title, char* text, void* context);

// Callback for TX UI rendering
uint16_t ui_stx_display_tx_state(uint8_t screen, char* title, char* text, void* context);