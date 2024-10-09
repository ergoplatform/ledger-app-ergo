
#pragma once

#include <io.h>
#include "../context.h"

static bool flow_response = false;

static void set_flow_reseponse(bool response) {
    flow_response = response;
    app_set_ui_busy(false);
}

static bool io_ui_process() {
    // We are not waiting for the client's input, nor we are doing computations on the device
    //io_clear_processing_timeout();

    app_set_ui_busy(true);

    io_seproxyhal_general_status();
    do {
        io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
        io_seproxyhal_handle_event();
        io_seproxyhal_general_status();
    } while (io_seproxyhal_spi_is_status_sent() && G_app_context.is_ui_busy);

    // We're back at work, we want to show the "Processing..." screen when appropriate
    //io_start_processing_timeout();

    return flow_response;
}