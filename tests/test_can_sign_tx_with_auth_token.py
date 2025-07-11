from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, NavigationScenarioData, Navigator, Device, UseCase

from application_client.ergo_command_sender import ErgoCommandSender, StxState
from helpers.data import ADDRESS_0, ADDRESS_1, CHANGE_ADDRESS, NETWORK, AUTH_TOKEN
from helpers.tx_builder import TxBuilder

def test_can_sign_tx_with_auth_token(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    FROM    = ADDRESS_0
    TO      = ADDRESS_1
    CHANGE  = CHANGE_ADDRESS

    tx_id = "0000000000000000000000000000000000000000000000000000000000000000"

    builder = TxBuilder()
    builder.input(FROM, tx_id, 0, 1000000000)
    builder.data_input(FROM, tx_id, 1)
    builder.output(TO, 100000000)
    builder.fee(1000000)
    builder.change(CHANGE)
    tx = builder.build()

    client = ErgoCommandSender(backend)
    
    for nb in client.sign_tx_flow(tx.app_tx, NETWORK.__int__(), AUTH_TOKEN):
        if nb == StxState.ATTEST:
            scenario = NavigationScenarioData(device,
                                          backend,
                                          UseCase.ADDRESS_CONFIRMATION,
                                          approve=True)
        
            if not device.is_nano:
                scenario.validation.pop()

            navigator.navigate_until_text(
                navigate_instruction=scenario.navigation,
                validation_instructions=scenario.validation,
                text="Confirm" if not device.is_nano else "Approve",
                screen_change_before_first_instruction=False,
                screen_change_after_last_instruction=False)
            
        elif nb == StxState.WAITING_CONFIRMATION:
            scenario_navigator.review_approve(custom_screen_text="Sign transaction" if device.is_nano else None)