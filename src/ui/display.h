
#pragma once

#include <io.h>
#include "../context.h"
#include "../constants.h"

extern bool flow_response;

void set_flow_response(bool response);
void io_common_process();
bool io_ui_process();

#define N_UX_PAIRS TOKEN_MAX_COUNT * 2 + 10
static char pair_mem_title[N_UX_PAIRS][20];
static char pair_mem_text[N_UX_PAIRS][70];

#ifdef HAVE_NBGL
static nbgl_layoutTagValueList_t pair_list;
extern nbgl_layoutTagValue_t pairs_global[N_UX_PAIRS];
#endif