#ifdef HAVE_NBGL

#include "ui_application_id.h"

const nbgl_layoutTagValue_t ui_application_id_screen(uint32_t app_id,
                                                     char buffer[static APPLICATION_ID_STR_LEN]) {
    memset(buffer, 0, APPLICATION_ID_STR_LEN);
    snprintf(buffer, APPLICATION_ID_STR_LEN, "0x%08x", app_id);

    nbgl_layoutTagValue_t tag = (nbgl_layoutTagValue_t){.item = "Application", .value = buffer};

    return tag;
}

const void ui_application_id_screen_buf(uint32_t app_id,
                                        char buffer[static APPLICATION_ID_STR_LEN]) {
    memset(buffer, 0, APPLICATION_ID_STR_LEN);
    snprintf(buffer, APPLICATION_ID_STR_LEN, "Application: 0x%08x", app_id);
}

#endif