/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "xe/Sys.h"
#define COBJMACROS
#include <d3d12.h>
#include <dxgi1_6.h>

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );
extern HWND RenderWindow_GetNativeHandle( void );

typedef struct render_frame_d3d12_s {
    ID3D12CommandAllocator* allocator;
    UINT64 fenceValue;
} render_frame_d3d12_t;

typedef struct render_context_d3d12_s {
    ID3D12Device* device;
    ID3D12CommandQueue* commandQueue;
    IDXGISwapChain3* swapChain;
    ID3D12DescriptorHeap* rtvHeap;
    ID3D12GraphicsCommandList* commandList;
    ID3D12Fence* fence;
    ID3D12Resource* backBuffers[ DXGI_MAX_SWAP_CHAIN_BUFFERS ];
    render_frame_d3d12_t frames[ DXGI_MAX_SWAP_CHAIN_BUFFERS ];
    HANDLE fenceEvent;
    HANDLE frameLatencyEvent;
    UINT rtvDescriptorSize;
    UINT bufferCount;
    UINT frameCount;
    UINT nextFrame;
    UINT64 lastFenceValue;
    bool vsync;
    bool failed;
} render_context_d3d12_t;

static render_context_d3d12_t theContext;
static bool isContextCreated = false;

static bool Render_CreateSwapchain( render_params_t* params );
static void Render_DestroySwapchain( void );
static bool Render_WaitForFence( UINT64 value );

/*==========================================================================================================*/
void Render_Initialise( render_params_t* params ) {
    if ( isContextCreated ) {
        return;
    }

    if ( !params || params->width <= 0 || params->height <= 0 || params->maxNumFramesInFlight < 1 ||
         params->maxNumFramesInFlight > DXGI_MAX_SWAP_CHAIN_BUFFERS ) {
        Sys_PrintCat( "ERROR", "Invalid render dimensions or maxNumFramesInFlight (must be 1 to %u).",
                      DXGI_MAX_SWAP_CHAIN_BUFFERS );
        return;
    }

    RenderWindow_Create( params );
    if ( !Render_CreateSwapchain( params ) ) {
        RenderWindow_Destroy();
    }
}

/*==========================================================================================================*/
void Render_Finalise( void ) {
    Render_DestroySwapchain();
    RenderWindow_Destroy();
}

/*==========================================================================================================*/
static void Render_DestroySwapchain( void ) {
    if ( theContext.lastFenceValue ) {
        Render_WaitForFence( theContext.lastFenceValue );
    }

    if ( theContext.commandList ) {
        ID3D12GraphicsCommandList_Release( theContext.commandList );
    }

    for ( UINT i = 0; i < theContext.frameCount; ++i ) {
        if ( theContext.frames[ i ].allocator ) {
            ID3D12CommandAllocator_Release( theContext.frames[ i ].allocator );
        }
    }

    for ( UINT i = 0; i < theContext.bufferCount; ++i ) {
        if ( theContext.backBuffers[ i ] ) {
            ID3D12Resource_Release( theContext.backBuffers[ i ] );
        }
    }

    if ( theContext.rtvHeap ) {
        ID3D12DescriptorHeap_Release( theContext.rtvHeap );
    }

    if ( theContext.fence ) {
        ID3D12Fence_Release( theContext.fence );
    }

    if ( theContext.fenceEvent ) {
        CloseHandle( theContext.fenceEvent );
    }

    if ( theContext.frameLatencyEvent ) {
        CloseHandle( theContext.frameLatencyEvent );
    }

    if ( theContext.swapChain ) {
        IDXGISwapChain3_Release( theContext.swapChain );
    }

    if ( theContext.commandQueue ) {
        ID3D12CommandQueue_Release( theContext.commandQueue );
    }

    if ( theContext.device ) {
        ID3D12Device_Release( theContext.device );
    }

    memset( &theContext, 0, sizeof( theContext ) );
    isContextCreated = false;
}

/*==========================================================================================================*/
static bool Render_WaitForFence( UINT64 value ) {
    UINT64 completed = ID3D12Fence_GetCompletedValue( theContext.fence );
    if ( completed == UINT64_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 device removed while waiting for the GPU (HRESULT 0x%08lx).",
                      ( unsigned long )ID3D12Device_GetDeviceRemovedReason( theContext.device ) );
        return false;
    }

    if ( completed >= value ) {
        return true;
    }

    HRESULT result = ID3D12Fence_SetEventOnCompletion( theContext.fence, value, theContext.fenceEvent );
    if ( FAILED( result ) ) {
        Sys_PrintCat( "ERROR", "Failed to wait for D3D12 fence (HRESULT 0x%08lx).", ( unsigned long )result );
        return false;
    }

    DWORD waitResult = WaitForSingleObject( theContext.fenceEvent, INFINITE );
    if ( waitResult != WAIT_OBJECT_0 ) {
        Sys_PrintCat( "ERROR", "Failed to wait for D3D12 fence event (result %lu, error %lu).",
                      ( unsigned long )waitResult, ( unsigned long )GetLastError() );
        return false;
    }

    if ( ID3D12Fence_GetCompletedValue( theContext.fence ) == UINT64_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 device removed while waiting for the GPU (HRESULT 0x%08lx).",
                      ( unsigned long )ID3D12Device_GetDeviceRemovedReason( theContext.device ) );
        return false;
    }

    return true;
}

/*==========================================================================================================*/
static bool Render_CreateSwapchain( render_params_t* params ) {
    IDXGIFactory4* factory = NULL;
    IDXGISwapChain1* swapChain = NULL;
    HWND window = RenderWindow_GetNativeHandle();
    HRESULT result;
    const char* operation = "get render window";

    if ( !window ) {
        Sys_PrintCat( "ERROR", "Failed to get the native render window." );
        return false;
    }

    operation = "create DXGI factory";
    result = CreateDXGIFactory1( &IID_IDXGIFactory4, ( void** )&factory );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "create D3D12 device";
    result = D3D12CreateDevice( NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, ( void** )&theContext.device );
    if ( FAILED( result ) ) {
        goto fail;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = { 0 };
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    operation = "create D3D12 command queue";
    result = ID3D12Device_CreateCommandQueue( theContext.device, &queueDesc, &IID_ID3D12CommandQueue,
                                              ( void** )&theContext.commandQueue );
    if ( FAILED( result ) ) {
        goto fail;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
    desc.Width = ( UINT )params->width;
    desc.Height = ( UINT )params->height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = params->maxNumFramesInFlight < 2 ? 2 : params->maxNumFramesInFlight;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    operation = "create DXGI swapchain";
    result = IDXGIFactory4_CreateSwapChainForHwnd( factory, ( IUnknown* )theContext.commandQueue, window, &desc, NULL,
                                                   NULL, &swapChain );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "query DXGI swapchain";
    result = IDXGISwapChain1_QueryInterface( swapChain, &IID_IDXGISwapChain3, ( void** )&theContext.swapChain );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "set DXGI maximum frame latency";
    result = IDXGISwapChain3_SetMaximumFrameLatency( theContext.swapChain, params->maxNumFramesInFlight );
    if ( FAILED( result ) ) {
        goto fail;
    }

    theContext.bufferCount = desc.BufferCount;
    theContext.frameCount = params->maxNumFramesInFlight;
    theContext.vsync = params->vsync;
    theContext.frameLatencyEvent = IDXGISwapChain3_GetFrameLatencyWaitableObject( theContext.swapChain );
    if ( !theContext.frameLatencyEvent ) {
        Sys_PrintCat( "ERROR", "Failed to get DXGI frame latency event (error %lu).", ( unsigned long )GetLastError() );
        goto fail_without_hresult;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { 0 };
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = theContext.bufferCount;
    operation = "create render target descriptor heap";
    result = ID3D12Device_CreateDescriptorHeap( theContext.device, &heapDesc, &IID_ID3D12DescriptorHeap,
                                                ( void** )&theContext.rtvHeap );
    if ( FAILED( result ) ) {
        goto fail;
    }

    theContext.rtvDescriptorSize =
        ID3D12Device_GetDescriptorHandleIncrementSize( theContext.device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    D3D12_CPU_DESCRIPTOR_HANDLE rtv;
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart( theContext.rtvHeap, &rtv );
    for ( UINT i = 0; i < theContext.bufferCount; ++i ) {
        operation = "get swapchain back buffer";
        result = IDXGISwapChain3_GetBuffer( theContext.swapChain, i, &IID_ID3D12Resource,
                                            ( void** )&theContext.backBuffers[ i ] );
        if ( FAILED( result ) ) {
            goto fail;
        }

        ID3D12Device_CreateRenderTargetView( theContext.device, theContext.backBuffers[ i ], NULL, rtv );
        rtv.ptr += theContext.rtvDescriptorSize;
    }

    for ( UINT i = 0; i < theContext.frameCount; ++i ) {
        operation = "create command allocator";
        result = ID3D12Device_CreateCommandAllocator( theContext.device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      &IID_ID3D12CommandAllocator,
                                                      ( void** )&theContext.frames[ i ].allocator );
        if ( FAILED( result ) ) {
            goto fail;
        }
    }

    operation = "create command list";
    result = ID3D12Device_CreateCommandList( theContext.device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                             theContext.frames[ 0 ].allocator, NULL, &IID_ID3D12GraphicsCommandList,
                                             ( void** )&theContext.commandList );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "close initial command list";
    result = ID3D12GraphicsCommandList_Close( theContext.commandList );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "create D3D12 fence";
    result = ID3D12Device_CreateFence( theContext.device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence,
                                       ( void** )&theContext.fence );
    if ( FAILED( result ) ) {
        goto fail;
    }

    theContext.fenceEvent = CreateEventW( NULL, FALSE, FALSE, NULL );
    if ( !theContext.fenceEvent ) {
        Sys_PrintCat( "ERROR", "Failed to create D3D12 fence event (error %lu).", ( unsigned long )GetLastError() );
        goto fail_without_hresult;
    }

    IDXGISwapChain1_Release( swapChain );
    IDXGIFactory4_Release( factory );
    isContextCreated = true;
    return true;

fail:
    Sys_PrintCat( "ERROR", "Failed to %s (HRESULT 0x%08lx).", operation, ( unsigned long )result );
fail_without_hresult:
    if ( swapChain ) {
        IDXGISwapChain1_Release( swapChain );
    }

    if ( factory ) {
        IDXGIFactory4_Release( factory );
    }

    Render_DestroySwapchain();
    return false;
}

/*==========================================================================================================*/
void Render_SubmitScene( batch_cmd_scene_t* scene ) {
    if ( !isContextCreated || theContext.failed || !scene ) {
        Sys_PrintCat( "ERROR", "Cannot submit scene: renderer unavailable or scene is null." );
        return;
    }

    DWORD waitResult = WaitForSingleObject( theContext.frameLatencyEvent, INFINITE );
    if ( waitResult != WAIT_OBJECT_0 ) {
        Sys_PrintCat( "ERROR", "Failed to wait for DXGI frame latency event (result %lu, error %lu).",
                      ( unsigned long )waitResult, ( unsigned long )GetLastError() );
        theContext.failed = true;
        return;
    }

    render_frame_d3d12_t* frame = &theContext.frames[ theContext.nextFrame ];
    if ( frame->fenceValue && !Render_WaitForFence( frame->fenceValue ) ) {
        theContext.failed = true;
        return;
    }

    const char* operation = "reset command allocator";
    HRESULT result = ID3D12CommandAllocator_Reset( frame->allocator );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "reset command list";
    result = ID3D12GraphicsCommandList_Reset( theContext.commandList, frame->allocator, NULL );
    if ( FAILED( result ) ) {
        goto fail;
    }

    UINT bufferIndex = IDXGISwapChain3_GetCurrentBackBufferIndex( theContext.swapChain );
    D3D12_RESOURCE_BARRIER barrier = { 0 };
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = theContext.backBuffers[ bufferIndex ];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    ID3D12GraphicsCommandList_ResourceBarrier( theContext.commandList, 1, &barrier );

    D3D12_CPU_DESCRIPTOR_HANDLE rtv;
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart( theContext.rtvHeap, &rtv );
    rtv.ptr += ( SIZE_T )bufferIndex * theContext.rtvDescriptorSize;
    const float clearColor[ 4 ] = { 1.0f, 0.0f, 0.0f, 1.0f };
    ID3D12GraphicsCommandList_ClearRenderTargetView( theContext.commandList, rtv, clearColor, 0, NULL );

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier( theContext.commandList, 1, &barrier );

    operation = "close command list";
    result = ID3D12GraphicsCommandList_Close( theContext.commandList );
    if ( FAILED( result ) ) {
        goto fail;
    }

    ID3D12CommandList* lists[] = { ( ID3D12CommandList* )theContext.commandList };
    ID3D12CommandQueue_ExecuteCommandLists( theContext.commandQueue, 1, lists );
    HRESULT presentResult = IDXGISwapChain3_Present( theContext.swapChain, theContext.vsync ? 1 : 0, 0 );
    operation = "signal frame fence";
    UINT64 fenceValue = theContext.lastFenceValue + 1;
    result = ID3D12CommandQueue_Signal( theContext.commandQueue, theContext.fence, fenceValue );
    if ( FAILED( result ) ) {
        goto fail;
    }

    frame->fenceValue = fenceValue;
    theContext.lastFenceValue = fenceValue;
    theContext.nextFrame = ( theContext.nextFrame + 1 ) % theContext.frameCount;
    if ( FAILED( presentResult ) ) {
        operation = "present swapchain";
        result = presentResult;
        goto fail;
    }

    return;

fail:
    Sys_PrintCat( "ERROR", "Failed to %s (HRESULT 0x%08lx).", operation, ( unsigned long )result );
    theContext.failed = true;
}