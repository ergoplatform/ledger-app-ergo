from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0

def test_can_show_address(backend: BackendInterface, scenario_navigator: NavigateWithScenario) -> None:
    address = ADDRESS_0

    client = ErgoCommandSender(backend)
    with client.derive_address(address.network.__int__(), address.path.__str__(), None, True):
        scenario_navigator.review_approve()

    response = client.get_async_response().data

    assert len(response) == 0