from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, NavigationScenarioData, Navigator, Device, UseCase

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0, ADDRESS_1, CHANGE_ADDRESS
from helpers.tx_builder import TxBuilder
from helpers.unsigned_box import UnsignedBox

def test_can_sign_tx(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
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
    
