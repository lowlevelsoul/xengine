/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __RENDERTYPES_H__
#define __RENDERTYPES_H__

#include "Platform.h"

typedef struct render_window_s render_window_t;

typedef struct render_params_s {
    uint32_t width;
    uint32_t height;
    bool fullscreen;
    bool vsync;
    const char* title;
    uint32_t maxFramesInFlight;
} render_params_t;

#endif // __RENDERTYPES_H__