from ergo_lib_python.wallet import DerivationPath, ExtPubKey

class Account:
    def __init__(self, pk_hex: str, chain_code_hex: str, path: DerivationPath):
        self.public_key = ExtPubKey(bytes.fromhex(pk_hex), bytes.fromhex(chain_code_hex), path)
        self.path = path