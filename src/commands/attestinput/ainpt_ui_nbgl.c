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

void ui_action_attest_input(bool approved) {
    set_flow_reseponse(approved);
}

int ui_display_access_token(uint32_t app_access_token, attest_input_ctx_t* context) {
    context->ui.app_token_value = app_access_token;

    pairList.nbMaxLinesForValue = 2;

    int n_pairs = 0;

    pairs[n_pairs++] = (nbgl_layoutTagValue_t){
            .item = "Confirm Attest Input",
            .value = ""};

    pairs[n_pairs].item = "test";
    pairs[n_pairs].value = "test2";
    n_pairs++;

    pairs[n_pairs].item = "nest";
    pairs[n_pairs].value = "nest2";
    n_pairs++;

    if (app_access_token != 0) {
        pairs[n_pairs++] = ui_application_id_screen(app_access_token, context->ui.app_token);
    }

    pairList.nbPairs = n_pairs;
    pairList.pairs = pairs;

	nbgl_useCaseReview(TYPE_OPERATION, &pairList, &C_round_warning_64px, "Confirm Attest Input", NULL, "Confirm Attest Input", ui_action_attest_input);
    bool approved = io_ui_process();

    if (approved) {
        app_set_connected_app_id(context->ui.app_token_value);
        context->state = ATTEST_INPUT_STATE_APPROVED;
        send_response_attested_input_session_id(context->session);
    } else {
        app_set_current_command(CMD_NONE);
        res_deny();
    }

    ui_menu_main();

    return 0;
}

#endif