from ledgered.devices import Device
from ragger.navigator import Navigator, NavInsID, NavIns
from ragger.firmware.touch import positions
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigationScenarioData, Navigator, Device, UseCase

def enable_blind_signing(device: Device, navigator: Navigator):
    # Navigate in the main menu
    instructions = []
    if device.is_nano:
        instructions += [
            # settings
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
            # enable blind signing
            NavInsID.BOTH_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
            # about
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            # return to main menu
            NavInsID.BOTH_CLICK
        ]
    else:
        instructions += [
            # settings
            NavInsID.USE_CASE_HOME_SETTINGS,
            # enable blind signing
            NavIns(NavInsID.TOUCH, positions.POSITIONS["ChoiceList"][device.type][1]),
            # about
            NavIns(NavInsID.TOUCH, positions.POSITIONS["UseCaseSettings"][device.type]["next"]),
            # return to main menu
            NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT
        ]

    assert len(instructions) > 0
    navigator.navigate(instructions, screen_change_before_first_instruction=False)

def confirm_approve(device: Device, backend: BackendInterface, navigator: Navigator):
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