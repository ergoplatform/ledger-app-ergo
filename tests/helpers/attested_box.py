from ergo_lib_python.chain import Token
from helpers.unsigned_box import UnsignedBox
from helpers.ergo_writer import ErgoWriter

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

    def set_extension(self, extension: bytes):
        first_frame = self.frames[0]
        length = len(first_frame.buffer) + 4
        new_frame = ErgoWriter(length)
        new_frame.write_bytes(first_frame.buffer)
        if len(extension) == 1 and extension[0] == 0:
          new_frame.write_uint32(0)
        else:
            self.extension = extension
            first_frame.extension_length = len(extension)
            new_frame.write_uint32(len(extension))
            
        first_frame.buffer = new_frame.buffer
        self.frames[0] = first_frame
        return self