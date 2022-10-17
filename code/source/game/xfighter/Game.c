/*
===========================================================================================================================================

    Copyright 2022 James Steele

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

#include "Game.h"
#include "XeGame.h"
#include "render/Render3d.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/MaterialResource.h"
#include "render/Texture.h"
#include "ecs/Ecs.h"
#include "Fs.h"
#include "resource/Resource.h"
#include "Camera.h"
#include <string.h>
#include <assert.h>

#include "gameplay/CompTransform.h"
#include "gameplay/CompShipModel.h"
#include "gameplay/CompPreview.h"

typedef struct game_s {
    model_t *       shipModel;
    material_t *    shipMaterial;
    camera_t        camera;
    float           rotation;
    
    int32_t         shipModelSystem;
    int32_t         transformSystem;
    int32_t         previewSystem;
    
    ecs_think_params_t thinkParams;
    ecs_entity_t        player;
} game_t;

static game_t gameLocal;
game_t * const game = &gameLocal;
static boolean_t gameInit = false;

void Game_Initialise(void);
void Game_Finalise(void);
void Game_Think( float timeStep );
void Game_Draw( float timeStep );

/*=======================================================================================================================================*/
mem_allocator_t * Game_GetAllocator(void) {
    /* No allocator for now */
    return NULL;
}

/*=======================================================================================================================================*/
void Game_Create( game_interface_t * self_) {
    if (gameInit == true ) {
        return;
    }
    
    memset(game, 0, sizeof(*game));
    self_->initialise = Game_Initialise;
    self_->finalise = Game_Finalise;
    self_->think = Game_Think;
    self_->draw = Game_Draw;
}

/*=======================================================================================================================================*/
void Game_Destroy( game_interface_t * self_) {
    if ( gameInit == false ) {
        return;
    }
}

/*=======================================================================================================================================*/
void Game_Initialise(void) {
    
    xprintf("=== Game Init ==================\n");
    
    Resource_RegisterFactory( model_resource_factory, "mdl" );
    Resource_RegisterFactory( texture_resource_factory, "png" );
    Resource_RegisterFactory( texture_resource_factory, "tga" );
    Resource_RegisterFactory( texture_resource_factory, "jpg" );
    Resource_RegisterFactory( material_resource_factory, "mat" );
    
    Ecs_RegisterComponent( comp_transform_t, ECS_MAX_ENTITIES );
    Ecs_RegisterComponent( comp_shipmodel_t, 32 );
    Ecs_RegisterComponent( comp_preview_t, 32 );
    
    gameLocal.transformSystem =  Ecs_RegisterSystem( &sys_transform, comp_transform_t );
    gameLocal.shipModelSystem = Ecs_RegisterSystem( &sys_shipmodel, comp_shipmodel_t );
    gameLocal.previewSystem = Ecs_RegisterSystem( &sys_preview, comp_preview_t );
    
    gameLocal.player = Ecs_EntityAlloc();
    comp_transform_t * compTransform = Ecs_AddComponent( gameLocal.player, comp_transform_t );
    comp_shipmodel_t * shipModel = Ecs_AddComponent( gameLocal.player, comp_shipmodel_t );
    comp_preview_t * preview = Ecs_AddComponent( gameLocal.player, comp_preview_t );
    Ecs_EntityConstructDefault( gameLocal.player );
    
    resource_t * modelRes = Resource_Load( "~/rx2/ship.mdl" );
    resource_t * materialRes = Resource_Load( "~/rx2/red.mat" );
    
    game->shipModel = (model_t *) Resource_GetData( modelRes );
    game->shipMaterial = ( material_t * ) Resource_GetData( materialRes );
    shipModel->model = game->shipModel;
    shipModel->material = game->shipMaterial;
    
    Camera_Initialise( &gameLocal.camera );
    
    vec3_t eye = {0, 15, 0 };
    vec3_t lookat = {0, 0, 20 };
    vec3_t up = {0, 1, 0 };
    Camera_SetLookAt( &gameLocal.camera, &eye, &lookat, &up );
    
    gameLocal.rotation = 0;
}

/*=======================================================================================================================================*/
void Game_Finalise(void) {

}

/*=======================================================================================================================================*/
void Game_Think( float timeStep ) {
    gameLocal.rotation += 45 * (1.0f / 60.0f );
    while (gameLocal.rotation > 360.0f ) gameLocal.rotation -= 360.0f;
    
    ecs_think_params_t thinkParams = { timeStep, NULL };
    gameLocal.thinkParams = thinkParams;
    
    Ecs_SystemThink( gameLocal.previewSystem, &thinkParams );
    Ecs_SystemThink( gameLocal.transformSystem, &thinkParams );
    
    /* This needs to be called so that the ECS can clean up
       messages and do some housekeeping
     */
    Ecs_EndFrame();
}

/*=======================================================================================================================================*/
void Game_Draw( float timeStep ) {
    uint32_t dispW, dispH;
    float aspect;
    mat4_t modelMat;
    
    Render_GetDisplaySize( &dispW, &dispH );
    aspect = (float) dispW / (float) dispH;
    
    Camera_SetShape( &gameLocal.camera, 80.0f, aspect, 5, 1000 );
    Camera_UpdateMatrices( &gameLocal.camera );
    
    vec3_t axis = {0, 1, 0};
    _Mat4_SetRotationAA( &modelMat, &axis, scalar_DegToRad( gameLocal.rotation ) );
    Vec4_Set( modelMat.rows[3], 0, 0, 20, 1 );
    
    int32_t viewport[] = {0, 0, (int32_t)dispW, (int32_t) dispH };
    
    Render_Begin( &gameLocal.camera, viewport );
    //Render_SubmitModel( gameLocal.shipModel, gameLocal.shipMaterial, &modelMat );
    Ecs_SystemThink( gameLocal.shipModelSystem, &gameLocal.thinkParams );
    Render_End();
}

