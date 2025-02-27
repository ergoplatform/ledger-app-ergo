#include <cx.h>
#include <os_io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <parser.h>
#include <sys/types.h>
#include <buffer_ext.h>
#include "commands/attestinput/ainpt_handler.h"


uint8_t G_io_apdu_buffer[IO_APDU_BUFFER_SIZE];

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint8_t *input = NULL;

    while (size > 5) {
        uint8_t ins = data[0];
        uint8_t p1 = data[1];
        uint8_t p2 = data[2];
        uint8_t lc = data[3];

        data += sizeof(uint8_t) * 4;
        size -= sizeof(uint8_t) * 4;

        if (size < lc) {
            return 0;
        }

        uint8_t *input = malloc(lc);
        if (input == NULL) {
            return 0;
        }

        memcpy(input, data, lc);

        data += lc;
        size -= lc;

        buffer_t buf;
        buffer_init(&buf, input, lc);

        BEGIN_TRY {
            TRY {
                handler_attest_input(&buf, p1, p2);
            }
            CATCH_ALL {
            }
            FINALLY {
            }
        }
        END_TRY;

        free(input);
    }
    return 0;
}