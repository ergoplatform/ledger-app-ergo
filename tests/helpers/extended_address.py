from ergo_lib_python.chain import NetworkPrefix, Address
from ergo_lib_python.wallet import DerivationPath
from typing import List

class ExtendedAddress:
    def __init__(self, network: NetworkPrefix, address: str, path:List[int]):
        self.network = network
        self.address = Address(address, self.network)
        self.path = DerivationPath(path[0], [path[1]])
        self.acc_index = path[0]
        self.addr_index = path[1]