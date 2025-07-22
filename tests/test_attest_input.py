from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, NavigationScenarioData, Navigator, Device, UseCase

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0, TX_ID
from helpers.tx_builder import TxBuilder
from helpers.unsigned_box import UnsignedBox

def test_attest_input(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    unsigned_box: UnsignedBox = TxBuilder().input(ADDRESS_0, TX_ID, 0, 1000000000).inputs[0].box

    client = ErgoCommandSender(backend)
    
    for nb in client.attest_input(unsigned_box, None):
        if nb != None:
            box = nb
            break

        scenario = NavigationScenarioData(device,
                                          backend,
                                          UseCase.ADDRESS_CONFIRMATION,
                                          approve=True)
        
        if not device.is_nano:
            scenario.validation.pop()

        navigator.navigate_until_text_and_compare(
                navigate_instruction=scenario.navigation,
                validation_instructions=scenario.validation,
                text="Confirm" if not device.is_nano else "Approve",
                path=scenario_navigator.screenshot_path,
                screen_change_before_first_instruction=False,
                screen_change_after_last_instruction=False,
                test_case_name=scenario_navigator.test_name)



    assert box.box == unsigned_box
    assert len(box.frames) == 1
    
    frame = box.frames[0]
    assert len(frame.box_id) == 64
    assert frame.count == 1
    assert frame.index == 0
    assert frame.amount == 1000000000
    assert len(frame.tokens) == 0