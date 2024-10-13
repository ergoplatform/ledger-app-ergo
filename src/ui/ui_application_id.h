#pragma once

#include <ux.h>
#include <stdint.h>

#define APPLICATION_ID_STR_LEN 11

#ifdef HAVE_BAGL

const ux_flow_step_t* ui_application_id_screen(uint32_t app_id,
                                               char buffer[static APPLICATION_ID_STR_LEN]);

#endif

#ifdef HAVE_NBGL
#include <nbgl_use_case.h>

nbgl_layoutTagValue_t ui_application_id_screen(uint32_t app_id,
                                               char buffer[static APPLICATION_ID_STR_LEN]);

void ui_application_id_screen_buf(uint32_t app_id, char buffer[static APPLICATION_ID_STR_LEN]);

#endif