#include "ui_dynamic_flow.h"

#include "../globals.h"
#include "../constants.h"
#include "../helpers/response.h"
#include "ui_menu.h"

#include <ux.h>

struct ui_dynamic_flow_ctx_t {
    uint8_t screen_count;
    int8_t current_screen;
    ui_dynamic_flow_show_screen_cb show_cb;
    void *cb_context;
};

static ux_layout_bnnn_paging_params_t G_ui_dynamic_step_params[1];
static struct ui_dynamic_flow_ctx_t G_dynamic_flow_context;

// This is a special function we must call for bnnn_paging to work properly in an edgecase.
// It does some weird stuff with the `G_ux` global which is defined by the SDK.
// No need to dig deeper into the code, a simple copy paste will do.
void bnnn_paging_edgecase() {
    G_ux.flow_stack[G_ux.stack_count - 1].prev_index =
        G_ux.flow_stack[G_ux.stack_count - 1].index - 2;
    G_ux.flow_stack[G_ux.stack_count - 1].index--;
    ux_flow_relayout();
}

#define DISPLAY_DYNAMIC_STATE(switch_method)                                                      \
    do {                                                                                          \
        uint16_t res = G_dynamic_flow_context.show_cb(G_dynamic_flow_context.current_screen,      \
                                                      (char *) G_ui_dynamic_step_params[0].title, \
                                                      (char *) G_ui_dynamic_step_params[0].text,  \
                                                      G_dynamic_flow_context.cb_context);         \
        if (res == SW_OK) {                                                                       \
            switch_method();                                                                      \
        } else {                                                                                  \
            res_error(res);                                                                       \
            clear_context(&G_context, CMD_NONE);                                                  \
            ui_menu_main();                                                                       \
        }                                                                                         \
    } while (0)

static inline void ui_dynamic_step_right() {
    if (G_dynamic_flow_context.current_screen >= G_dynamic_flow_context.screen_count) {
        G_dynamic_flow_context.current_screen = G_dynamic_flow_context.screen_count - 1;
        DISPLAY_DYNAMIC_STATE(bnnn_paging_edgecase);
    } else {
        G_dynamic_flow_context.current_screen++;
        if (G_dynamic_flow_context.current_screen < G_dynamic_flow_context.screen_count) {
            DISPLAY_DYNAMIC_STATE(bnnn_paging_edgecase);
        } else {
            // go to the next static screen
            ux_flow_next();
        }
    }
}

static inline void ui_dynamic_step_left() {
    if (G_dynamic_flow_context.current_screen < 0) {
        G_dynamic_flow_context.current_screen = 0;
        DISPLAY_DYNAMIC_STATE(ux_flow_next);
    } else {
        G_dynamic_flow_context.current_screen--;
        if (G_dynamic_flow_context.current_screen < 0) {
            // Similar to `ux_flow_prev()` but updates layout to account for `bnnn_paging`'s
            // weird behaviour.
            bnnn_paging_edgecase();
        } else {
            DISPLAY_DYNAMIC_STATE(ux_flow_next);
        }
    }
}

const ux_flow_step_t ux_dynamic_step = {ux_layout_bnnn_paging_init,
                                        G_ui_dynamic_step_params,
                                        NULL,
                                        NULL};

UX_STEP_INIT(ux_dynamic_upper_delimiter_step, NULL, NULL, { ui_dynamic_step_left(); });
UX_STEP_INIT(ux_dynamic_lower_delimiter_step, NULL, NULL, { ui_dynamic_step_right(); });

bool ui_add_dynamic_flow_screens(uint8_t *screen,
                                 uint8_t dynamic_screen_count,
                                 char *title_storage,
                                 char *text_storage,
                                 ui_dynamic_flow_show_screen_cb show_cb,
                                 void *cb_ctx) {
    if (MAX_NUMBER_OF_SCREENS - *screen < 3) return false;
    if (dynamic_screen_count > 127) return false;

    G_ui_dynamic_step_params[0].title = title_storage;
    G_ui_dynamic_step_params[0].text = text_storage;
    G_dynamic_flow_context.cb_context = cb_ctx;
    G_dynamic_flow_context.screen_count = dynamic_screen_count;
    G_dynamic_flow_context.current_screen = -1;
    G_dynamic_flow_context.show_cb = show_cb;

    G_ux_flow[(*screen)++] = &ux_dynamic_upper_delimiter_step;
    G_ux_flow[(*screen)++] = &ux_dynamic_step;
    G_ux_flow[(*screen)++] = &ux_dynamic_lower_delimiter_step;

    return true;
}
