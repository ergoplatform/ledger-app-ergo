from ragger.bip import pack_derivation_path

class ErgoWriter:
    def __init__(self, max_apdu_size: int):
        self.index = 0
        self.buffer = bytearray(max_apdu_size)

    def write_byte(self, val: int):
        self.buffer[self.index] = val.to_bytes(1, byteorder="big")[0]
        self.index += 1
        return self

    def write_path(self, val: str):
        packed_data = pack_derivation_path(val)
        size = len(packed_data)
        self.buffer[self.index:self.index + size] = packed_data
        self.index += size
        return self

    def write_auth_token(self, token: int | None = None):
        if(token == None):
            return self

        data = token.to_bytes(4, byteorder="big")

        size = len(data)
        self.buffer[self.index:self.index + size] = data
        self.index += size

        return self
    
    def write_hex(self, val: str):
        data = bytearray.fromhex(val)

        size = len(data)
        self.buffer[self.index:self.index + size] = data
        self.index += size

        return self
    
    def write_uint16(self, val: int):
        data = val.to_bytes(2, byteorder="big")

        size = len(data)
        self.buffer[self.index:self.index + size] = data
        self.index += size

        return self
    
    def write_uint32(self, val: int):
        data = val.to_bytes(4, byteorder="big")

        size = len(data)
        self.buffer[self.index:self.index + size] = data
        self.index += size

        return self
    
    def write_uint64(self, val: int):
        data = val.to_bytes(8, byteorder="big")

        size = len(data)
        self.buffer[self.index:self.index + size] = data
        self.index += size

        return self
    
    def write_bytes(self, val: bytes):
        size = len(val)
        self.buffer[self.index:self.index + size] = val
        self.index += size

        return self

    
    def get_position(self) -> int:
        return self.index
    
    def get_buffer(self) -> bytes:
        return self.buffer[0:self.index]