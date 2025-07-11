from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, Navigator, Device
from ergo_lib_python.chain import ErgoBox

from application_client.ergo_command_sender import ErgoCommandSender, StxState
from helpers.data import ADDRESS_0, ADDRESS_1, CHANGE_ADDRESS, NETWORK
from helpers.tx_builder import TxBuilder
from helpers.nav_helper import confirm_approve

def test_can_sign_tx_additional_regs(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    FROM    = ADDRESS_0
    TO      = ADDRESS_1
    CHANGE  = CHANGE_ADDRESS
    ERGO_BOX = ErgoBox.from_json({
                    "boxId": "ef16f4a6db61a1c31aea55d3bf10e1fb6443cf08cff4a1cf2e3a4780e1312dba",
                    "value": 1000000000,
                    "ergoTree": FROM.address.ergo_tree().__bytes__().hex(),
                    "assets": [],
                    "additionalRegisters": {
                    "R5": "0e050102030405",
                    "R4": "04f601"
                    },
                    "creationHeight": 0,
                    "transactionId": "0000000000000000000000000000000000000000000000000000000000000000",
                    "index": 0
                })

    builder = TxBuilder()
    builder.box_input(ERGO_BOX, FROM.path)
    builder.output(TO, 100000000)
    builder.fee(1000000)
    builder.change(CHANGE)
    tx = builder.build()

    client = ErgoCommandSender(backend)
    
    for nb in client.sign_tx_flow(tx.app_tx, NETWORK.__int__()):
        if nb == StxState.ATTEST:
            confirm_approve(device, backend, navigator)
            
        elif nb == StxState.WAITING_CONFIRMATION:
            scenario_navigator.review_approve(custom_screen_text="Sign transaction" if device.is_nano else None)