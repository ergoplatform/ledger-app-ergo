from ergo_lib_python.chain import ErgoBox, ContextExtension

class UnsignedBox:
    def __init__(self, ergo_box: ErgoBox, context_extension: ContextExtension, sign_path: str):
        self.tx_id = ergo_box.transaction_id.__str__()
        self.index = ergo_box.index
        self.value = ergo_box.value
        self.ergo_tree:bytes = ergo_box.ergo_tree.__bytes__()
        self.creation_height = ergo_box.creation_height
        self.tokens = ergo_box.tokens
        self.additional_registers = ergo_box.additional_registers.__bytes__()
        self.extension = context_extension.__bytes__() # b"" if context_extension.__len__() == 0 else context_extension.__bytes__()
        self.sign_path = sign_path
