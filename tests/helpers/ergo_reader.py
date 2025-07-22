class ErgoReader:
    def __init__(self, buffer: bytes):
        self.index = 0
        self.buffer = buffer

    def read_hex(self, size:int = 32):
        res = self.buffer[self.index:self.index + size].hex()
        self.index += size
        return res
    
    def read_byte(self):
        res = self.buffer[self.index]
        self.index += 1
        return res
    
    def read_uint64(self):
        size = 8
        res = int.from_bytes(self.buffer[self.index:self.index + size], byteorder='big')
        self.index += size
        return res
    
    def read_slice(self, size: int):
        res = self.buffer[self.index:self.index + size]
        self.index += size
        return res