/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __RENDER_H__
#define __RENDER_H__

#include "Platform.h"
#include "RenderTypes.h"

X_API void Render_Create( const render_params_t* params );
X_API void Render_Destroy(void);
X_API void Render_BeginFrame(void);
X_API void Render_EndFrame(void);
X_API bool Render_PumpWindowEvents(void);
X_API render_window_t* Render_GetWindow(void);
X_API void Render_GetWindowSize( float * width, float * height );
X_API void Render_GetWindowScale( float * scaleX, float * scaleY );

#endif // __RENDER_H__
