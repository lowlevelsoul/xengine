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

#include "ecs/Ecs.h"
#include "ecs/EcsComponentArray.h"
#include "core/Sys.h"
#include "core/Array.h"
#include "core/Bsearch.h"
#include "core/fh64.h"

#include <string.h>
#include <stdatomic.h>

typedef struct ecs_entity_info_s {
    uint32_t        componentList[ ECS_MAX_COMPONENT_TYPES ];       /* Index of the component arrays that the entity was added to */
    uint8_t         messageBuffer[ ECS_MESSAGE_DATA_SIZE ];         /* Messages sent to this entity */
    uint32_t        componentCount;                                 /* Number of components that the entity has */
    atomic_uint     messagePtr;                                     /* Pointer into the message buffer of the current buffer end */
    atomic_uint     messageCommitPtr;                               /* Used to ensure that we only process messesg once they've been commited */
} ecs_entity_info_t;

typedef struct ecs_s {
    /* Entity management */
    ecs_entity_t            entityFreeList[ ECS_MAX_ENTITIES ];
    size_t                  entityFreeCount;
    ecs_entity_info_t       entityInfo[ ECS_MAX_ENTITIES ];
    
    ecs_system_t *          systems[ ECS_MAX_SYSTEMS ];
    ecs_component_index_t   systemComponent[ ECS_MAX_SYSTEMS ];
    size_t                  systemCount;
    
    ecs_component_array_t   components[ ECS_MAX_COMPONENT_TYPES ];
    ecs_system_t *          componentSystems[ ECS_MAX_COMPONENT_TYPES ];
    const char *            componentNames[ ECS_MAX_COMPONENT_TYPES ];
    uint32_t                componentNameMap[ ECS_MAX_COMPONENT_TYPES ];
    uint64_t                componentNameHashes[ ECS_MAX_COMPONENT_TYPES ];
    uint32_t                componentNameHashMap[ ECS_MAX_COMPONENT_TYPES ];
    size_t                  componentCount;
    
    uint8_t                 messageBufferTemp[ ECS_MESSAGE_DATA_SIZE ];
    
    
} ecs_t;

static ecs_t ecs;
static boolean_t ecsInit = false;

/*=======================================================================================================================================*/
void Ecs_Initialise(void) {
    if ( ecsInit == true ) {
        return;
    }
    
    xprintf("=== ECS Init ===================\n");
    
    memset( &ecs, 0, sizeof( ecs ) );
    ecsInit = true;
    
    /* Set up the list of free entity ids */
    for ( int n = 0; n < ECS_MAX_ENTITIES; ++n ) {
        ecs.entityFreeList[ n ] = ECS_MAX_ENTITIES - 1 - n;
        ecs.entityInfo[ n ].componentCount = 0;
        ecs.entityInfo[ n ].messagePtr = 0;
        ecs.entityInfo[ n ].messageCommitPtr = 0;
    }
    
    for ( int n = 0; n < ECS_MAX_SYSTEMS; ++n ) {
        ecs.systems[ n ] = NULL;
        ecs.systemComponent[ n ] = ECS_COMPONENT_INVALID;
    }

    
    ecs.entityFreeCount = ECS_MAX_ENTITIES;
}

/*=======================================================================================================================================*/
void Ecs_Finalise(void) {
    
}

/*=======================================================================================================================================*/
XE_API void Ecs_EndFrame(void) {
    uint32_t msgPtr = 0;
    uint32_t msgDstPtr = 0;
    ecs_msg_t * msg;
    
    for ( uint32_t n = 0; n < ECS_MAX_ENTITIES; ++n ) {
        ecs_entity_info_t * info = &ecs.entityInfo[ n ];
        
        if ( info->messagePtr != 0 ) {
            /* This entity has messages. But are any pending (the seen flag is 0) ?*/
            msgPtr = 0;
            msgDstPtr = 0;
            
            /* Go through the message data and copy any messages that have not been seen
               by the system used by the entity
            */
            while ( msgPtr < info->messagePtr ) {
                msg = (ecs_msg_t*) &info->messageBuffer[ msgPtr ];
                if ( msg->seen == 0 ) {
                    memcpy( ecs.messageBufferTemp+msgDstPtr, msg, msg->size );
                    msgDstPtr += msg->size;
                }
                msgPtr += msg->size;
            }
            
            if ( msgDstPtr != 0 ) {
                /* If we caught any messages that were not seen by the entity
                   copy over the buffer
                   Note: msgDstPtr gives us the size of the message data to copy
                 */
                memcpy( info->messageBuffer, ecs.messageBufferTemp, msgDstPtr );
            }
            
            /* Set the new end pointer for the entity message buffer. This will
               be zero if the entity does not have any messages pending.
             */
            info->messagePtr = msgDstPtr;
            info->messageCommitPtr = msgDstPtr;
        }
    }
}

/*=======================================================================================================================================*/
void Ecs_RegisterNamedComponent( const char * name, size_t capacity, size_t structSize ) {
    xassert( ecs.componentCount < ECS_MAX_COMPONENT_TYPES );
    
    uint32_t componentIndex = ( uint32_t ) ecs.componentCount;
    int32_t insertIndex = -1;
    boolean_t found;
    uint64_t nameHash;
    
    /* Insert the name pointer into a sorted look-up map  */
    found = Bsearch_FindVoidPtr( &insertIndex, (void*) name, (const void**) ecs.componentNames, ecs.componentCount );
    xassert( found == false );

    Array_InsertAtPosVoidPtr( insertIndex, (void *)name, (void**) ecs.componentNames, ecs.componentCount, ECS_MAX_COMPONENT_TYPES );
    Array_InsertAtPosUint32( insertIndex, componentIndex, ecs.componentNameMap, ecs.componentCount, ECS_MAX_COMPONENT_TYPES );
    
    /* Insert the hash of the name into a sorted look-up map */
    nameHash = FH64_CalcFromCStr( name );
    
    found = Bsearch_FindUint64( &insertIndex, nameHash, ecs.componentNameHashes, ecs.componentCount );
    xassert( found == false );
    
    Array_InsertAtPosUint64( insertIndex, nameHash, ecs.componentNameHashes, ecs.componentCount, ECS_MAX_COMPONENT_TYPES );
    Array_InsertAtPosUint32( insertIndex, componentIndex, ecs.componentNameHashMap, ecs.componentCount, ECS_MAX_COMPONENT_TYPES );
    
    /* Create the component array */
    EcsComponentArray_Create( &ecs.components[ componentIndex ], capacity, structSize, name );
    ++ecs.componentCount;
    
    xprintf( "ECS : Registered component array '%s'\n", name );
}

/*=======================================================================================================================================*/
ecs_entity_t Ecs_EntityAlloc(void) {
    xassert( ecs.entityFreeCount > 0 );
    
    ecs_entity_info_t * entInfo = NULL;
    
    ecs_entity_t ent = ecs.entityFreeList[ ecs.entityFreeCount - 1 ];
    --ecs.entityFreeCount;
   
    /* Init the entity info */
    entInfo = &ecs.entityInfo[ ent ];
    
    entInfo->componentCount = 0;
    entInfo->messagePtr = 0;
    entInfo->messageCommitPtr = 0;
    
    return ent;
}

/*=======================================================================================================================================*/
void Ecs_EntityFree( ecs_entity_t ent ) {
    xassert( ecs.entityFreeCount < ECS_MAX_ENTITIES );
    
    ecs_entity_info_t * entInfo = &ecs.entityInfo[ ent ];
    
    /* Loop through any component arrays that have a component aray and free it up */
    size_t compCount = entInfo->componentCount;
    uint32_t * compArray = entInfo->componentList;
    
    for ( int n = 0; n < compCount; ++n ) {
        ecs_component_array_t * compArrayPtr = &ecs.components[ compArray[ n ] ];
        EcsComponentArray_RemoveEntity( compArrayPtr, ent );
    }
    
    /* Add the entity id to the free list */
    ecs.entityFreeList[ ecs.entityFreeCount ] = ent;
    entInfo->componentCount = 0;
    ++ecs.entityFreeCount;
}

/*=======================================================================================================================================*/
void * Ecs_AddComponentInternal( ecs_entity_t ent, ecs_component_array_t * compArray, uint32_t compArrayIndex ) {
    /* Add the component array to the list of components for the entity */
    ecs_entity_info_t * entInfo = &ecs.entityInfo[ ent ];
    xassert( entInfo->componentCount < ECS_MAX_COMPONENT_TYPES );
    
    entInfo->componentList[ entInfo->componentCount ] =  compArrayIndex;
    ++entInfo->componentCount;
    
    /* Add the entity to the component array */
    void * compData = EcsComponentArray_AddEntity( compArray, ent );
    return compData;
}

/*=======================================================================================================================================*/
void * Ecs_AddNamedComponent( ecs_entity_t ent, const char * compName ) {
    int32_t index = -1;
    boolean_t found = Bsearch_FindVoidPtr( &index, (void*) compName, (const void**) ecs.componentNames, ecs.componentCount );
    xassert( found == true );
    
    /* Returned index is actually an index into the name map, which contains the index of the component
       array that we want to use */
    index = (uint32_t) ecs.componentNameMap[ index ];
    
    return Ecs_AddComponentInternal( ent, &ecs.components[ index ], index );
}

/*=======================================================================================================================================*/
void * Ecs_AddHashedNamedComponent( ecs_entity_t ent, const char * compName ) {
    uint64_t hash = FH64_CalcFromCStr( compName );
    int32_t index = -1;
    boolean_t found = Bsearch_FindUint64( &index, hash, ecs.componentNameHashes, ecs.componentCount );
    xassert( found == true );
    
    /* Returned index is actually an index into the name map, which contains the index of the component
       array that we want to use */
    index = (uint32_t) ecs.componentNameMap[ index ];
    
    return Ecs_AddComponentInternal( ent, &ecs.components[ index ], index );
}

/*=======================================================================================================================================*/
int32_t Ecs_GetComponentArrayIndex( const char * name ) {
    int32_t index = -1;
    boolean_t found = Bsearch_FindVoidPtr( &index, (void*) name, (const void**) ecs.componentNames, ecs.componentCount );
    if ( found == false ) {
        return -1;
    }
    
    /* Returned index is actually an index into the name map, which contains the index of the component
       array that we want to use */
    index = (int32_t) ecs.componentNameMap[ index ];
    return index;
}

/*=======================================================================================================================================*/
ecs_system_id_t Ecs_RegisterSystemNamed( ecs_system_t * system, const char * componentName ) {
    int32_t componentArrayIndex = -1;
    xassert( ecs.systemCount < ECS_MAX_SYSTEMS );
    
    componentArrayIndex = Ecs_GetComponentArrayIndex( componentName );
    xassert( componentArrayIndex >= 0 );
    
    xprintf("ECS : Registering system '%s' for component '%s'\n", system->desc, EcsComponentArray_GetName( &ecs.components[ componentArrayIndex ] ) );
    
    ecs_system_id_t systemIndex = ecs.systemCount;
    ++ecs.systemCount;

    ecs.systems[ systemIndex ] = system;
    ecs.systemComponent[ systemIndex ] = componentArrayIndex;
    
    /*
        Set the primary system for the component if :-
        1) There is no primary system registered for the component
        2) The system being registered has a valid construct and destruct callback 
     */
    if ( ecs.componentSystems[ componentArrayIndex ] == NULL && system->constructDefault != NULL && system->destruct != NULL ) {
        ecs.componentSystems[ componentArrayIndex ] = system;
    }
    
    return systemIndex;
}

/*=======================================================================================================================================*/
void Ecs_EntityConstructDefault( ecs_entity_t ent ) {
    uint32_t numComponents = 0;
    uint32_t * componentArrayIndices = NULL;
    
    ecs_entity_info_t * entInfo = &ecs.entityInfo[ ent ];
    if ( entInfo->componentCount == 0 ) {
        return;
    }
    
    numComponents = entInfo->componentCount;
    componentArrayIndices = entInfo->componentList;
    
    for ( int n = 0; n < numComponents; ++n ) {
        uint32_t componentIndex = componentArrayIndices[ n ];
        
        ecs_component_array_t * compArray = &ecs.components[ componentArrayIndices[ n ] ];
        void * data = EcsComponentArray_GetComponentForEntity( compArray, ent);
        if ( ecs.componentSystems[ componentIndex ] != NULL ) {
            ecs.componentSystems[ componentIndex ]->constructDefault( ent, data );
        }
    }
}
 
/*=======================================================================================================================================*/
int32_t Ecs_GetEntityNamedComponentIndex( ecs_entity_t ent, const char * name ) {
    int32_t index = -1;
    ecs_entity_info_t * entInfo = &ecs.entityInfo[ ent ];
    
    xassert( ent != ECS_ENTITY_NULL && ent < ECS_MAX_ENTITIES );

    for ( int n = 0; n < entInfo->componentCount; ++n ) {
        int32_t currIndex = entInfo->componentList[n];
        if ( currIndex >= 0 ) {
            if ( EcsComponentArray_GetName( &ecs.components[currIndex] ) == name ) {
                index = currIndex;
                break;
            }
        }
    }
    
    return index;
}

/*=======================================================================================================================================*/
void * Ecs_GetEntityIndexedComponent( ecs_entity_t ent, int32_t index ) {
    xassert( ent != ECS_ENTITY_NULL && ent < ECS_MAX_ENTITIES );
    return EcsComponentArray_GetComponentForEntity( &ecs.components[ index ], ent );
}

/*=======================================================================================================================================*/
void Ecs_SystemThink( int32_t systemIndex, ecs_think_params_t * params ) {
    xassert( systemIndex >= 0 && systemIndex < ecs.systemCount );
    
    ecs_component_list_t systemComponents;
    ecs_component_array_t * compArray = &ecs.components[ ecs.systemComponent[ systemIndex ] ];
    EcsComponentArray_GetActiveComponents( compArray, &systemComponents );
    ecs_entity_t ent;
    ecs_entity_info_t * entInfo;
    uint32_t msgEnd;
    uintptr_t msgPtr;
    ecs_msg_t * msg;
    
    for ( int n = 0; n < systemComponents.count; ++n ) {
        
        ent = systemComponents.entities[ n ];
        entInfo = &ecs.entityInfo[ ent ];
               
        /* If the system has a message function, we'll send messages for this
           entity to it. */
        if ( ecs.systems[ systemIndex ]->message != NULL ) {
            
            /* Cache the message end pointer - another thread may add messages and it may
               be unsafe to process them at this point.
             */
            msgEnd = atomic_load( &entInfo->messageCommitPtr );
            msgPtr = 0;
            
            while ( msgPtr < msgEnd ) {
                
                /* Get the pointer to the message header and flag. */
                msg = ( ecs_msg_t *) &entInfo->messageBuffer[ msgPtr ];
                msg->seen = 1;
                    
                /* Process the message and update the pointer to the next message */
                ecs.systems[ systemIndex ]->message( msg, systemComponents.componentData[ n ] );
                msgPtr += msg->size;
            }
        }
        
        ecs.systems[ systemIndex ]->think( systemComponents.entities[ n ], systemComponents.componentData[n], params );
    }
}

/*=======================================================================================================================================*/
static boolean_t Ecs_IsMessageCommited( ecs_entity_info_t * info ) {
    return atomic_load( &info->messagePtr ) == atomic_load( &info->messageCommitPtr ) ? true : false;
}

/*=======================================================================================================================================*/
void Ecs_SendMessage( ecs_entity_t ent, uint16_t msgId, const void * data, size_t dataSize ) {
    ecs_entity_info_t * info = &ecs.entityInfo[ ent ];
    
    uint32_t ptr = 0;
    uint32_t newPtr = 0;
    boolean_t loop = true;
    
    do {
        /* Wait for any current messages being written to commit */
        while ( Ecs_IsMessageCommited( info ) == false ) {
        }
        
        /* Allocate space to write the message. If another thread has
           gotten there before us, we need to go around again*/
        ptr = atomic_load( &info->messagePtr );
        newPtr = ptr + (uint32_t) dataSize;
        xassert( newPtr <= ECS_MESSAGE_DATA_SIZE );
        
        if ( atomic_compare_exchange_strong( &info->messagePtr, &ptr, newPtr ) ) {
            /* We wrote the internal working pointer successfully, so bail from the loop */
            loop = false;
        }
        
    } while( loop == true );
    
    /* Write the message and update the commited memory pointer */
    ecs_msg_t * msg = (ecs_msg_t*)(info->messageBuffer + ptr);
    memcpy( msg, data, dataSize );
    
    /* Need to fill in the header info */
    msg->msg  = msgId;
    msg->size = dataSize;
    msg->seen = 0;
    
    atomic_store( &info->messageCommitPtr, newPtr );
}
