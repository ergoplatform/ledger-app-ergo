from enum import IntEnum
from typing import Generator, List, Optional
from contextlib import contextmanager

from ragger.backend.interface import BackendInterface, RAPDU

from helpers.ergo_writer import ErgoWriter


MAX_APDU_LEN: int = 255

CLA: int = 0xE0

class P1(IntEnum):
    P1_ZERO         = 0x00
    P1_DA_RETURN    = 0x01
    P1_DA_DISPLAY   = 0x02

class P2(IntEnum):
    P2_ZERO             = 0x00
    P2_DA_WITHOUT_TOKEN = 0x01
    P2_DA_WITH_TOKEN    = 0x02

class InsType(IntEnum):
    GET_VERSION = 0x01
    GET_NAME    = 0x02
    DERIVE_ADDR = 0x11

class Errors(IntEnum):
    SW_DENY                       = 0x6985
    SW_WRONG_P1P2                 = 0x6A86
    SW_WRONG_DATA_LENGTH          = 0x6A87
    SW_INS_NOT_SUPPORTED          = 0x6D00
    SW_CLA_NOT_SUPPORTED          = 0x6E00
    SW_BUSY                       = 0xB000
    SW_WRONG_RESPONSE_LENGTH      = 0xB001
    SW_BAD_SESSION_ID             = 0xB002
    SW_WRONG_SUBCOMMAND           = 0xB003
    SW_SCREENS_BUFFER_OVERFLOW    = 0xB004
    SW_BAD_STATE                  = 0xB0FF
    SW_TX_HASH_FAIL               = 0xB006
    SW_SIGNATURE_FAIL             = 0xB008
    SW_BAD_TOKEN_ID               = 0xE001
    SW_BAD_TOKEN_VALUE            = 0xE002
    SW_BAD_CONTEXT_EXTENSION_SIZE = 0xE003
    SW_BAD_DATA_INPUT             = 0xE004
    SW_BAD_BOX_ID                 = 0xE005
    SW_BAD_TOKEN_INDEX            = 0xE006
    SW_BAD_FRAME_INDEX            = 0xE007
    SW_BAD_INPUT_COUNT            = 0xE008
    SW_BAD_OUTPUT_COUNT           = 0xE009
    SW_TOO_MANY_TOKENS            = 0xE00A
    SW_TOO_MANY_INPUTS            = 0xE00B
    SW_TOO_MANY_DATA_INPUTS       = 0xE00C
    SW_TOO_MANY_INPUT_FRAMES      = 0xE00D
    SW_TOO_MANY_OUTPUTS           = 0xE00E
    SW_HASHER_ERROR               = 0xE00F
    SW_BUFFER_ERROR               = 0xE010
    SW_U64_OVERFLOW               = 0xE011
    SW_BIP32_BAD_PATH             = 0xE012
    SW_INTERNAL_CRYPTO_ERROR      = 0xE013
    SW_NOT_ENOUGH_DATA            = 0xE014
    SW_TOO_MUCH_DATA              = 0xE015
    SW_ADDRESS_GENERATION_FAILED  = 0xE016
    SW_SCHNORR_SIGNING_FAILED     = 0xE017
    SW_BAD_FRAME_SIGNATURE        = 0xE018
    SW_BAD_NET_TYPE_VALUE         = 0xE019
    SW_SMALL_CHUNK                = 0xE01A
    SW_BIP32_FORMATTING_FAILED    = 0xE101
    SW_ADDRESS_FORMATTING_FAILED  = 0xE102


def split_message(message: bytes, max_size: int) -> List[bytes]:
    return [message[x:x + max_size] for x in range(0, len(message), max_size)]


class ErgoCommandSender:
    def __init__(self, backend: BackendInterface) -> None:
        self.backend = backend


    def get_version(self) -> RAPDU:
        return self.backend.exchange(cla = CLA,
                                     ins  = InsType.GET_VERSION,
                                     p1   = P1.P1_ZERO,
                                     p2   = P2.P2_ZERO,
                                     data = b"")
    

    def get_name(self) -> RAPDU:
        return self.backend.exchange(cla = CLA,
                                     ins  = InsType.GET_NAME,
                                     p1   = P1.P1_ZERO,
                                     p2   = P2.P2_ZERO,
                                     data = b"")


    @contextmanager
    def derive_address(self, network: int, path: str, token: int | None = None, show: bool = False) -> Generator[None, None, None]:
        writer = ErgoWriter(MAX_APDU_LEN)
        data = writer.write_byte(network).write_path(path).write_auth_token(token).get_buffer()

        with self.backend.exchange_async(cla = CLA,
                                     ins  = InsType.DERIVE_ADDR,
                                     p1   = P1.P1_DA_RETURN if not show else P1.P1_DA_DISPLAY,
                                     p2   = P2.P2_DA_WITHOUT_TOKEN if token == None else P2.P2_DA_WITH_TOKEN,
                                     data = data) as response:
            yield response

    def get_async_response(self) -> Optional[RAPDU]:
        return self.backend.last_async_response