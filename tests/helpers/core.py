from ergo_lib_python.transaction import UnsignedTransaction, Transaction, UnsignedInput, Input
from ergo_lib_python.chain import Address
from ergo_lib_python.verifier import verify_signature

from helpers.data import ADDRESS_0, NETWORK

def chunk(array: list, size: int) -> list:
    if not array:
        return []
    if len(array) <= size:
        return [array]

    chunks = []
    for i in range(0, len(array), size):
        chunks.append(array[i:i + size])

    return chunks


def uniq(array: list) -> list:
    if not array:
        return array
    return list(dict.fromkeys(array))

def verify_signatures(unsigned: UnsignedTransaction, signatures: list[bytes], from_address: Address):
    signed = Transaction.from_unsigned_tx(unsigned, signatures)

    inputs:list[Input] = []
    for sinp in signed.inputs:
        input = UnsignedInput(sinp.box_id)
        input = Input.from_unsigned_input(input, bytes())
        inputs.append(input)

    unsigned_tx_for_prove = Transaction(inputs, signed.data_inputs, signed.output_candidates)
    
    return verify_signature(from_address, unsigned_tx_for_prove.__bytes__(), signatures[0])