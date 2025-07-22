#pragma once

/**
 * Instruction class of the Ergo application.
 */
#define CLA 0xE0

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION || DEBUG.
 */
#define APPVERSION_LEN 4

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Bip32 coin type of Ergo token
 */
#define BIP32_ERGO_COIN 429

/**
 * Number of fraction digits in ERG token
 */
#define ERGO_ERG_FRACTION_DIGIT_COUNT 9

/**
 * Length of hashed ids in Ergo.
 */
#define ERGO_ID_LEN 32

/**
 * Maximum number of tokens in TX.
 */
#if defined(TARGET_NANOS2)
    #define TOKEN_MAX_COUNT 124
#elif defined(TARGET_NANOX)
    #define TOKEN_MAX_COUNT 100
#else
    #define TOKEN_MAX_COUNT 114
#endif

/**
 * Length of Session Key.
 */
#define SESSION_KEY_LEN 16

/**
 * Length of Chain Code.
 */
#define CHAIN_CODE_LEN 32

/**
 * Length of Public Key.
 */
#define PUBLIC_KEY_LEN 65

/**
 * Length of Compressed Public Key.
 */
#define COMPRESSED_PUBLIC_KEY_LEN 33

/**
 * Length of Private Key.
 */
#define PRIVATE_KEY_LEN 32

/**
 * Length of Extended Public Key.
 */
#define EXTENDED_PUBLIC_KEY_LEN (COMPRESSED_PUBLIC_KEY_LEN + CHAIN_CODE_LEN)

/**
 * Length of Input Frame Signature.
 */
#define INPUT_FRAME_SIGNATURE_LEN 16

/**
 * Length of the secp265k1 schnorr signature
 */
#define ERGO_SIGNATURE_LEN 56

/**
 * Max number of screens
 */
#define MAX_NUMBER_OF_SCREENS 16

/**
 * Max length of TX data part
 */
#define MAX_TX_DATA_PART_LEN 32768

/**
 * Max length of TX data chunk
 */
#define MAX_DATA_CHUNK_LEN 255

/**
 * Max length of BIP32 path string
 */
#define MAX_BIP32_STRING_LEN 60