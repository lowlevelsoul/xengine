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

#ifndef __ECSTYPES_H__
#define __ECSTYPES_H__

#include "core/Common.h"

#define ECS_MAX_ENTITIES 2048
#define ECS_MAX_SYSTEMS 128
#define ECS_MAX_COMPONENT_TYPES 128
#define ECS_MAX_MESSAGES 128

#define ECS_ENTITY_NULL -1
#define ECS_COMPONENT_INVALID -1
#define ECS_COMPONENT_SIZE_ALIGN 16

#define ECS_BLUEPRINT_MAX_COMPONENTS 16
#define ECS_MAX_NUM_MESSAGES 256
#define ECS_MESSAGE_DATA_SIZE 1024 * 2

typedef int64_t ecs_entity_t;
typedef int64_t ecs_component_index_t;
typedef int64_t ecs_system_id_t;

typedef struct ecs_entity_blueprint_s {
    void * components[ECS_BLUEPRINT_MAX_COMPONENTS];
    size_t componentCount;
} ecs_entity_blueprint_s;

typedef struct ecs_msg_s {
    uint16_t        msg;            /* Message type */
    uint16_t        seen : 1;       /* Flag to tell us that this message has been seen */
    uint16_t        complete : 1;   /* Set if the message is complete and can be used */
    uint32_t        size;           /* Size of the message header */
} ecs_msg_t;

typedef struct ecs_think_params_s {
    float       timeStep;           /* Time in seconds since the last think was called */
    void *      context;            /* User context */
} ecs_think_params_t;

typedef struct ecs_system_s {
    const char * desc;
    void (*constructDefault)( ecs_entity_t ent, void * component );
    void (*destruct)( void * component );
    void (*think)( ecs_entity_t ent, void * component, ecs_think_params_t * params );
    void (*message)( ecs_msg_t * msg, void * componentPtr );
} ecs_system_t;



#endif 
