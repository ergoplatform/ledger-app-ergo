from ergo_lib_python.chain import NetworkPrefix
from ergo_lib_python.wallet import DerivationPath

from helpers.extended_address import ExtendedAddress
from helpers.account import Account

NETWORK = NetworkPrefix.Mainnet
AUTH_TOKEN = 0x629f6fbf # generate with random.randint(1, 0xffffffff)

ADDRESS_0 = ExtendedAddress(NETWORK, "9gqBSpseifxnkjRLZUxs5wbJGsvYPG7MLRcBgnKEzFiJoMJaakg", [0, 0])


ACCOUNT = Account(
    "03c24e55008b523ccaf03b6c757f88c4881ef3331a255b76d2e078016c69c3dfd4",
    "8eb29c7897d57aee371bf254be6516e6963e2d9b379d0d626c17a39d1a3bf553",
    DerivationPath.from_str("m/44'/429'/0'"))