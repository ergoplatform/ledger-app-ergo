from ledgered.devices import Device
from ragger.navigator import Navigator, NavInsID, NavIns
from ragger.firmware.touch import positions


# In this test we check the behavior of the device main menu
def test_app_mainmenu(device: Device,
                      navigator: Navigator,
                      test_name: str,
                      default_screenshot_path: str) -> None:
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
    navigator.navigate_and_compare(default_screenshot_path, test_name, instructions,
                                   screen_change_before_first_instruction=False)
