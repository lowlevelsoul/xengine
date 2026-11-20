/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"

void RenderParams_Initialise( render_params_t* params ) {
    if ( params ) {
        params->width = 800;
        params->height = 600;
        params->fullscreen = false;
        params->debug = false;
        params->vsync = true;
        params->maxNumFramesInFlight = 2;
        params->renderMemHeapSize = 64 * 1024 * 1024; // 64 MB
    }
}