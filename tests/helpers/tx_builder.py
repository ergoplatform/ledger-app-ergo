from ergo_lib_python.transaction import TxBuilder as TxBuilderErgo, TxId
from ergo_lib_python.chain import ErgoBox, ContextExtension, ErgoBoxCandidate, Address, Token
from ergo_lib_python.wallet import DerivationPath

from helpers.data import NETWORK
from helpers.unsigned_box import UnsignedBox
from helpers.extended_address import ExtendedAddress


def create_ergo_box(recipient: str, tx_id: str, index: int, amount: int, tokens: list[Token] | None):
    return ErgoBox.from_box_candidate(ErgoBoxCandidate(
        value=amount, script=Address(recipient, NETWORK), creation_height=0, tokens=tokens
    ), TxId(tx_id), index)

def to_unsigned_box(ergo_box: ErgoBox, context_extension: ContextExtension, sign_path: str):
    return UnsignedBox(ergo_box, context_extension, sign_path)


class ErgoTxInput:
    def __init__(self, box: UnsignedBox, ergo: ErgoBox):
        self.box = box
        self.ergo = ergo
        

class ErgoTxBuilder:

    inputs:list[ErgoTxInput]

    def __init__(self):
        self.amount = 0 # int 64
        self.inputs = []
        self.data_inputs = []
        self.outputs = []
        self.fee_amount = 1100000 #u64, see https://github.com/ergoplatform/sigma-rust/blob/2725f402ca6398c2bfeeaede3b144e07793cee9b/ergo-lib/src/wallet/tx_builder.rs
        self.burning_tokens = []
        self.minting_outputs = []
        self.change_address = None
        self.change_map = None

    def input(self, extended_address: ExtendedAddress, tx_id: str, index: int, amount: int, tokens: list[Token] | None = None):
        ergo_box = create_ergo_box(extended_address.address.to_str(extended_address.network), tx_id, index, amount, tokens)
        return self.box_input(ergo_box, extended_address.path)
    
    def box_input(self, ergo_box: ErgoBox, path: DerivationPath):
        context_extension = ContextExtension()
        box = to_unsigned_box(ergo_box, context_extension, path.__str__())
        self.inputs.append(ErgoTxInput(box, ergo_box))
        return self;    


class TxBuilder(ErgoTxBuilder):
    def __init__(self):
        super().__init__()