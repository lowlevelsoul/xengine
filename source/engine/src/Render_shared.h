/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_RENDER_SHARED_H
#define XE_RENDER_SHARED_H

#include "xe/Platform.h"
#include "xe/RenderTypes.h"

typedef struct batch_cmd_scene3d_s batch_cmd_scene3d_t;

typedef struct render_pass_s {
    void ( *Initialise )( struct render_pass_s* self_ );
    void ( *Finalise )( struct render_pass_s* self_ );
    void ( *Render )( struct render_pass_s* self_, struct render_pass_s* prevPass, batch_cmd_scene3d_t* scene );
    
    render_gpu_fence_t * fence;
    bool isFenceValid;
} render_pass_t;

#define RENDER_PASS_DEFINE( name, prefix, struct_type )                                                                \
    static void prefix##_Initialise( struct render_pass_s* self_ );                                                    \
    static void prefix##_Finalise( struct render_pass_s* self_ );                                                      \
    static void prefix##_Render( struct render_pass_s* self_, struct render_pass_s* prevPass,                          \
                                 batch_cmd_scene3d_t* scene );                                                            \
    static struct_type name##PassInst = { .super.Initialise = prefix##_Initialise,                                     \
                                          .super.Finalise = prefix##_Finalise,                                         \
                                          .super.Render = prefix##_Render };                                           \
    render_pass_t* name##Pass = &name##PassInst.super;

#define RENDER_PASS_EXTERN( name ) extern render_pass_t* name##Pass;

#define RenderPass_Get( type, self_ ) ( ( type* )( self_ ) )

    

#endif // XE_RENDER_SHARED_H
