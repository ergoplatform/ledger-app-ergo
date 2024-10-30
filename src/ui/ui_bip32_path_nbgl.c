#ifdef HAVE_NBGL

#include "ui_bip32_path.h"
#include "../common/bip32_ext.h"

bool ui_bip32_path_screen(uint32_t* path, uint8_t path_len, char* buffer, uint8_t buffer_len) {
    memset(buffer, 0, buffer_len);
    if (!bip32_path_format(path, path_len, buffer, buffer_len)) {
        return false;
    }

    return true;
}

#endif