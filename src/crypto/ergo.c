#include "os.h"
#include "cx.h"

#define HASH_32_LEN 32
#define HASH_64_LEN 64
#define PUBLICKEY_COMPRESSED_LEN 33
#define PUBLICKEY_UNCOMPRESSED_LEN 65
#define ERGO_SOUNDNESS_BYTES 24

static unsigned char const SECP256K1_G[] = {
  // Gx: 0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798
  0x79, 0xbe, 0x66, 0x7e, 0xf9, 0xdc, 0xbb, 0xac,
  0x55, 0xa0, 0x62, 0x95, 0xce, 0x87, 0x0b, 0x07,
  0x02, 0x9b, 0xfc, 0xdb, 0x2d, 0xce, 0x28, 0xd9,
  0x59, 0xf2, 0x81, 0x5b, 0x16, 0xf8, 0x17, 0x98,
  // Gy:  0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8
  0x48, 0x3a, 0xda, 0x77, 0x26, 0xa3, 0xc4, 0x65,
  0x5d, 0xa4, 0xfb, 0xfc, 0x0e, 0x11, 0x08, 0xa8,
  0xfd, 0x17, 0xb4, 0x48, 0xa6, 0x85, 0x54, 0x19,
  0x9c, 0x47, 0xd0, 0x8f, 0xfb, 0x10, 0xd4, 0xb8 };

static unsigned char const SECP256K1_N[] = {
  // n: 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
  0xba, 0xae, 0xdc, 0xe6, 0xaf, 0x48, 0xa0, 0x3b,
  0xbf, 0xd2, 0x5e, 0x8c, 0xd0, 0x36, 0x41, 0x41 };

uint32_t ergo_sign(unsigned char *signature, const unsigned char *msg, const unsigned char *sk) {
  const unsigned char prefix[7] = {0x01, 0x00, 0x27, 0x10, 0x01, 0x08, 0xcd};
  const unsigned char postfix[4] = {0x73, 0x00, 0x00, 0x21};

  cx_blake2b_t ctx;
  unsigned WIDE char y[HASH_32_LEN];
  unsigned WIDE char c[HASH_32_LEN];
  unsigned WIDE char pk[PUBLICKEY_UNCOMPRESSED_LEN];
  unsigned WIDE char w[PUBLICKEY_UNCOMPRESSED_LEN];

  uint32_t d_len = HASH_32_LEN;
  uint32_t sig_len = ERGO_SOUNDNESS_BYTES + HASH_32_LEN;

  unsigned WIDE char G[PUBLICKEY_UNCOMPRESSED_LEN];
  // base point G
  {
    G[0] = 0x04;
    os_memcpy(G + 1, SECP256K1_G, HASH_64_LEN);
  }

  // ephemeral private key
  cx_rng(y, 32);

  if (cx_math_is_zero(y, d_len) || cx_math_cmp(y, SECP256K1_N, d_len) > 0) {
    sig_len = 0;
    goto CLEAR_LOCALS;
  }

  if (cx_math_is_zero(sk, d_len) || cx_math_cmp(sk, SECP256K1_N, d_len) > 0) {
    sig_len = 0;
    goto CLEAR_LOCALS;
  }

  // pk = G * sk
  {
    os_memcpy(pk, G, sizeof(G));
    cx_ecfp_scalar_mult(CX_CURVE_SECP256K1, pk, sizeof(pk), sk, d_len);
    pk[0] = (pk[PUBLICKEY_UNCOMPRESSED_LEN - 1] & 1) == 1 ? 0x03 : 0x02;
  }

  // w = G * y
  {
    os_memcpy(w, G, sizeof(G));
    cx_ecfp_scalar_mult(CX_CURVE_SECP256K1, w, sizeof(w), y, d_len);
    w[0] = (w[PUBLICKEY_UNCOMPRESSED_LEN - 1] & 1) == 1 ? 0x03 : 0x02;
  }

  // compute commitment c = H(prefix || pk || postfix || w || msg)
  {
    cx_blake2b_init(&ctx, 256);
    cx_hash(&ctx.header, 0, prefix, 7, NULL, 0);
    cx_hash(&ctx.header, 0, pk, PUBLICKEY_COMPRESSED_LEN, NULL, 0);
    cx_hash(&ctx.header, 0, postfix, 4, NULL, 0);
    cx_hash(&ctx.header, 0, w, PUBLICKEY_COMPRESSED_LEN, NULL, 0);
    cx_hash(&ctx.header, 0, msg, HASH_32_LEN, NULL, 0);
    cx_hash(&ctx.header, CX_LAST | CX_NO_REINIT, NULL, 0, signature, HASH_32_LEN);

    // important: we only use the first 24 bytes of the hash output!
    os_memset(c, 0, HASH_32_LEN - ERGO_SOUNDNESS_BYTES);
    os_memcpy(c + HASH_32_LEN - ERGO_SOUNDNESS_BYTES, signature, ERGO_SOUNDNESS_BYTES);

    if (cx_math_is_zero(c, d_len)) {
      sig_len = 0;
      goto CLEAR_LOCALS;
    }
  }

  // z = c * sk + y
  cx_math_multm(c, c, sk, SECP256K1_N, d_len);
  cx_math_addm(signature + ERGO_SOUNDNESS_BYTES, y, c, SECP256K1_N, d_len);

CLEAR_LOCALS:
  os_memset(y, 0, sizeof(y));
  os_memset(c, 0, sizeof(c));
  os_memset(pk, 0, sizeof(pk));
  os_memset(w, 0, sizeof(w));

  return sig_len;
};
