from ergo_lib_python.chain import NetworkPrefix, Address

from helpers.data import MINER_FEE_TREE

def get_miner_address(network: NetworkPrefix):
    return Address(MINER_FEE_TREE, network)