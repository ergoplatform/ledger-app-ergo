from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario
import base58

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ADDRESS_0, AUTH_TOKEN

def test_can_derive_address_with_auth_token(backend: BackendInterface, scenario_navigator: NavigateWithScenario) -> None:
    address = ADDRESS_0
    auth_token = AUTH_TOKEN

    client = ErgoCommandSender(backend)
    with client.derive_address(address.network.__int__(), address.path.__str__(), auth_token):
        scenario_navigator.address_review_approve(custom_screen_text="Approve")

    print("wait for response")
    response = client.get_async_response().data
    compare = base58.b58decode(address.address.to_str(address.network))
    
    assert response == compare