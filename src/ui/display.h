
#pragma once

#include <io.h>
#include "../context.h"
#include "../constants.h"

extern bool flow_response;

void set_flow_response(bool response);
void io_common_process();
bool io_ui_process();

#ifdef HAVE_NBGL
#define N_UX_PAIRS 10

static nbgl_layoutTagValueList_t pair_list;
extern nbgl_layoutTagValue_t pairs_global[N_UX_PAIRS];

static char pair_mem_title[N_UX_PAIRS][20];
static char pair_mem_text[N_UX_PAIRS][70];
#endif