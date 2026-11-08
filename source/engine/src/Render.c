/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"

typedef struct render_s {
    render_window_t* window;
} render_t;

static render_t render;
static bool render_initialized = false;

extern render_window_t* RenderWindow_Create( int32_t width, int32_t height, bool fullscreen, const char* title );
extern void RenderWindow_Destroy( render_window_t* window );

extern bool RenderWindow_PumpWindowEvents( render_window_t * window );
extern void RenderWindow_GetWindowSize( render_window_t * window, float * width, float * height );
extern void RenderWindow_GetWindowScale( render_window_t * window, float * scaleX, float * scaleY );

/*==========================================================================================================*/
X_API void Render_Create( const render_params_t* params ) {
    if ( render_initialized ) {
        return;
    }

    render.window = RenderWindow_Create( params->width, params->height, params->fullscreen, params->title );
    render_initialized = true;
}

/*==========================================================================================================*/
X_API void Render_Destroy(void) {
    if ( render.window ) {
        RenderWindow_Destroy( render.window );
        render.window = NULL;
    }

    render_initialized = false;
}

/*==========================================================================================================*/
X_API render_window_t* Render_GetWindow(void) {
    return render.window;
}

/*==========================================================================================================*/
X_API void Render_GetWindowSize( float * width, float * height ) {
    RenderWindow_GetWindowSize( render.window, width, height);
}

/*==========================================================================================================*/
X_API void Render_GetWindowScale( float * scaleX, float * scaleY ) {
    RenderWindow_GetWindowScale( render.window, scaleX, scaleY );
}

/*==========================================================================================================*/
X_API bool Render_PumpWindowEvents(void) {
    return RenderWindow_PumpWindowEvents( render.window );
}
