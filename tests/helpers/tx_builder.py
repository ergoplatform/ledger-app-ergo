from ergo_lib_python.transaction import TxBuilder as TxBuilderErgo, TxId, DataInput, UnsignedTransaction
from ergo_lib_python.chain import ErgoBox, ContextExtension, ErgoBoxCandidate, Address, Token, TokenId
from ergo_lib_python.wallet import DerivationPath, select_boxes_simple
from typing import Iterable, Set

from helpers.data import NETWORK
from helpers.unsigned_box import UnsignedBox
from helpers.extended_address import ExtendedAddress

def distinct_token_ids(output_candidates: Iterable[ErgoBoxCandidate]) -> list[TokenId]:
    token_ids = [
        t.token_id
        for b in output_candidates
        for t in (b.tokens or [])
    ]
    return list(dict.fromkeys(token_ids)) #set(token_ids)

def create_ergo_box(recipient: str, tx_id: str, index: int, amount: int, tokens: list[Token] | None):
    return ErgoBox.from_box_candidate(ErgoBoxCandidate(
        value=amount, script=Address(recipient, NETWORK), creation_height=0, tokens=tokens
    ), TxId(tx_id), index)

def to_unsigned_box(ergo_box: ErgoBox, context_extension: ContextExtension, sign_path: str):
    return UnsignedBox(ergo_box, context_extension, sign_path)

def is_tokens_equal(t1: Token, t2: Token) -> bool:
    return t1.token_id.__str__() == t2.token_id.__str__() and t1.amount == t2.amount

def to_data_input(data_input: DataInput):
    return data_input.box_id.__str__()


class ErgoTxInput:
    def __init__(self, box: UnsignedBox, ergo: ErgoBox):
        self.box = box
        self.ergo = ergo

class ErgoToken:
    def __init__(self, id: str, amount: int):
        self.id = id
        self.amount = amount   

class ErgoChangeMap:
    def __init__(self, address: str, path: str):
        self.address = address
        self.path = path    

class AppTx:
    def __init__(self, inputs: list[UnsignedBox], data_inputs: list[str], outputs: list[ErgoBoxCandidate], distinct_token_ids: list[TokenId], change_map: ErgoChangeMap | None):
        self.inputs = inputs
        self.data_inputs = data_inputs    
        self.outputs = outputs
        self.distinct_token_ids = distinct_token_ids
        self.change_map = change_map

class AttestedTransaction:
    def __init__(self, app_tx: AppTx, ergo_tx: UnsignedTransaction, u_inputs: list[ErgoBox]):
        self.app_tx = app_tx
        self.ergo_tx = ergo_tx
        self.u_inputs = u_inputs
        

class ErgoTxBuilder:

    inputs:list[ErgoTxInput]

    def __init__(self):
        self.amount = 0 # int 64
        self.inputs:list[ErgoTxInput] = []
        self.data_inputs:list[DataInput] = []
        self.outputs:list[ErgoBoxCandidate] = []
        self.fee_amount:int | None = 1100000 #u64, see https://github.com/ergoplatform/sigma-rust/blob/2725f402ca6398c2bfeeaede3b144e07793cee9b/ergo-lib/src/wallet/tx_builder.rs
        self.burning_tokens:list[Token] = []
        self.minting_outputs:list[Token | None] = []
        self.change_address: Address | None = None
        self.change_map: ErgoChangeMap | None = None

    def input(self, extended_address: ExtendedAddress, tx_id: str, index: int, amount: int, tokens: list[Token] | None = None):
        ergo_box = create_ergo_box(extended_address.address.to_str(extended_address.network), tx_id, index, amount, tokens)
        return self.box_input(ergo_box, extended_address.path)
    
    def box_input(self, ergo_box: ErgoBox, path: DerivationPath):
        context_extension = ContextExtension()
        box = to_unsigned_box(ergo_box, context_extension, path.__str__())
        self.inputs.append(ErgoTxInput(box, ergo_box))
        return self
    
    def data_input(self, address: ExtendedAddress, tx_id: str, index: int):
        ergo_box = create_ergo_box(address.address.to_str(address.network), tx_id, index, 100000000, [])
        self.data_inputs.append(DataInput(ergo_box.box_id))
        return self
    
    def output(self, address: ExtendedAddress, value:int, send_tokens: list[ErgoToken] | None = None, mint_token_amount: int | None = None):        
        stokens: list[Token] | None = None
        if send_tokens != None:
            stokens = []
            for stoken in send_tokens:
                id = TokenId(stoken.id)
                stokens.append(Token(id, stoken.amount))
        
        mint_token = None
        if mint_token_amount != None:
            id = TokenId(self.inputs[0].ergo.box_id.__str__())
            mint_token = Token(id, mint_token_amount)
            self.minting_outputs.append(mint_token)
        else:
            self.minting_outputs.append(None)
        
        output = ErgoBoxCandidate(
            value=value, script=address.address, creation_height=0,
            tokens=stokens, mint_token=mint_token
        )
        self.amount = self.amount + output.value #.checked_add(output.value().as_i64())
        self.outputs.append(output)
        return self
    
    def burn(self, tokens: list[ErgoToken]):
        for token in tokens:
            id = TokenId(token.id)
            self.burning_tokens.append(Token(id, token.amount))

        return self    

    def fee(self, amount: int | None):
        self.fee_amount = amount
        return self

    def change(self, extendedAddress: ExtendedAddress):
        self.change_map = ErgoChangeMap(extendedAddress.address.to_str(extendedAddress.network), extendedAddress.path.__str__())
        self.change_address = extendedAddress.address
        return self
    
    def build(self):
        target_balance = self.amount + self.fee_amount

        target_tokens:list[Token] = []
        for idx, output in enumerate(self.outputs):
            minting = self.minting_outputs[idx]
            for tkn in output.tokens:
                if (minting != None and not is_tokens_equal(tkn, minting)) or minting == None:
                    target_tokens.append(tkn)

        for tkn in self.burning_tokens:
            target_tokens.append(tkn)
        
        inputs:list[ErgoBox] = []
        for input in self.inputs:
            inputs.append(input.ergo)

        outputs:list[ErgoBoxCandidate] = []
        for output in self.outputs:
            outputs.append(output)

        data_inputs:list[DataInput] = []
        for input in self.data_inputs:
            data_inputs.append(input)

        box_selection = select_boxes_simple(inputs, target_balance, target_tokens)

        tx_builder = TxBuilderErgo(box_selection, outputs, 0, self.fee_amount, self.change_address)
        if len(self.burning_tokens) > 0:
            burn:list[Token] = []
            for tkn in self.burning_tokens:
                burn.append(tkn)

            tx_builder.set_token_burn_permit(burn)

        tx_builder.set_data_inputs(data_inputs)
        ergo_tx = tx_builder.build()

        tx_inputs:list[ErgoTxInput] = []
        for inp in ergo_tx.inputs:
            for linp in self.inputs:
                if inp.box_id.__str__() == linp.ergo.box_id.__str__():
                    tx_inputs.append(linp)
        

        app_tx = AppTx([i.box for i in tx_inputs], 
            [to_data_input(i) for i in ergo_tx.data_inputs],
            ergo_tx.output_candidates,
            distinct_token_ids(ergo_tx.output_candidates),
            self.change_map
        )
        tx_inputs[0].ergo

        return AttestedTransaction(app_tx, ergo_tx, [i.ergo for  i in tx_inputs])


class TxBuilder(ErgoTxBuilder):
    def __init__(self):
        super().__init__()