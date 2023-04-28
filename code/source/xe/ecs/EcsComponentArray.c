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

#include "ecs/EcsComponentArray.h"
#include "core/Sys.h"
#include "mem/Mem.h"
#include <assert.h>
#include <string.h>

typedef struct ecs_array_data_s {
    const char *    name;                       /* Name of the component struct / array */
    uintptr_t       componentData;              /* Address of the chunk of memory that we allocate for all of the components */
    size_t          componentDataSize;          /* Total size of the memory for components */
    size_t          componentSize;              /* Size of a single component (rounded up to 16 bytes */
    size_t          componentCapacity;          /* Maximum number of components */
    void **         componentPointers;          /* Array of pointers to the data for each component in the array */
    
    ecs_component_index_t * entityComponentMap;     /* Quick look-up of component indices by entity id */
    ecs_entity_t *          componentEntityMap;     /* Quick look-up of entity id by component index */
    size_t                  componentCount;         /* Number of active components */

} ecs_array_data_t;

/*=======================================================================================================================================*/
void EcsComponentArray_Create( ecs_component_array_t * self_, size_t capacity, size_t componentSize, const char * name ) {
    static_assert( sizeof(ecs_array_data_t) <= sizeof(ecs_component_array_t), "Size of ecs_component_array_t.data is too small for implementation" );
    
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    memset( data, 0, sizeof( *data ) );
    
    data->name                  = name;
    data->componentSize         = Sys_Align( componentSize, ECS_COMPONENT_SIZE_ALIGN );
    data->componentDataSize     = data->componentSize * capacity;
    data->componentCapacity     = capacity;
    data->componentData         = (uintptr_t) Mem_Alloc( data->componentDataSize );
    data->componentPointers     = (void**) Mem_Alloc( sizeof(void*) * capacity );
    data->entityComponentMap    = (ecs_component_index_t *) Mem_Alloc( sizeof(ecs_component_index_t) * ECS_MAX_ENTITIES );
    data->componentEntityMap    = (ecs_entity_t *) Mem_Alloc( sizeof(ecs_entity_t) * capacity );
    data->componentCount         = 0;
    
    for ( int n = 0; n < ECS_MAX_ENTITIES; ++n ) {
        data->entityComponentMap[ n ] = ECS_COMPONENT_INVALID;

    }
    
    uintptr_t addrr = data->componentData;
    for ( int n = 0; n < data->componentCapacity; ++n ) {
        data->componentEntityMap[n] = ECS_ENTITY_NULL;
        data->componentPointers[ n ] = ( void * ) addrr;
        addrr += data->componentSize;
    }
}

/*=======================================================================================================================================*/
const char * EcsComponentArray_GetName( const ecs_component_array_t * self_ ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    return data->name;
}

/*=======================================================================================================================================*/
void EcsComponentArray_Destroy( ecs_component_array_t * self_ ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
}

/*=======================================================================================================================================*/
void * EcsComponentArray_AddEntity( ecs_component_array_t * self_, ecs_entity_t ent ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    
    xassert( ent >= 0 && ent < ECS_MAX_ENTITIES );
    xassert( data->componentCount < data->componentCapacity );
    
    /* Cache */
    ecs_component_index_t componentIndex = ( ecs_component_index_t ) data->componentCount;
    ++data->componentCount;
    
    /* Set the component / entity mapping indices */
    data->entityComponentMap[ ent ] = componentIndex;
    data->componentEntityMap[ componentIndex ] = ent;
    
    return data->componentPointers[ componentIndex ];
}

/*=======================================================================================================================================*/
void EcsComponentArray_RemoveEntity( ecs_component_array_t * self_, ecs_entity_t ent ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    
    xassert( ent >= 0 && ent < ECS_MAX_ENTITIES );
    xassert( data->componentCount > 0 );
    xassert( data->entityComponentMap[ ent ] != -1 );
    
    /* Grab the index of the component of the entit that we're removing */
    ecs_component_index_t componentIndex = data->entityComponentMap[ ent ];
    
    if ( data->componentCount > 1 && componentIndex < data->componentCount ) {
        /* If we have more than one component active ( before removal )and it's not at
           the end of the array of data, then we just move the component for the entity
           at the end of the list down to the compinent that we're removing
        */
        ecs_entity_t lastEnt = data->componentEntityMap[ data->componentCount ];
        
        void * oldComponentMem = data->componentPointers[ data->componentSize - 1];
        void * newComponentMem = data->componentPointers[ componentIndex ];
        
        /* Copy component from last entry to the slot that */
        memcpy(newComponentMem, oldComponentMem, data->componentSize );
        
        data->entityComponentMap[ lastEnt ] = componentIndex;
        data->componentEntityMap[ componentIndex ] = lastEnt;
    }
    else {
        /* Either a single component or the component is at the end of the data array */
        data->entityComponentMap[ ent ] = -1;
        data->componentEntityMap[ componentIndex ] = - 1;
    }
    
    --data->componentCount;
}

/*=======================================================================================================================================*/
void * EcsComponentArray_GetComponentForEntity( ecs_component_array_t * self_, ecs_entity_t ent ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    
    xassert( ent != ECS_ENTITY_NULL );
    xassert( ent < ECS_MAX_ENTITIES );
    
    ecs_component_index_t componentIndex = data->entityComponentMap[ ent ];
    xassert ( componentIndex  != ECS_COMPONENT_INVALID );
    
    return data->componentPointers[ componentIndex ];
}

/*=======================================================================================================================================*/
bool_t EcsComponentArray_HasEntity( const ecs_component_array_t * self_, ecs_entity_t ent ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    
    xassert( ent != ECS_ENTITY_NULL );
    xassert( ent < ECS_MAX_ENTITIES );
    
    return ( data->entityComponentMap[ ent ] != ECS_COMPONENT_INVALID ) ? true : false;
}

/*=======================================================================================================================================*/
void EcsComponentArray_GetActiveComponents( ecs_component_array_t * self_, ecs_component_list_t * activeListOut ) {
    ecs_array_data_t * data = (ecs_array_data_t *) self_;
    
    if ( data->componentCount > 0 ) {
        activeListOut->componentData = data->componentPointers;
        activeListOut->entities = data->componentEntityMap;
        activeListOut->count = data->componentCount;
    }
    else {
        memset( activeListOut, 0,   sizeof( ecs_component_list_t ) );
    }
}
