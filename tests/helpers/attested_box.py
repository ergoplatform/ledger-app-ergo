from ergo_lib_python.chain import Token
from helpers.unsigned_box import UnsignedBox

class AttestedBoxFrame:
    def __init__(self, box_id: str, count: int, index: int, amount: int, tokens: list[Token], attestation: str, extension_length: int | None, buffer: bytes):
        self.box_id = box_id
        self.count = count
        self.index = index
        self.amount = amount
        self.tokens = tokens
        self.attestation = attestation
        self.extension_length: int | None = extension_length
        self.buffer: bytes = buffer

class AttestedBox:
    def __init__(self, box: UnsignedBox, frames: list[AttestedBoxFrame]):
        self.box = box
        self.frames = frames