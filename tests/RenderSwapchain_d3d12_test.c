/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include <assert.h>
#include <stdio.h>
#include "xe/RenderBatch.h"
#define COBJMACROS
#include <d3d12.h>
#include <dxgi1_6.h>

static HRESULT Test_Present( IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags );
#undef IDXGISwapChain3_Present
#define IDXGISwapChain3_Present( swapChain, syncInterval, flags ) Test_Present( swapChain, syncInterval, flags )
#include "../source/engine/src/d3d12/Render_d3d12.c"

static HWND testWindow;
static unsigned int errorCount;
static unsigned int presentCount;
static bool failNativeHandle;
static bool checkPixels;
static bool failPresent;
static UINT expectedSyncInterval;

static HRESULT Test_Present( IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags ) {
    assert( syncInterval == expectedSyncInterval && flags == 0 );
    ++presentCount;
    if ( failPresent ) {
        return DXGI_ERROR_DEVICE_RESET;
    }

    if ( !checkPixels ) {
        return swapChain->lpVtbl->Present( swapChain, syncInterval, flags );
    }

    checkPixels = false;
    UINT index = IDXGISwapChain3_GetCurrentBackBufferIndex( swapChain );
    ID3D12Resource* buffer = theContext.backBuffers[ index ];
    ID3D12Resource* readback = NULL;
    ID3D12CommandAllocator* allocator = NULL;
    ID3D12GraphicsCommandList* list = NULL;
    ID3D12Fence* fence = NULL;
    HANDLE event = CreateEventW( NULL, FALSE, FALSE, NULL );
    assert( event );

    D3D12_HEAP_PROPERTIES heap = { 0 };
    heap.Type = D3D12_HEAP_TYPE_READBACK;
    heap.CreationNodeMask = 1;
    heap.VisibleNodeMask = 1;
    D3D12_RESOURCE_DESC desc = { 0 };
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
    assert( SUCCEEDED( ID3D12Device_CreateCommittedResource( theContext.device, &heap, D3D12_HEAP_FLAG_NONE, &desc,
                                                             D3D12_RESOURCE_STATE_COPY_DEST, NULL, &IID_ID3D12Resource,
                                                             ( void** )&readback ) ) );
    assert( SUCCEEDED( ID3D12Device_CreateCommandAllocator( theContext.device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                            &IID_ID3D12CommandAllocator, ( void** )&allocator ) ) );
    assert( SUCCEEDED( ID3D12Device_CreateCommandList( theContext.device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator,
                                                       NULL, &IID_ID3D12GraphicsCommandList, ( void** )&list ) ) );

    D3D12_RESOURCE_BARRIER barrier = { 0 };
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = buffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    ID3D12GraphicsCommandList_ResourceBarrier( list, 1, &barrier );

    D3D12_TEXTURE_COPY_LOCATION src = { 0 };
    src.pResource = buffer;
    src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    D3D12_TEXTURE_COPY_LOCATION dst = { 0 };
    dst.pResource = readback;
    dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dst.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dst.PlacedFootprint.Footprint.Width = width;
    dst.PlacedFootprint.Footprint.Height = height;
    dst.PlacedFootprint.Footprint.Depth = 1;
    dst.PlacedFootprint.Footprint.RowPitch = rowPitch;
    ID3D12GraphicsCommandList_CopyTextureRegion( list, &dst, 0, 0, 0, &src, NULL );

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier( list, 1, &barrier );
    assert( SUCCEEDED( ID3D12GraphicsCommandList_Close( list ) ) );
    assert( SUCCEEDED(
        ID3D12Device_CreateFence( theContext.device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, ( void** )&fence ) ) );
    ID3D12CommandList* lists[] = { ( ID3D12CommandList* )list };
    ID3D12CommandQueue_ExecuteCommandLists( theContext.commandQueue, 1, lists );
    assert( SUCCEEDED( ID3D12CommandQueue_Signal( theContext.commandQueue, fence, 1 ) ) );
    assert( SUCCEEDED( ID3D12Fence_SetEventOnCompletion( fence, 1, event ) ) );
    assert( WaitForSingleObject( event, 10000 ) == WAIT_OBJECT_0 );
    assert( ID3D12Fence_GetCompletedValue( fence ) == 1 );

    unsigned char* pixels = NULL;
    D3D12_RANGE readRange = { 0, ( SIZE_T )rowPitch * height };
    assert( SUCCEEDED( ID3D12Resource_Map( readback, 0, &readRange, ( void** )&pixels ) ) );
    for ( UINT y = 0; y < height; ++y ) {
        for ( UINT x = 0; x < width; ++x ) {
            const unsigned char* pixel = pixels + y * rowPitch + x * 4;
            assert( pixel[ 0 ] == 255 && pixel[ 1 ] == 0 && pixel[ 2 ] == 0 && pixel[ 3 ] == 255 );
        }
    }

    D3D12_RANGE writtenRange = { 0, 0 };
    ID3D12Resource_Unmap( readback, 0, &writtenRange );
    ID3D12Fence_Release( fence );
    ID3D12GraphicsCommandList_Release( list );
    ID3D12CommandAllocator_Release( allocator );
    ID3D12Resource_Release( readback );
    CloseHandle( event );

    return swapChain->lpVtbl->Present( swapChain, syncInterval, flags );
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

static void TestFrameCount( uint32_t count ) {
    render_params_t params = { 0 };
    params.width = 320;
    params.height = 240;
    params.maxNumFramesInFlight = count;
    params.vsync = ( count % 2 ) != 0;
    expectedSyncInterval = params.vsync ? 1 : 0;
    Render_Initialise( &params );
    assert( isContextCreated );

    DXGI_SWAP_CHAIN_DESC1 desc;
    UINT latency = 0;
    assert( SUCCEEDED( IDXGISwapChain3_GetDesc1( theContext.swapChain, &desc ) ) );
    assert( SUCCEEDED( IDXGISwapChain3_GetMaximumFrameLatency( theContext.swapChain, &latency ) ) );
    assert( desc.BufferCount == ( count < 2 ? 2 : count ) );
    assert( desc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT );
    assert( latency == count );

    batch_cmd_scene_t scene = { 0 };
    unsigned int previousPresentCount = presentCount;
    checkPixels = true;
    for ( uint32_t i = 0; i < count + 2; ++i ) {
        Render_SubmitScene( &scene );
        assert( !theContext.failed );
    }

    assert( presentCount == previousPresentCount + count + 2 );
    Render_Finalise();
    assert( !isContextCreated && !theContext.swapChain && !theContext.commandQueue && !theContext.device );
    assert( !testWindow );
}

int main( void ) {
    render_params_t invalid = { 0 };
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
    batch_cmd_scene_t scene = { 0 };
    expectedSyncInterval = 0;
    failPresent = true;
    Render_SubmitScene( &scene );
    assert( theContext.failed && errorCount == 5 && theContext.lastFenceValue == 1 );
    Render_Finalise();
    assert( !testWindow && !isContextCreated );
    puts( "D3D12 swapchain tests passed." );
    return 0;
}
