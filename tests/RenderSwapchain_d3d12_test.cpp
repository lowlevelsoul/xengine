/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include <assert.h>
#include <stdio.h>
#include "xe/RenderBatch.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../source/engine/src/d3d12/Render_d3d12.cpp"

static HWND testWindow;
static unsigned int errorCount;
static bool failNativeHandle;

static void Test_Pixels() {
    UINT index = renderLocal.swapChain->GetCurrentBackBufferIndex();
    ID3D12Resource* buffer = renderLocal.backBuffers[ index ];
    ID3D12Resource* readback = NULL;
    ID3D12CommandAllocator* allocator = NULL;
    ID3D12GraphicsCommandList* list = NULL;
    ID3D12Fence* fence = NULL;
    HANDLE event = CreateEventW( NULL, FALSE, FALSE, NULL );
    assert( event );

    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = D3D12_HEAP_TYPE_READBACK;
    heap.CreationNodeMask = 1;
    heap.VisibleNodeMask = 1;
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    const UINT width = 320;
    const UINT height = 240;
    const UINT rowPitch =
        ( width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1 ) & ~( D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1 );
    desc.Width = ( UINT64 )rowPitch * height;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    assert( SUCCEEDED( renderLocal.device->CreateCommittedResource(
        &heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS( &readback ) ) ) );
    assert( SUCCEEDED(
        renderLocal.device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &allocator ) ) ) );
    assert( SUCCEEDED( renderLocal.device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL,
                                                              IID_PPV_ARGS( &list ) ) ) );
    assert( SUCCEEDED( renderLocal.frames[ 0 ].allocator->Reset() ) );
    assert( SUCCEEDED( renderLocal.commandList->Reset( renderLocal.frames[ 0 ].allocator, NULL ) ) );
    Render_RecordScene( index );
    assert( SUCCEEDED( renderLocal.commandList->Close() ) );
    ID3D12CommandList* renderLists[] = { renderLocal.commandList };
    renderLocal.commandQueue->ExecuteCommandLists( 1, renderLists );

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = buffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    list->ResourceBarrier( 1, &barrier );

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = buffer;
    src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = readback;
    dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dst.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dst.PlacedFootprint.Footprint.Width = width;
    dst.PlacedFootprint.Footprint.Height = height;
    dst.PlacedFootprint.Footprint.Depth = 1;
    dst.PlacedFootprint.Footprint.RowPitch = rowPitch;
    list->CopyTextureRegion( &dst, 0, 0, 0, &src, NULL );

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    list->ResourceBarrier( 1, &barrier );
    assert( SUCCEEDED( list->Close() ) );
    assert( SUCCEEDED( renderLocal.device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &fence ) ) ) );
    ID3D12CommandList* lists[] = { list };
    renderLocal.commandQueue->ExecuteCommandLists( 1, lists );
    assert( SUCCEEDED( renderLocal.commandQueue->Signal( fence, 1 ) ) );
    assert( SUCCEEDED( fence->SetEventOnCompletion( 1, event ) ) );
    assert( WaitForSingleObject( event, 10000 ) == WAIT_OBJECT_0 );
    assert( fence->GetCompletedValue() == 1 );

    unsigned char* pixels = NULL;
    D3D12_RANGE readRange = { 0, ( SIZE_T )rowPitch * height };
    assert( SUCCEEDED( readback->Map( 0, &readRange, ( void** )&pixels ) ) );
    for ( UINT y = 0; y < height; ++y ) {
        for ( UINT x = 0; x < width; ++x ) {
            const unsigned char* pixel = pixels + y * rowPitch + x * 4;
            assert( pixel[ 0 ] == 255 && pixel[ 1 ] == 0 && pixel[ 2 ] == 0 && pixel[ 3 ] == 255 );
        }
    }

    D3D12_RANGE writtenRange = { 0, 0 };
    readback->Unmap( 0, &writtenRange );
    fence->Release();
    list->Release();
    allocator->Release();
    readback->Release();
    CloseHandle( event );
}

void Sys_PrintCat( const char* cat, const char* fmt, ... ) {
    ( void )cat;
    ( void )fmt;
    ++errorCount;
}

void RenderWindow_Create( render_params_t* params ) {
    testWindow = CreateWindowExW( 0, L"STATIC", L"Swapchain test", WS_OVERLAPPEDWINDOW, 0, 0, params->width,
                                  params->height, NULL, NULL, GetModuleHandleW( NULL ), NULL );
    assert( testWindow );
}

void RenderWindow_Destroy( void ) {
    if ( testWindow ) {
        DestroyWindow( testWindow );
        testWindow = NULL;
    }
}

HWND RenderWindow_GetNativeHandle( void ) {
    return failNativeHandle ? NULL : testWindow;
}

static void Test_BufferWrites() {
    render_gpu_buffer_t* src = Render_GpuBufferCreate( 4, 0 );
    render_gpu_buffer_t* dst = Render_GpuBufferCreate( 12, RENDER_GPU_BUFFER_USAGE_GPU_ONLY );
    render_gpu_buffer_t* dst2 = Render_GpuBufferCreate( 12, RENDER_GPU_BUFFER_USAGE_GPU_ONLY );
    assert( src && dst && dst2 );
    unsigned char* data = ( unsigned char* )Render_GpuBuffer_Map( src );
    assert( data );
    data[ 0 ] = 11;
    data[ 1 ] = 22;
    data[ 2 ] = 33;
    data[ 3 ] = 44;
    Render_GpuBuffer_Unmap( src );

    batch_cmd_buffer_write_t third = { dst, dst2, 0, false, NULL };
    batch_cmd_buffer_write_t second = { src, dst, 8, true, &third };
    batch_cmd_buffer_write_t first = { src, dst, 0, true, &second };
    batch_cmd_scene3d_t scene3d = {};
    scene3d.bufferWriteListHead = &first;
    batch_cmd_scene_t scene = {};
    scene.scene3dListHead = &scene3d;

    second.offset = 9;
    Render_SubmitScene( &scene );
    assert( errorCount == 5 && renderLocal.lastFenceValue == 0 && renderLocal.bufferWriteCompleteCount == 0 );
    second.offset = 8;
    Render_SubmitScene( &scene );
    assert( !renderLocal.failed && renderLocal.lastFenceValue == 1 && renderLocal.bufferWriteCompleteCount == 1 );
    Render_CollectBufferWrites( 0 );
    assert( renderLocal.bufferWriteCompleteCount == 1 );
    assert( Render_WaitForFence( 1 ) );

    batch_cmd_buffer_write_t nextWrite = { dst2, dst, 0, false, NULL };
    scene3d.bufferWriteListHead = &nextWrite;
    Render_SubmitScene( &scene );
    assert( !renderLocal.failed && renderLocal.lastFenceValue == 2 && renderLocal.bufferWriteCompleteCount == 0 );
    assert( Render_WaitForFence( 2 ) );

    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = D3D12_HEAP_TYPE_READBACK;
    D3D12_RESOURCE_DESC desc = dst->resource->GetDesc();
    ID3D12Resource* readback = NULL;
    assert( SUCCEEDED( renderLocal.device->CreateCommittedResource(
        &heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS( &readback ) ) ) );
    ID3D12CommandAllocator* allocator = NULL;
    ID3D12GraphicsCommandList* list = NULL;
    assert( SUCCEEDED(
        renderLocal.device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &allocator ) ) ) );
    assert( SUCCEEDED( renderLocal.device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL,
                                                              IID_PPV_ARGS( &list ) ) ) );
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = dst->resource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = dst->state;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    list->ResourceBarrier( 1, &barrier );
    list->CopyBufferRegion( readback, 0, dst->resource, 0, 12 );
    assert( SUCCEEDED( list->Close() ) );
    ID3D12CommandList* lists[] = { list };
    renderLocal.commandQueue->ExecuteCommandLists( 1, lists );
    assert( SUCCEEDED( renderLocal.commandQueue->Signal( renderLocal.fence, 3 ) ) );
    assert( Render_WaitForFence( 3 ) );

    unsigned char* copied = NULL;
    D3D12_RANGE readRange = { 0, 12 };
    assert( SUCCEEDED( readback->Map( 0, &readRange, ( void** )&copied ) ) );
    const unsigned char expected[] = { 11, 22, 33, 44 };
    for ( UINT i = 0; i < 4; ++i ) {
        assert( copied[ i ] == expected[ i ] && copied[ i + 8 ] == expected[ i ] );
    }

    D3D12_RANGE writtenRange = { 0, 0 };
    readback->Unmap( 0, &writtenRange );
    readback->Release();
    list->Release();
    allocator->Release();
    assert( renderLocal.bufferWriteCompleteCount == 0 );
    Render_GpuBufferDestroy( dst );
    Render_GpuBufferDestroy( dst2 );
    renderLocal.lastFenceValue = 3;
}

static void Test_BufferMapping() {
    unsigned int errors = errorCount;
    render_gpu_buffer_t* upload = Render_GpuBufferCreate( 4, 0 );
    render_gpu_buffer_t* gpu = Render_GpuBufferCreate( 4, RENDER_GPU_BUFFER_USAGE_GPU_ONLY );
    assert( upload && gpu );

    unsigned char* first = ( unsigned char* )Render_GpuBuffer_Map( upload );
    unsigned char* second = ( unsigned char* )Render_GpuBuffer_Map( upload );
    assert( first && first == second && upload->mapCount == 2 );
    first[ 0 ] = 42;
    Render_GpuBuffer_Unmap( upload );
    assert( upload->mapCount == 1 && second[ 0 ] == 42 );
    Render_GpuBuffer_Unmap( upload );
    assert( upload->mapCount == 0 && !upload->mappedMemory );

    first = ( unsigned char* )Render_GpuBuffer_Map( upload );
    assert( first && first[ 0 ] == 42 && upload->mapCount == 1 );
    upload->mapCount = UINT32_MAX;
    assert( !Render_GpuBuffer_Map( upload ) && upload->mapCount == UINT32_MAX );
    upload->mapCount = 1;
    Render_GpuBuffer_Unmap( upload );
    Render_GpuBuffer_Unmap( upload );
    Render_GpuBuffer_Unmap( NULL );
    assert( !Render_GpuBuffer_Map( gpu ) && !Render_GpuBuffer_Map( NULL ) );
    assert( errorCount == errors + 5 && upload->mapCount == 0 && gpu->mapCount == 0 );

    assert( Render_GpuBuffer_Map( upload ) );
    Render_GpuBufferDestroy( upload );
    Render_GpuBufferDestroy( gpu );
}

static void TestFrameCount( uint32_t count ) {
    render_params_t params = { 0 };
    params.width = 320;
    params.height = 240;
    params.maxNumFramesInFlight = count;
    params.vsync = ( count % 2 ) != 0;
    Render_Initialise( &params );
    assert( isContextCreated && renderLocal.memoryAllocator );

    DXGI_SWAP_CHAIN_DESC1 desc;
    UINT latency = 0;
    assert( SUCCEEDED( renderLocal.swapChain->GetDesc1( &desc ) ) );
    assert( SUCCEEDED( renderLocal.swapChain->GetMaximumFrameLatency( &latency ) ) );
    assert( desc.BufferCount == ( count < 2 ? 2 : count ) );
    assert( desc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT );
    assert( latency == count );

    assert( renderLocal.vsync == params.vsync );
    Test_Pixels();
    batch_cmd_scene_t scene = {};
    UINT presentCountBefore = 0;
    UINT presentCountAfter = 0;
    assert( SUCCEEDED( renderLocal.swapChain->GetLastPresentCount( &presentCountBefore ) ) );
    for ( uint32_t i = 0; i < count + 2; ++i ) {
        Render_SubmitScene( &scene );
        assert( !renderLocal.failed );
    }

    assert( renderLocal.lastFenceValue == count + 2 );
    assert( SUCCEEDED( renderLocal.swapChain->GetLastPresentCount( &presentCountAfter ) ) );
    assert( presentCountAfter == presentCountBefore + count + 2 );
    Render_Finalise();
    assert( !isContextCreated && !renderLocal.swapChain && !renderLocal.commandQueue && !renderLocal.device &&
            !renderLocal.memoryAllocator );
    assert( !testWindow );
}

int main( void ) {
    render_params_t invalid = {};
    invalid.width = 320;
    invalid.height = 240;
    Render_Initialise( &invalid );
    assert( errorCount == 1 && !testWindow && !isContextCreated );
    invalid.maxNumFramesInFlight = DXGI_MAX_SWAP_CHAIN_BUFFERS + 1;
    Render_Initialise( &invalid );
    assert( errorCount == 2 && !testWindow && !isContextCreated );

    invalid.maxNumFramesInFlight = 2;
    invalid.width = 0;
    Render_Initialise( &invalid );
    assert( errorCount == 3 && !testWindow && !isContextCreated );

    invalid.width = 320;
    failNativeHandle = true;
    Render_Initialise( &invalid );
    assert( errorCount == 4 && !testWindow && !isContextCreated );
    failNativeHandle = false;

    TestFrameCount( 1 );
    TestFrameCount( 2 );
    TestFrameCount( 3 );
    TestFrameCount( DXGI_MAX_SWAP_CHAIN_BUFFERS );
    assert( errorCount == 4 );

    invalid.maxNumFramesInFlight = 2;
    Render_Initialise( &invalid );
    assert( isContextCreated );
    Test_BufferWrites();
    Render_SubmitScene( NULL );
    assert( !renderLocal.failed && errorCount == 6 && renderLocal.lastFenceValue == 3 );

    render_gpu_buffer_t* src = Render_GpuBufferCreate( 4, 0 );
    render_gpu_buffer_t* dst = Render_GpuBufferCreate( 4, RENDER_GPU_BUFFER_USAGE_GPU_ONLY );
    assert( src && dst );
    batch_cmd_buffer_write_t write = { src, dst, 0, true, NULL };
    batch_cmd_scene3d_t scene3d = {};
    scene3d.bufferWriteListHead = &write;
    batch_cmd_scene_t scene = {};
    scene.scene3dListHead = &scene3d;
    Render_SubmitScene( &scene );
    assert( !renderLocal.failed && renderLocal.bufferWriteCompleteCount == 1 );
    Render_Finalise();
    assert( isContextCreated && testWindow && errorCount == 7 && renderLocal.bufferWriteCompleteCount == 0 &&
            renderLocal.liveBufferCount == 1 );
    Render_GpuBufferDestroy( dst );
    Render_Finalise();
    assert( !testWindow && !isContextCreated );

    Render_Initialise( &invalid );
    assert( isContextCreated );
    Test_BufferMapping();
    Render_Finalise();
    assert( !testWindow && !isContextCreated );
    puts( "D3D12 swapchain tests passed." );
    return 0;
}
