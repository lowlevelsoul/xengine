/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "xe/RenderBatch.h"

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );

typedef struct render_metal_s {


} render_metal_t;

static render_metal_t render;
static bool isRenderCreated = false;

static void Render_CreateSwapchain( render_params_t* params );

/*==========================================================================================================*/
void Render_Initialise( render_params_t* params ) {
    RenderWindow_Create( params );
    Render_CreateSwapchain( params );
}

/*==========================================================================================================*/
void Render_Finalise( void ) {
    RenderWindow_Destroy();
    if ( isRenderCreated ) {
        isRenderCreated = false;
    }
}

/*==========================================================================================================*/
void Render_CreateSwapchain( render_params_t* params ) {
    isRenderCreated = true;
}

/*==========================================================================================================*/
void Render_SubmitScene( batch_cmd_scene_t * scene ) {
    
}

