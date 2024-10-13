
#pragma once

#include <io.h>
#include "../context.h"

extern bool flow_response;

void set_flow_response(bool response);
void io_common_process();
bool io_ui_process();

#ifdef HAVE_NBGL
#define N_UX_PAIRS 13

static nbgl_layoutTagValue_t pairs[N_UX_PAIRS];
static nbgl_layoutTagValueList_t pairList;

extern nbgl_layoutTagValue_t pairs_global[N_UX_PAIRS];

static char pair_mem_title[N_UX_PAIRS][20];
static char pair_mem_text[N_UX_PAIRS][70];
#endif