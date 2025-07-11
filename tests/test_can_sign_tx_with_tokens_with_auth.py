from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, Navigator, Device

from application_client.ergo_command_sender import ErgoCommandSender, StxState
from helpers.data import ADDRESS_0, ADDRESS_1, CHANGE_ADDRESS, NETWORK, TX_ID, AUTH_TOKEN
from helpers.tx_builder import ErgoToken, TxBuilder
from helpers.nav_helper import confirm_approve

def test_can_sign_tx_with_tokens_with_auth(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    FROM        = ADDRESS_0
    TO          = ADDRESS_1
    CHANGE      = CHANGE_ADDRESS
    TOKEN_ID    = "1111111111111111111111111111111111111111111111111111111111111111"
    TOKENS      = [ErgoToken(TOKEN_ID, 1000)]

    builder = TxBuilder()
    builder.input(FROM, TX_ID, 0, 1000000000, TOKENS)
    builder.data_input(FROM, TX_ID, 1)
    builder.output(TO, 100000000, TOKENS)
    builder.fee(1000000)
    builder.change(CHANGE)
    tx = builder.build()

    client = ErgoCommandSender(backend)
    
    for nb in client.sign_tx_flow(tx.app_tx, NETWORK.__int__(), AUTH_TOKEN):
        if nb == StxState.ATTEST:
            confirm_approve(device, backend, navigator)
            
        elif nb == StxState.WAITING_CONFIRMATION:
            scenario_navigator.review_approve(custom_screen_text="Sign transaction" if device.is_nano else None)