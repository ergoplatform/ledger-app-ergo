#ifdef HAVE_NBGL

#include "ainpt_ui.h"
#include "../../helpers/response.h"
#include "../../sw.h"
#include <ux.h>
#include <nbgl_use_case.h>
#include <os.h>
#include <string.h>
#include <glyphs.h>
#include "epk_response.h"
#include "../../context.h"
#include "../../common/bip32_ext.h"
#include "../../common/macros_ext.h"
#include "../../ui/ui_bip32_path.h"
#include "../../ui/ui_application_id.h"
#include "../../ui/ui_approve_reject.h"
#include "../../ui/ui_menu.h"
#include "../../ui/ui_main.h"
#include "../../ui/display.h"
#include <os_print.h>

#define APPLICATION_ID_SUBLEN APPLICATION_ID_STR_LEN + 13
static char sub_message[APPLICATION_ID_SUBLEN];

void ui_action_attest_input(bool approved) {
    set_flow_reseponse(approved);
}

int ui_display_access_token(uint32_t app_access_token, attest_input_ctx_t* context) {
    context->ui.app_token_value = app_access_token;

    if (app_access_token != 0) {
        ui_application_id_screen(app_access_token, context->ui.app_token);
        memset(sub_message, 0, APPLICATION_ID_SUBLEN);
        snprintf(sub_message, APPLICATION_ID_SUBLEN, "Application: 0x%08x", app_access_token);
    }

    nbgl_useCaseReviewStreamingStart(TYPE_TRANSACTION,
                                     &VALIDATE_ICON,
                                     "Confirm Attest Input",
                                     sub_message,
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