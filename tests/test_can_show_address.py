from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0

def test_can_show_address(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario) -> None:
    address = ADDRESS_0

    client = ErgoCommandSender(backend)
    with client.derive_address(address.network.__int__(), address.path.__str__(), None, True):
        scenario_navigator.address_review_approve(custom_screen_text="Approve" if device.is_nano else "Confirm")

    response = client.get_async_response().data

    assert len(response) == 0