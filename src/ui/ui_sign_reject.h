#pragma once
#ifdef HAVE_BAGL
#include <stdbool.h>
#include <ux.h>

typedef void (*ui_sign_reject_callback)(bool, void*);

void ui_sign_reject_screens(ui_sign_reject_callback cb,
                            void* context,
                            const ux_flow_step_t** sign,
                            const ux_flow_step_t** reject);

#endif