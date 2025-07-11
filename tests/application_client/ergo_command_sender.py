from enum import IntEnum
from typing import Generator, List, Optional
from contextlib import contextmanager

from ragger.backend.interface import BackendInterface, RAPDU
from ergo_lib_python.chain import Token, TokenId
import math

from helpers.ergo_writer import ErgoWriter
from helpers.unsigned_box import UnsignedBox
import helpers.core as core
from helpers.attested_box import AttestedBox, AttestedBoxFrame
from helpers.ergo_reader import ErgoReader
from helpers.tx_builder import AttestedTransaction


MAX_APDU_LEN: int       = 255
TOKEN_ENTRY_SIZE: int   = 40

CLA: int = 0xE0

class P1(IntEnum):
    P1_ZERO             = 0x00
    P1_DA_RETURN        = 0x01
    P1_DA_DISPLAY       = 0x02
    P1_PK_WITHOUT_TOKEN = 0x01
    P1_PK_WITH_TOKEN    = 0x02
    # AT
    P1_AT_BOX_START                 = 0x01,
    P1_AT_ADD_ERGO_TREE_CHUNK       = 0x02,
    P1_AT_ADD_TOKENS                = 0x03,
    P1_AT_ADD_REGISTERS_CHUNK       = 0x04,
    P1_AT_GET_ATTESTED_BOX_FRAME    = 0x05
    # STX
    P1_STX_START_SIGNING                            = 0x01,
    P1_STX_START_TRANSACTION                        = 0x10,
    P1_STX_ADD_TOKEN_IDS                            = 0x11,
    P1_STX_ADD_INPUT_BOX_FRAME                      = 0x12,
    P1_STX_ADD_INPUT_BOX_CONTEXT_EXTENSION_CHUNK    = 0x13,
    P1_STX_ADD_DATA_INPUTS                          = 0x14,
    P1_STX_ADD_OUTPUT_BOX_START                     = 0x15,
    P1_STX_ADD_OUTPUT_BOX_ERGO_TREE_CHUNK           = 0x16,
    P1_STX_ADD_OUTPUT_BOX_MINERS_FEE_TREE           = 0x17,
    P1_STX_ADD_OUTPUT_BOX_CHANGE_TREE               = 0x18,
    P1_STX_ADD_OUTPUT_BOX_TOKENS                    = 0x19,
    P1_STX_ADD_OUTPUT_BOX_REGISTERS_CHUNK           = 0x1a,
    P1_STX_CONFIRM_AND_SIGN                         = 0x20

class P2(IntEnum):
    P2_ZERO             = 0x00
    P2_WITHOUT_TOKEN = 0x01
    P2_WITH_TOKEN    = 0x02

class InsType(IntEnum):
    GET_VERSION = 0x01
    GET_NAME    = 0x02
    EXT_PUB_KEY = 0x10
    DERIVE_ADDR = 0x11
    ATTEST_BOX  = 0x20

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
                                     p2   = P2.P2_WITHOUT_TOKEN if token == None else P2.P2_WITH_TOKEN,
                                     data = data) as response:
            yield response

    @contextmanager
    def ext_pub_key(self, path: str, token: int | None = None) -> Generator[None, None, None]:
        writer = ErgoWriter(MAX_APDU_LEN)
        data = writer.write_path(path).write_auth_token(token).get_buffer()

        with self.backend.exchange_async(cla = CLA,
                                     ins  = InsType.EXT_PUB_KEY,
                                     p1   = P1.P1_PK_WITHOUT_TOKEN if token == None else P1.P1_PK_WITH_TOKEN,
                                     p2   = P2.P2_ZERO,
                                     data = data) as response:
            yield response

    
    #
    # ATTEST INPUT
    #
    @contextmanager
    def attest_send_header(self, box: UnsignedBox, token: int | None = None):
        writer = ErgoWriter(MAX_APDU_LEN)
        data = writer.write_hex(box.tx_id).write_uint16(box.index).write_uint64(box.value).write_uint32(len(box.ergo_tree)).write_uint32(box.creation_height).write_byte(len(box.tokens)).write_uint32(len(box.additional_registers)).write_auth_token(token).get_buffer()
        with self.backend.exchange_async(cla = CLA,
                                     ins  = InsType.ATTEST_BOX,
                                     p1   = P1.P1_AT_BOX_START,
                                     p2   = P2.P2_WITHOUT_TOKEN if token == None else P2.P2_WITH_TOKEN,
                                     data = data) as response:
            yield response
    
    def attest_send_ergo_tree(self, ergo_tree: bytes, session_id: int) -> int:
        res = self.backend.exchange(cla = CLA,
                                     ins  = InsType.ATTEST_BOX,
                                     p1   = P1.P1_AT_ADD_ERGO_TREE_CHUNK,
                                     p2   = session_id,
                                     data = ergo_tree)
        return res.data[0] if len(res.data) > 0 else 0
    
    def attest_send_tokens(self, tokens: list[Token], session_id: int) -> int:
        chunks:list[list[Token]] = core.chunk(tokens, math.floor(MAX_APDU_LEN / TOKEN_ENTRY_SIZE))
        results:list[RAPDU] = []

        for chunk in chunks:
            data = ErgoWriter(len(chunk) * TOKEN_ENTRY_SIZE)
            for token in chunk:
                data.write_hex(token.token_id).write_uint64(token.amount)

            results.append(self.backend.exchange(cla = CLA,
                                     ins  = InsType.ATTEST_BOX,
                                     p1   = P1.P1_AT_ADD_TOKENS,
                                     p2   = session_id,
                                     data = data.get_buffer()))
            
        return results[0].data[0] if len(results) > 0 else 0

    def attest_send_registers(self, data: bytes, session_id: int) -> int:
        res = self.backend.exchange(cla = CLA,
                                     ins  = InsType.ATTEST_BOX,
                                     p1   = P1.P1_AT_ADD_REGISTERS_CHUNK,
                                     p2   = session_id,
                                     data = data)
        return res.data[0] if len(res.data) > 0 else 0
    
    def attest_decode_frame_response(self, data: bytes) -> AttestedBoxFrame:
        reader = ErgoReader(data)
        box_id = reader.read_hex(32)
        count = reader.read_byte()
        index = reader.read_byte()
        amount = reader.read_uint64()
        token_count = reader.read_byte()
    

        tokens: list[Token] = []
        for i in range(token_count):
            tokens.append(Token(TokenId(reader.read_slice(32)), reader.read_uint64()))

        attestation = reader.read_hex(16)

        return AttestedBoxFrame(box_id, count, index, amount, tokens, attestation, None, data)


    def attest_get_attested_frames(self, count: int, session_id: int) -> list[AttestedBoxFrame]:
        responses:list[AttestedBoxFrame] = []
        for i in range(count):
            responses.append(self.attest_decode_frame_response(self.backend.exchange(cla = CLA,
                                     ins  = InsType.ATTEST_BOX,
                                     p1   = P1.P1_AT_GET_ATTESTED_BOX_FRAME,
                                     p2   = session_id,
                                     data = i.to_bytes(1, byteorder="big")).data))
            
        return responses

 
    def attest_input(self, box: UnsignedBox, token: int | None = None) -> Generator[AttestedBox | None, None, None]:
        with self.attest_send_header(box, token):
            yield None
        
        session_id = self.get_async_response().data[0]
        frame_count = self.attest_send_ergo_tree(box.ergo_tree, session_id)

        if len(box.tokens) > 0:
            frame_count = self.attest_send_tokens(box.tokens, session_id)

        if len(box.additional_registers) > 0:
            frame_count = self.attest_send_registers(box.additional_registers, session_id)

        yield AttestedBox(box, self.attest_get_attested_frames(frame_count, session_id))

    #
    # SIGN TRANSACTION
    #
    
    def sign_tx(self, tx: AttestedTransaction, sign_path: str, network: int, token: int | None = None) -> Generator[AttestedBox | None, None, None]:
        pass

    def get_async_response(self) -> Optional[RAPDU]:
        return self.backend.last_async_response