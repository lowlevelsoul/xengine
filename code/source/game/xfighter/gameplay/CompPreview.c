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

#include "CompPreview.h"
#include "CompTransform.h"
#include "CompMessages.h"

static void PreviewConstruct( ecs_entity_t ent, void * component );
static void PreviewDestroy( void * component );
static void PreviewThink( ecs_entity_t ent, void * component, ecs_think_params_t * params );

ecs_system_t sys_preview = {
    "Ship Model System", PreviewConstruct, PreviewDestroy, PreviewThink, NULL
};

/*=======================================================================================================================================*/
void PreviewConstruct( ecs_entity_t ent, void * component ) {
    comp_preview_t * comp = (comp_preview_t*) component;
    
    comp->transformComponent = -666;    
    comp->angle = 0;
    comp->rotSpeed = 90.0f;
    Vec3_SetXyz( comp->location, 0, 0, 40);
}

/*=======================================================================================================================================*/
void PreviewDestroy( void * component ) {
    
}

/*=======================================================================================================================================*/
void PreviewThink( ecs_entity_t ent, void * component, ecs_think_params_t * params ) {
    comp_preview_t * comp = (comp_preview_t*) component;
    msg_loc_rot_t msg;
        
    /* Update spin angle */
    comp->angle += comp->rotSpeed * params->timeStep;
    while ( comp->angle >= 360.0f ) comp->angle -= 360.0f;
    
    /* Prepare the updated rotation Q and location and broadcast to the entity components */
    vec3_t upAxis = {0, 1, 0 };
    _Quat_SetAA( &msg.rotation, &upAxis, scalar_DegToRad( comp->angle ) );
    msg.location = comp->location;
    
    Ecs_SendMessage( ent, MSG_LOC_ROT, &msg, sizeof( msg ) );
}
