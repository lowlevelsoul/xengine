/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Render_D3D12.h"
#include "xe/Mem.h"

/*==========================================================================================================*/
X_API render_gpu_buffer_t* Render_GpuBufferCreate( size_t size, uint64_t usage ) {
    if ( !renderLocal.memoryAllocator || !size || ( usage & ~( uint64_t )RENDER_GPU_BUFFER_USAGE_GPU_ONLY ) ) {
        Sys_PrintCat( "ERROR", "Cannot create D3D12 buffer: invalid size, usage, or renderer unavailable." );
        return NULL;
    }

    render_gpu_buffer_t* buffer = ( render_gpu_buffer_t* )Mem_Alloc( sizeof( *buffer ) );
    if ( !buffer ) {
        Sys_PrintCat( "ERROR", "Failed to allocate D3D12 buffer metadata." );
        return NULL;
    }

    Mem_Zero( buffer, sizeof( *buffer ) );
    buffer->size = size;
    buffer->gpuOnly = ( usage & RENDER_GPU_BUFFER_USAGE_GPU_ONLY ) != 0;
    buffer->state = buffer->gpuOnly ? D3D12_RESOURCE_STATE_COMMON : D3D12_RESOURCE_STATE_GENERIC_READ;

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = buffer->gpuOnly ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    HRESULT result = renderLocal.memoryAllocator->CreateResource( &allocationDesc, &resourceDesc, buffer->state, NULL,
                                                                  &buffer->allocation, IID_NULL, NULL );
    if ( FAILED( result ) ) {
        Sys_PrintCat( "ERROR", "Failed to create D3D12 buffer (HRESULT 0x%08lx).", ( unsigned long )result );
        Mem_Free( buffer );
        return NULL;
    }

    buffer->resource = buffer->allocation->GetResource();
    ++renderLocal.liveBufferCount;
    return buffer;
}

/*==========================================================================================================*/
X_API void Render_GpuBufferDestroy( render_gpu_buffer_t* buffer ) {
    if ( !buffer ) {
        return;
    }

    if ( !Render_WaitForBufferUse() ) {
        return;
    }

    Render_GpuBufferDestroyNow( buffer );
}

/*==========================================================================================================*/
void Render_GpuBufferDestroyNow( render_gpu_buffer_t* buffer ) {
    if ( buffer->mappedMemory ) {
        buffer->resource->Unmap( 0, NULL );
    }

   Render_GpuBufferDestroy( buffer );
   
    --renderLocal.liveBufferCount;
}

/*==========================================================================================================*/
X_API void* Render_GpuBuffer_Map( render_gpu_buffer_t* buffer ) {
    if ( !buffer || buffer->gpuOnly ) {
        Sys_PrintCat( "ERROR", "Cannot map a null or GPU-only D3D12 buffer." );
        return NULL;
    }

    if ( buffer->mapCount == UINT32_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 buffer map count exceeded." );
        return NULL;
    }

    if ( !buffer->mapCount ) {
        D3D12_RANGE readRange = { 0, 0 };
        HRESULT result = buffer->resource->Map( 0, &readRange, &buffer->mappedMemory );
        if ( FAILED( result ) ) {
            Sys_PrintCat( "ERROR", "Failed to map D3D12 buffer (HRESULT 0x%08lx).", ( unsigned long )result );
            return NULL;
        }
    }

    ++buffer->mapCount;
    return buffer->mappedMemory;
}

/*==========================================================================================================*/
X_API void Render_GpuBuffer_Unmap( render_gpu_buffer_t* buffer ) {
    if ( !buffer || !buffer->mapCount ) {
        Sys_PrintCat( "ERROR", "Cannot unmap a null or unmapped D3D12 buffer." );
        return;
    }

    --buffer->mapCount;
    if ( !buffer->mapCount ) {
        buffer->resource->Unmap( 0, NULL );
        buffer->mappedMemory = NULL;
    }
}

/*==========================================================================================================*/
X_API void Render_GpuBuffer_QueueWrite( render_gpu_buffer_t* buffer, render_gpu_buffer_t* srcBuffer, size_t offset,
                                         bool releaseSrcBuffer ) {

    Sys_AssertMsg( buffer && srcBuffer, "Cannot queue a write with a null source or destination buffer." );
    Sys_AssertMsg( offset < buffer->size, "Offset is out of bounds for the destination buffer." );
    Sys_AssertMsg( offset + srcBuffer->size <= buffer->size, "Source buffer size exceeds destination buffer bounds." );

    buffer_write_request_t request = {
        .dstBuffer = buffer,
        .srcBuffer = srcBuffer,
        .offset = offset
    };

    request.canDisposeSrcBuffer = releaseSrcBuffer ? 1 : 0;

    if ( !renderLocal.bufferWriteQueue.Write( request ) ) {
        Sys_PrintCat( "ERROR", "Failed to write to buffer write queue." );
    }
}