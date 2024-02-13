#pragma once

#include <bip32.h>

#define BIP32_HARDENED_CONSTANT 0x80000000u

#define BIP32_HARDENED(x) (BIP32_HARDENED_CONSTANT + x)

typedef enum {
    BIP32_PATH_VALIDATE_COIN,
    BIP32_PATH_VALIDATE_COIN_GE2_HARD,
    BIP32_PATH_VALIDATE_ACCOUNT_E3,
    BIP32_PATH_VALIDATE_ACCOUNT_GE3,
    BIP32_PATH_VALIDATE_ADDRESS_E5,
    BIP32_PATH_VALIDATE_ADDRESS_GE5
} bip32_path_validation_type_e;

bool bip32_path_validate(const uint32_t *bip32_path,
                         uint8_t bip32_path_len,
                         uint32_t type,
                         uint32_t coin,
                         bip32_path_validation_type_e vtype);