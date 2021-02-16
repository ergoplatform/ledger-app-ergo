/**
 * Signs a 32-byte message with the given secret key.
 *
 * @signature: 56-byte output signature
 * @msg: 32-byte message for signing
 * @sk: 32-byte secret key
 */
void ergo_sign(unsigned char *signature, const unsigned char *msg, const unsigned char *sk);
