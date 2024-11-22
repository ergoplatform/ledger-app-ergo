#ifdef HAVE_BAGL
#include <glyphs.h>
#include "ui_sign_reject.h"

static ui_sign_reject_callback G_ui_sign_reject_callback;
static void* G_ui_sign_reject_callback_context;

// Step with sign button
UX_STEP_CB(ux_sign_step,
           pb,
           G_ui_sign_reject_callback(true, G_ui_sign_reject_callback_context),
           {
               &C_icon_validate_14,
               "Sign transaction",
           });
// Step with reject button
UX_STEP_CB(ux_sign_reject_step,
           pb,
           G_ui_sign_reject_callback(false, G_ui_sign_reject_callback_context),
           {
               &C_icon_crossmark,
               "Reject",
           });

void ui_sign_reject_screens(ui_sign_reject_callback cb,
                            void* context,
                            const ux_flow_step_t** sign,
                            const ux_flow_step_t** reject) {
    G_ui_sign_reject_callback = cb;
    G_ui_sign_reject_callback_context = context;
    *sign = &ux_sign_step;
    *reject = &ux_sign_reject_step;
}
#endif