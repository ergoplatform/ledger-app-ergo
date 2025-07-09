from ledgered.devices import Device
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0, AUTH_TOKEN

def test_can_show_address_with_auth_token(device: Device, backend: BackendInterface, scenario_navigator: NavigateWithScenario) -> None:
    address = ADDRESS_0
    auth_token = AUTH_TOKEN

    client = ErgoCommandSender(backend)
    with client.derive_address(address.network.__int__(), address.path.__str__(), auth_token, True):
        scenario_navigator.address_review_approve(custom_screen_text="Approve" if device.is_nano else "Confirm")

    response = client.get_async_response().data

    assert len(response) == 0