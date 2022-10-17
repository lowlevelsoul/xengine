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

#include "CompShipModel.h"
#include "CompTransform.h"
#include "CompMessages.h"
#include "render/Render3d.h"

static void ShipModel_Construct( ecs_entity_t ent, void * component );
static void ShipModel_Destroy( void * component );
static void ShipModel_Think( ecs_entity_t ent, void * component, ecs_think_params_t * params );
static void ShipModel_Message( ecs_msg_t * msg, void * componentPtr );

ecs_system_t sys_shipmodel = {
    "Ship Model System",
    ShipModel_Construct,
    ShipModel_Destroy,
    ShipModel_Think,
    ShipModel_Message
};

/*=======================================================================================================================================*/
void ShipModel_Construct( ecs_entity_t ent, void * component ) {
    comp_shipmodel_t * comp = (comp_shipmodel_t*) component;
    
    comp->transformComponent = Ecs_GetEntityComponentIndex( ent, comp_transform_t );
    xassert( comp->transformComponent >= 0 );
    
    comp->model = NULL;
    comp->material = NULL;
}

/*=======================================================================================================================================*/
void ShipModel_Destroy( void * component ) {
    
}

/*=======================================================================================================================================*/
void ShipModel_Message( ecs_msg_t * msg, void * componentPtr ) {
    comp_shipmodel_t* comp = (comp_shipmodel_t*) componentPtr;
    switch( msg->msg ) {
        case MSG_TRANSORM: {
            comp->transform = ((msg_transform_t*) msg)->transform;
            break;
        }
        default:
            break;
    }
}

/*=======================================================================================================================================*/
void ShipModel_Think( ecs_entity_t ent, void * component, ecs_think_params_t * params ) {
    comp_shipmodel_t * comp = (comp_shipmodel_t *) component;
    Render_SubmitModel( comp->model, comp->material, &comp->transform );
}
