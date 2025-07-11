from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, Navigator, Device

from application_client.ergo_command_sender import ErgoCommandSender, StxState
from helpers.data import ADDRESS_0, ADDRESS_SCRIPT, CHANGE_ADDRESS, NETWORK, TX_ID, AUTH_TOKEN
from helpers.tx_builder import TxBuilder
from helpers.nav_helper import confirm_approve, enable_blind_signing

def test_can_blind_sign_tx_with_auth_token(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    FROM    = ADDRESS_0
    TO      = ADDRESS_SCRIPT
    CHANGE  = CHANGE_ADDRESS

    enable_blind_signing(device, navigator)

    builder = TxBuilder()
    builder.input(FROM, TX_ID, 0, 1000000000)
    builder.data_input(FROM, TX_ID, 1)
    builder.output(TO, 100000000)
    builder.fee(1000000)
    builder.change(CHANGE)
    tx = builder.build()

    client = ErgoCommandSender(backend)
    
    for nb in client.sign_tx_flow(tx.app_tx, NETWORK.__int__(), AUTH_TOKEN):
        if nb == StxState.ATTEST:
            confirm_approve(device, backend, navigator)
            
        elif nb == StxState.WAITING_CONFIRMATION:
            scenario_navigator.review_approve_with_warning(custom_screen_text="Sign transaction" if device.is_nano else None)