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

#include "CompTransform.h"
#include "CompMessages.h"

static void Construct( ecs_entity_t ent, void * component );
static void Destroy( void * component );
static void Think( ecs_entity_t ent, void * component, ecs_think_params_t * params );
static void Transform_Message( ecs_msg_t * msg, void * componentPtr );

ecs_system_t sys_transform = {
    "Transform System", Construct, Destroy, Think, Transform_Message
};

/*=======================================================================================================================================*/
void Construct( ecs_entity_t ent, void * component ) {
    
}

/*=======================================================================================================================================*/
void Destroy( void * component ) {
    
}

/*=======================================================================================================================================*/
void Transform_Message( ecs_msg_t * msg, void * componentPtr  ) {
    comp_transform_t * comp = (comp_transform_t*) componentPtr;
    
    switch( msg->msg ) {
        case MSG_LOC_ROT: {
            comp->location = ((msg_loc_rot_t*) msg)->location;
            comp->rotation = ((msg_loc_rot_t*) msg)->rotation;
            break;
        }
        default:
            break;
    }
}

/*=======================================================================================================================================*/
void Think( ecs_entity_t ent, void * component, ecs_think_params_t * params ) {
    comp_transform_t * comp = (comp_transform_t*) component;
    msg_transform_t msgTransform;
        
    Mat4_SetRotationQ( msgTransform.transform, comp->rotation );
    Mat4_SetTranslationVec3( msgTransform.transform, comp->location );
    
    /* Broadcast the completed transform to the rest of the component */
    Ecs_SendMessage( ent, MSG_TRANSORM, &msgTransform, sizeof( msgTransform ) );
}
