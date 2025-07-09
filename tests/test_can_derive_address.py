from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario
import base58

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0

def test_can_derive_address(backend: BackendInterface, scenario_navigator: NavigateWithScenario) -> None:
    address = ADDRESS_0

    client = ErgoCommandSender(backend)
    with client.derive_address(address.network.__int__(), address.path.__str__()):
        scenario_navigator.review_approve()

    response = client.get_async_response().data
    compare = base58.b58decode(address.address.to_str(address.network))
    
    assert response == compare