from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario, NavigationScenarioData, Navigator, Device, UseCase

from application_client.ergo_command_sender import ErgoCommandSender
from helpers.data import ACCOUNT, AUTH_TOKEN

def test_can_get_extended_public_key_with_auth_token(backend: BackendInterface, device: Device, scenario_navigator: NavigateWithScenario, navigator: Navigator) -> None:
    client = ErgoCommandSender(backend)
    with client.ext_pub_key(ACCOUNT.path.__str__(), AUTH_TOKEN):

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
                test_case_name=scenario_navigator.test_name)

    response = client.get_async_response().data

    assert response[0:33]  == ACCOUNT.public_key.pub_key_bytes
    assert response[33:65] == ACCOUNT.public_key.chain_code