
/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "Gpu_metal.h"
#include "xe/Mem.h"
#include <stdio.h>

/*==========================================================================================================*/
X_API gpu_buffer_t* Gpu_BufferCreate( void ) {
    gpu_buffer_t* buffer = Mem_Alloc( sizeof( *buffer ) );
    if ( !buffer ) {
        fprintf( stderr, "Gpu_BufferCreate: allocation failed\n" );
        return NULL;
    }

    Mem_Zero( buffer, sizeof( *buffer ) );
    atomic_init( &buffer->pendingWrites, 0 );
    return buffer;
}

/*==========================================================================================================*/
X_API void Gpu_BufferDestroy( gpu_buffer_t* buffer ) {
    if ( !buffer ) {
        return;
    }

    gpu_render_context_t* context = gpu.currentContext;
    if ( context ) {
        Sys_MuxLock( &context->mutex );
        if ( buffer->pendingWrites ) {
            Gpu_Metal_Fatal( "Cannot destroy a buffer with pending uploads" );
        }

        if ( buffer->allocation ) {
            Gpu_Metal_ReleaseAllocationLocked( context, buffer->allocation );
            --context->liveBuffers;
            buffer->allocation = NULL;
        }

        Sys_MuxUnlock( &context->mutex );
    }

    buffer->buffer = nil;
    Mem_Free( buffer );
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Initialise( gpu_buffer_t* buffer, uint32_t size, uint32_t usage ) {
    gpu_render_context_t* context = gpu.currentContext;
    if ( !buffer || !context || !size || usage > GPU_BUFFER_USAGE_GPU_ONLY ) {
        fprintf( stderr, "Gpu_Buffer_Initialise: invalid buffer, device, size, usage, or mapped state\n" );
        return;
    }

    Sys_MuxLock( &context->mutex );
    if ( buffer->pendingWrites ) {
        Gpu_Metal_Fatal( "Cannot reinitialize a buffer with pending uploads" );
    }

    if ( buffer->mapped ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_Buffer_Initialise: buffer is mapped\n" );
        return;
    }

    MTLResourceOptions options =
        usage == GPU_BUFFER_USAGE_GPU_ONLY ? MTLResourceStorageModePrivate : MTLResourceStorageModeShared;
    gpu_allocation_t* allocation = Gpu_Metal_AllocateBufferLocked( context, size, options );
    if ( buffer->allocation ) {
        Gpu_Metal_ReleaseAllocationLocked( context, buffer->allocation );
    } else {
        ++context->liveBuffers;
    }

    buffer->allocation = allocation;
    buffer->buffer = allocation->buffer;
    buffer->size = size;
    Sys_MuxUnlock( &context->mutex );
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Write( gpu_device_t* device, gpu_buffer_t* buffer, const void* data, uint32_t offset,
                             uint32_t size ) {
    ( void )device;
    gpu_render_context_t* context = gpu.currentContext;
    if ( !buffer || !context ) {
        fprintf( stderr, "Gpu_Buffer_Write: invalid buffer or context\n" );
        return;
    }

    Sys_MuxLock( &context->mutex );
    if ( !buffer->buffer || buffer->mapped || offset > buffer->size || size > buffer->size - offset ||
         ( size && !data ) ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_Buffer_Write: invalid buffer, data, range, or mapped state\n" );
        return;
    }

    if ( !size ) {
        Sys_MuxUnlock( &context->mutex );
        return;
    }

    if ( buffer->buffer.storageMode != MTLStorageModePrivate ) {
        void* contents = [buffer->buffer contents];
        if ( !contents ) {
            Sys_MuxUnlock( &context->mutex );
            fprintf( stderr, "Gpu_Buffer_Write: Metal buffer has no CPU-accessible contents\n" );
            return;
        }

        memcpy( ( char* )contents + offset, data, size );
        Sys_MuxUnlock( &context->mutex );
        return;
    }

    if ( context->writeCount == GPU_MAX_PENDING_WRITES ) {
        Gpu_Metal_Fatal( "Metal upload queue is full" );
    }

    gpu_allocation_t* staging = Gpu_Metal_AllocateBufferLocked( context, size, MTLResourceStorageModeShared );
    memcpy( [staging->buffer contents], data, size );
    ++buffer->pendingWrites;
    ++buffer->allocation->references;
    context->writes[ context->writeCount++ ] =
        ( gpu_pending_write_t ){ buffer, buffer->allocation, staging, offset, size };
    Sys_MuxUnlock( &context->mutex );
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Map( gpu_device_t* device, gpu_buffer_t* buffer, void** data ) {
    ( void )device;
    if ( !data ) {
        fprintf( stderr, "Gpu_Buffer_Map: output pointer is NULL\n" );
        return;
    }

    *data = NULL;
    gpu_render_context_t* context = gpu.currentContext;
    if ( !buffer || !context ) {
        fprintf( stderr, "Gpu_Buffer_Map: invalid buffer or context\n" );
        return;
    }

    Sys_MuxLock( &context->mutex );
    if ( !buffer->buffer || buffer->mapped || buffer->buffer.storageMode == MTLStorageModePrivate ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_Buffer_Map: buffer is invalid, already mapped, or GPU-only\n" );
        return;
    }

    *data = [buffer->buffer contents];
    if ( !*data ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_Buffer_Map: Metal buffer has no CPU-accessible contents\n" );
        return;
    }

    buffer->mapped = true;
    Sys_MuxUnlock( &context->mutex );
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Unmap( gpu_device_t* device, gpu_buffer_t* buffer ) {
    ( void )device;
    gpu_render_context_t* context = gpu.currentContext;
    if ( !buffer || !context ) {
        fprintf( stderr, "Gpu_Buffer_Unmap: invalid buffer or context\n" );
        return;
    }

    Sys_MuxLock( &context->mutex );
    if ( !buffer->buffer || !buffer->mapped ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_Buffer_Unmap: buffer is not mapped\n" );
        return;
    }

    buffer->mapped = false;
    Sys_MuxUnlock( &context->mutex );
}
