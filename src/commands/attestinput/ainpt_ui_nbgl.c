#ifdef HAVE_NBGL

#include <os.h>
#include <nbgl_use_case.h>
#include <string.h>
#include <glyphs.h>

#include "ainpt_ui.h"
#include "ainpt_response.h"
#include "../../context.h"
#include "../../common/macros_ext.h"
#include "../../helpers/response.h"
#include "../../ui/ui_application_id.h"
#include "../../ui/ui_menu.h"
#include "../../ui/ui_main.h"
#include "../../ui/display.h"

#define APPLICATION_ID_SUBLEN APPLICATION_ID_STR_LEN + 13
static char sub_message[APPLICATION_ID_SUBLEN];

void ui_action_attest_input(bool approved) {
    set_flow_response(approved);
}

int ui_display_access_token(uint32_t app_access_token, attest_input_ctx_t* context) {
    context->ui.app_token_value = app_access_token;

    if (app_access_token != 0) {
        ui_application_id_screen(app_access_token, context->ui.app_token);
        memset(sub_message, 0, APPLICATION_ID_SUBLEN);
        snprintf(sub_message, APPLICATION_ID_SUBLEN, "Application: 0x%08x", app_access_token);
    }

    nbgl_useCaseChoice(&VALIDATE_ICON,
                       "Confirm Attest Input",
                       sub_message,
                       "Confirm",
                       "Reject",
                       ui_action_attest_input);
    bool approved = io_ui_process();

    if (approved) {
        app_set_connected_app_id(context->ui.app_token_value);
        context->state = ATTEST_INPUT_STATE_APPROVED;
        send_response_attested_input_session_id(context->session);

    } else {
        app_set_current_command(CMD_NONE);
        res_deny();

        ui_menu_main();
    }

    return 0;
}

#endif