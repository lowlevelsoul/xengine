/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#include "render/Render3d_local.h"
#include "render/Material_local.h"
#include "Camera.h"
#include "math/Math3d.h"
#include <assert.h>

/* To be provided by the implementation */
extern void Render_SubmitScene( render_cmd_scene3d_t * scene );


/*=======================================================================================================================================*/
inline static render_cmd_material_t * Render_GetMaterialCmd( material_t * mat ) {
    render_cmd_material_t * matCmd = NULL;
    material_local_t * matLocal = (material_local_t *) mat;
    
    if ( matLocal->timeStamp == render3d->materialTimestamp ) {
        /* Material has already been seen and batched during this frame,
           so just return the command for the material
         */
        matCmd = &render3d->currScene->materials[ matLocal->batchIndex ];
    }
    else {
        /* First time that this material has been used. Need to allocate a command for
           this material*/
        assert( render3d->currScene->materialCount < render3d->currScene->materialCapacity );
        matCmd = &render3d->currScene->materials[ render3d->currScene->materialCount ];
        ++render3d->currScene->materialCount;
        
        matCmd->head = NULL;
        matCmd->tail = NULL;
        matCmd->material = mat;
        
        /* Need to set the material timestamp */
        matLocal->timeStamp = render3d->materialTimestamp;
    }
    
    return matCmd;
}

/*=======================================================================================================================================*/
void Render_Begin( camera_t * camera, const int32_t * viewport ) {
    render_cmd_scene3d_t * scene = NULL;
    
    assert( render3d->currScene == NULL );
    
    FrameHeap_Reset( render3d->batchHeap );
    
    scene = (render_cmd_scene3d_t*) FrameHeap_Alloc( render3d->batchHeap,  sizeof( render_cmd_scene3d_t ) );
    scene->matProj = camera->projection;
    scene->matViewWorld = camera->transform;
    _Mat4_Inverse( &scene->matView, &camera->transform );
    
    scene->materialCapacity = render3d->batchMaterialCapacity;
    scene->materialCount = 0;
    scene->materials = (render_cmd_material_t*) FrameHeap_Alloc( render3d->batchHeap, render3d->batchMaterialCapacity * sizeof( render_cmd_material_t ) );

    /* Setup a default global light */
    Vec3_Set( scene->globalLightDir, -2, -10, 10 );
    Vec3_Normalise( scene->globalLightDir, scene->globalLightDir );
    Vec3_Set( scene->globalLightColour, 1, 1, 1 );

    ++render3d->materialTimestamp;
    render3d->currScene = scene;
}

/*=======================================================================================================================================*/
void Render_End(void) {
    render_cmd_scene3d_t * scene = render3d->currScene;
    assert( scene != NULL );
    render3d->currScene = NULL;
    
    Render_SubmitScene( scene );
}

/*=======================================================================================================================================*/
void Render_SubmitModel( model_t * model, material_t * mat, const mat4_t * xform ) {
    render_cmd_material_t * matCmd = Render_GetMaterialCmd( mat );
    render_cmd_draw_t * drawCmd = (render_cmd_draw_t *) FrameHeap_Alloc( render3d->batchHeap, sizeof( render_cmd_draw_t ) );
    
    RENDER_CMD_ADD_ITEM( matCmd, drawCmd )
    
    drawCmd->model = model;
    drawCmd->xform = *xform;
}
