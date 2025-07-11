from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, Navigator, Device
from ragger.error import ExceptionRAPDU

from application_client.ergo_command_sender import ErgoCommandSender, Errors, StxState
from helpers.data import ADDRESS_0, NETWORK, TX_ID
from helpers.tx_builder import TxBuilder
from helpers.nav_helper import confirm_approve

def test_can_not_sign_tx_zero_outputs(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:    
    FROM = ADDRESS_0

    builder = TxBuilder()
    builder.input(FROM, TX_ID, 0, 1000000000)
    builder.data_input(FROM, TX_ID, 0)
    builder.fee(None)
    tx = builder.build_app_tx()

    valid = False
    try:
        client = ErgoCommandSender(backend)
    
        for nb in client.sign_tx_flow(tx, NETWORK.__int__()):
            if nb == StxState.ATTEST:
                confirm_approve(device, backend, navigator)
            
            elif nb == StxState.WAITING_CONFIRMATION:
                scenario_navigator.review_approve(custom_screen_text="Sign transaction" if device.is_nano else None, do_comparison=False)
    except ExceptionRAPDU as ex:
        valid = ex.status == Errors.SW_BAD_OUTPUT_COUNT

    assert valid