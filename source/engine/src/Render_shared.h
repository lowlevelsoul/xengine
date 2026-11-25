/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_RENDER_SHARED_H
#define XE_RENDER_SHARED_H

#include "xe/Platform.h"

typedef struct render_scene3d_s render_scene3d_t;

typedef struct render_pass_s {
        void ( * Initialise )( void );
        void ( * Finalise )( void );
        void ( * Render )( struct render_pass_s * prevPass, render_scene3d_t * scene );
} render_pass_t;

#endif // XE_RENDER_SHARED_H