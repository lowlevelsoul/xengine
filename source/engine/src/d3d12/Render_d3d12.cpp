/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Render_D3D12.h"
#include "xe/RenderBatch.h"
#include "xe/Mem.h"

render_context_d3d12_t renderLocal;
static bool isContextCreated = false;

static bool Render_CreateSwapchain( render_params_t* params );
static void Render_DestroySwapchain( void );
static bool Render_WaitForFence( UINT64 value );
static void Render_CollectBufferWrites( UINT64 completed );

/*==========================================================================================================*/
void Render_Initialise( render_params_t* params ) {
    if ( isContextCreated ) {
        return;
    }

    Mem_Zero( &renderLocal, sizeof( renderLocal ) );

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

    renderLocal.bufferWriteCompleteItems = renderLocal.bufferWriteCompleteItemsPool[ 0 ];
    renderLocal.bufferWriteCompleteItemsTmp = renderLocal.bufferWriteCompleteItemsPool[ 1 ];
}

/*==========================================================================================================*/
void Render_SwapRenderCompleteItems( void ) {
    render_buffer_write_complete_t* tmp = renderLocal.bufferWriteCompleteItems;
    renderLocal.bufferWriteCompleteItems = renderLocal.bufferWriteCompleteItemsTmp;
    renderLocal.bufferWriteCompleteItemsTmp = tmp;
}

/*==========================================================================================================*/
void Render_Finalise( void ) {
    Render_DestroySwapchain();
    if ( !isContextCreated ) {
        RenderWindow_Destroy();
    }
}

/*==========================================================================================================*/
static void Render_DestroySwapchain( void ) {
    if ( !Render_WaitForBufferUse() ) {
        return;
    }

    Render_CollectBufferWrites( renderLocal.lastFenceValue );
    if ( renderLocal.liveBufferCount ) {
        Sys_PrintCat( "ERROR", "Cannot finalise D3D12 renderer with %u live buffers.", renderLocal.liveBufferCount );
        return;
    }

    if ( renderLocal.commandList ) {
        renderLocal.commandList->Release();
    }

    for ( UINT i = 0; i < renderLocal.frameCount; ++i ) {
        if ( renderLocal.frames[ i ].allocator ) {
            renderLocal.frames[ i ].allocator->Release();
        }
    }

    for ( UINT i = 0; i < renderLocal.bufferCount; ++i ) {
        if ( renderLocal.backBuffers[ i ] ) {
            renderLocal.backBuffers[ i ]->Release();
        }
    }

    if ( renderLocal.rtvHeap ) {
        renderLocal.rtvHeap->Release();
    }

    if ( renderLocal.fence ) {
        renderLocal.fence->Release();
    }

    if ( renderLocal.fenceEvent ) {
        CloseHandle( renderLocal.fenceEvent );
    }

    if ( renderLocal.frameLatencyEvent ) {
        CloseHandle( renderLocal.frameLatencyEvent );
    }

    if ( renderLocal.swapChain ) {
        renderLocal.swapChain->Release();
    }

    if ( renderLocal.commandQueue ) {
        renderLocal.commandQueue->Release();
    }

    if ( renderLocal.memoryAllocator ) {
        renderLocal.memoryAllocator->Release();
    }

    if ( renderLocal.device ) {
        renderLocal.device->Release();
    }

    memset( &renderLocal, 0, sizeof( renderLocal ) );
    isContextCreated = false;
}

/*==========================================================================================================*/
bool Render_WaitForBufferUse( void ) {
    if ( renderLocal.pendingSignalValue ) {
        HRESULT result = renderLocal.commandQueue->Signal( renderLocal.fence, renderLocal.pendingSignalValue );
        if ( FAILED( result ) ) {
            Sys_PrintCat( "ERROR", "Failed to signal D3D12 fence while waiting for buffer use (HRESULT 0x%08lx).",
                          ( unsigned long )result );
            return false;
        }

        renderLocal.lastFenceValue = renderLocal.pendingSignalValue;
        renderLocal.pendingSignalValue = 0;
    }

    if ( renderLocal.lastFenceValue ) {
        if ( !Render_WaitForFence( renderLocal.lastFenceValue ) ) {
            return false;
        }
    }

    return true;
}

/*==========================================================================================================*/
static bool Render_WaitForFence( UINT64 value ) {
    UINT64 completed = renderLocal.fence->GetCompletedValue();
    if ( completed == UINT64_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 device removed while waiting for the GPU (HRESULT 0x%08lx).",
                      ( unsigned long )renderLocal.device->GetDeviceRemovedReason() );
        return false;
    }

    if ( completed >= value ) {
        return true;
    }

    HRESULT result = renderLocal.fence->SetEventOnCompletion( value, renderLocal.fenceEvent );
    if ( FAILED( result ) ) {
        Sys_PrintCat( "ERROR", "Failed to wait for D3D12 fence (HRESULT 0x%08lx).", ( unsigned long )result );
        return false;
    }

    DWORD waitResult = WaitForSingleObject( renderLocal.fenceEvent, INFINITE );
    if ( waitResult != WAIT_OBJECT_0 ) {
        Sys_PrintCat( "ERROR", "Failed to wait for D3D12 fence event (result %lu, error %lu).",
                      ( unsigned long )waitResult, ( unsigned long )GetLastError() );
        return false;
    }

    if ( renderLocal.fence->GetCompletedValue() == UINT64_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 device removed while waiting for the GPU (HRESULT 0x%08lx).",
                      ( unsigned long )renderLocal.device->GetDeviceRemovedReason() );
        return false;
    }

    return true;
}

/*==========================================================================================================*/
static bool Render_CreateSwapchain( render_params_t* params ) {
    IDXGIFactory4* factory = NULL;
    IDXGIAdapter* adapter = NULL;
    IDXGISwapChain1* swapChain = NULL;
    HWND window = (HWND) RenderWindow_GetNativeHandle();
    HRESULT result;
    const char* operation = "get render window";
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    D3D12_CPU_DESCRIPTOR_HANDLE rtv = {};

    if ( !window ) {
        Sys_PrintCat( "ERROR", "Failed to get the native render window." );
        return false;
    }

    operation = "create DXGI factory";
    result = CreateDXGIFactory1( IID_PPV_ARGS( &factory ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "create D3D12 device";
    result = D3D12CreateDevice( NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &renderLocal.device ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "get D3D12 adapter";
    result = factory->EnumAdapterByLuid( renderLocal.device->GetAdapterLuid(), IID_PPV_ARGS( &adapter ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    allocatorDesc.Flags = D3D12MA_RECOMMENDED_ALLOCATOR_FLAGS;
    allocatorDesc.pDevice = renderLocal.device;
    allocatorDesc.pAdapter = adapter;
    operation = "create D3D12 memory allocator";
    result = D3D12MA::CreateAllocator( &allocatorDesc, &renderLocal.memoryAllocator );
    adapter->Release();
    adapter = NULL;
    if ( FAILED( result ) ) {
        goto fail;
    }

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    operation = "create D3D12 command queue";
    result = renderLocal.device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &renderLocal.commandQueue ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    desc.Width = ( UINT )params->width;
    desc.Height = ( UINT )params->height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = params->maxNumFramesInFlight < 2 ? 2 : params->maxNumFramesInFlight;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    operation = "create DXGI swapchain";
    result = factory->CreateSwapChainForHwnd( renderLocal.commandQueue, window, &desc, NULL, NULL, &swapChain );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "query DXGI swapchain";
    result = swapChain->QueryInterface( IID_PPV_ARGS( &renderLocal.swapChain ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "set DXGI maximum frame latency";
    result = renderLocal.swapChain->SetMaximumFrameLatency( params->maxNumFramesInFlight );
    if ( FAILED( result ) ) {
        goto fail;
    }

    renderLocal.bufferCount = desc.BufferCount;
    renderLocal.frameCount = params->maxNumFramesInFlight;
    renderLocal.vsync = params->vsync;
    renderLocal.frameLatencyEvent = renderLocal.swapChain->GetFrameLatencyWaitableObject();
    if ( !renderLocal.frameLatencyEvent ) {
        Sys_PrintCat( "ERROR", "Failed to get DXGI frame latency event (error %lu).", ( unsigned long )GetLastError() );
        goto fail_without_hresult;
    }

    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = renderLocal.bufferCount;
    operation = "create render target descriptor heap";
    result = renderLocal.device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &renderLocal.rtvHeap ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    renderLocal.rtvDescriptorSize =
        renderLocal.device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    rtv = renderLocal.rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for ( UINT i = 0; i < renderLocal.bufferCount; ++i ) {
        operation = "get swapchain back buffer";
        result = renderLocal.swapChain->GetBuffer( i, IID_PPV_ARGS( &renderLocal.backBuffers[ i ] ) );
        if ( FAILED( result ) ) {
            goto fail;
        }

        renderLocal.device->CreateRenderTargetView( renderLocal.backBuffers[ i ], NULL, rtv );
        rtv.ptr += renderLocal.rtvDescriptorSize;
    }

    for ( UINT i = 0; i < renderLocal.frameCount; ++i ) {
        operation = "create command allocator";
        result = renderLocal.device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                             IID_PPV_ARGS( &renderLocal.frames[ i ].allocator ) );
        if ( FAILED( result ) ) {
            goto fail;
        }
    }

    operation = "create command list";
    result =
        renderLocal.device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, renderLocal.frames[ 0 ].allocator,
                                               NULL, IID_PPV_ARGS( &renderLocal.commandList ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "close initial command list";
    result = renderLocal.commandList->Close();
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "create D3D12 fence";
    result = renderLocal.device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &renderLocal.fence ) );
    if ( FAILED( result ) ) {
        goto fail;
    }

    renderLocal.fenceEvent = CreateEventW( NULL, FALSE, FALSE, NULL );
    if ( !renderLocal.fenceEvent ) {
        Sys_PrintCat( "ERROR", "Failed to create D3D12 fence event (error %lu).", ( unsigned long )GetLastError() );
        goto fail_without_hresult;
    }

    swapChain->Release();
    factory->Release();
    isContextCreated = true;
    return true;

fail:
    Sys_PrintCat( "ERROR", "Failed to %s (HRESULT 0x%08lx).", operation, ( unsigned long )result );
fail_without_hresult:
    if ( adapter ) {
        adapter->Release();
    }

    if ( swapChain ) {
        swapChain->Release();
    }

    if ( factory ) {
        factory->Release();
    }

    Render_DestroySwapchain();
    return false;
}

/*==========================================================================================================*/
static void Render_RecordScene( UINT bufferIndex ) {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderLocal.backBuffers[ bufferIndex ];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    renderLocal.commandList->ResourceBarrier( 1, &barrier );

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = renderLocal.rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtv.ptr += ( SIZE_T )bufferIndex * renderLocal.rtvDescriptorSize;
    const float clearColor[ 4 ] = { 1.0f, 0.0f, 0.0f, 1.0f };
    renderLocal.commandList->ClearRenderTargetView( rtv, clearColor, 0, NULL );

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    renderLocal.commandList->ResourceBarrier( 1, &barrier );
}

/*==========================================================================================================*/
static void Render_TransitionBuffer( render_gpu_buffer_t* buffer, D3D12_RESOURCE_STATES state ) {
    if ( buffer->state == state || ( state != D3D12_RESOURCE_STATE_COMMON && ( buffer->state & state ) == state ) ) {
        return;
    }

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = buffer->resource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = buffer->state;
    barrier.Transition.StateAfter = state;
    renderLocal.commandList->ResourceBarrier( 1, &barrier );
    buffer->state = state;
}

/*==========================================================================================================*/
static void Render_ProcessBufferWriteRequests(void) {
    if ( BufferWriteQueue_IsEmpty( &renderLocal.bufferWriteQueue ) == true ) {
        return;
    }

    bool haveWriteRequest = false;
    buffer_write_request_t request;
    do {
        haveWriteRequest = BufferWriteQueue_Read( &renderLocal.bufferWriteQueue, &request );
        if ( haveWriteRequest == true ) {            
            renderLocal.commandList->CopyBufferRegion( request.dstBuffer->resource, request.offset,
                                                     request.srcBuffer->resource, 0, request.srcBuffer->size );

            Render_TransitionBuffer( request.dstBuffer, D3D12_RESOURCE_STATE_COPY_DEST );

            if ( request.canDisposeSrcBuffer ) {
                /* Add the buffer to the list of buffers to be discarded */
                Sys_AssertMsg( renderLocal.bufferWriteCompleteCount < MAX_NUM_BUFFER_WRITE_REQUESTS, "Buffer write complete array overflow." );

                render_buffer_write_complete_t completeItem;
                completeItem.buffer = request.srcBuffer;
                completeItem.fenceValue = renderLocal.lastFenceValue;

                renderLocal.bufferWriteCompleteItems[ renderLocal.bufferWriteCompleteCount ] = completeItem;
                renderLocal.bufferWriteCompleteCount++;
            }            
        }

    } while ( haveWriteRequest == true );
}

/*==========================================================================================================*/
static void Render_ProcessCompletedBufferWrites( UINT64 completed ) {
    UINT remaining = 0;
    for ( UINT i = 0; i < renderLocal.bufferWriteCompleteCount; ++i ) {
        render_buffer_write_complete_t item = renderLocal.bufferWriteCompleteItems[ i ];
        if ( item.fenceValue <= completed ) {
            Render_GpuBufferDestroyNow( item.buffer );
        } else {
            renderLocal.bufferWriteCompleteItemsTmp[ remaining++ ] = item;
        }
    }

    Render_SwapRenderCompleteItems();
    renderLocal.bufferWriteCompleteCount = remaining;
}

/*==========================================================================================================*/
void Render_SubmitScene( batch_cmd_scene_t* scene ) {
    if ( !isContextCreated || renderLocal.failed || !scene ) {
        Sys_PrintCat( "ERROR", "Cannot submit scene: renderer unavailable or scene is null." );
        return;
    }

    UINT64 completed = renderLocal.fence->GetCompletedValue();
    if ( completed == UINT64_MAX ) {
        Sys_PrintCat( "ERROR", "D3D12 device removed while processing buffer writes (HRESULT 0x%08lx).",
                      ( unsigned long )renderLocal.device->GetDeviceRemovedReason() );
        renderLocal.failed = true;
        return;
    }

    DWORD waitResult = WaitForSingleObject( renderLocal.frameLatencyEvent, INFINITE );
    if ( waitResult != WAIT_OBJECT_0 ) {
        Sys_PrintCat( "ERROR", "Failed to wait for DXGI frame latency event (result %lu, error %lu).",
                      ( unsigned long )waitResult, ( unsigned long )GetLastError() );
        renderLocal.failed = true;
        return;
    }

    render_frame_d3d12_t* frame = &renderLocal.frames[ renderLocal.nextFrame ];
    if ( frame->fenceValue && !Render_WaitForFence( frame->fenceValue ) ) {
        renderLocal.failed = true;
        return;
    }

    const char* operation = "reset command allocator";
    HRESULT result = frame->allocator->Reset();
    HRESULT presentResult = S_OK;
    UINT64 fenceValue = 0;
    ID3D12CommandList* lists[] = { renderLocal.commandList };
    if ( FAILED( result ) ) {
        goto fail;
    }

    operation = "reset command list";
    result = renderLocal.commandList->Reset( frame->allocator, NULL );
    if ( FAILED( result ) ) {
        goto fail;
    }

    /* Process any pending buffer write requests before recording the scene */
    if ( BufferWriteQueue_IsEmpty( &renderLocal.bufferWriteQueue ) == false ) {
        Render_ProcessBufferWriteRequests();
    }

    /* Process any completed buffer writes before recording the scene */
    if ( renderLocal.bufferWriteCompleteCount > 0 ) {
        Render_ProcessCompletedBufferWrites( renderLocal.fence->GetCompletedValue() );
    }

    Render_RecordScene( renderLocal.swapChain->GetCurrentBackBufferIndex() );

    /** TOODO: Perform rendering here. */

    operation = "close command list";
    result = renderLocal.commandList->Close();
    if ( FAILED( result ) ) {
        goto fail;
    }    

    renderLocal.commandQueue->ExecuteCommandLists( 1, lists );
    presentResult = renderLocal.swapChain->Present( renderLocal.vsync ? 1 : 0, 0 );
    operation = "signal frame fence";
    renderLocal.pendingSignalValue = fenceValue;
    result = renderLocal.commandQueue->Signal( renderLocal.fence, fenceValue );
    if ( FAILED( result ) ) {
        goto fail;
    }

    frame->fenceValue = fenceValue;
    renderLocal.lastFenceValue = fenceValue;
    renderLocal.pendingSignalValue = 0;
    renderLocal.nextFrame = ( renderLocal.nextFrame + 1 ) % renderLocal.frameCount;
    if ( FAILED( presentResult ) ) {
        operation = "present swapchain";
        result = presentResult;
        goto fail;
    }

    return;

fail:
    Sys_PrintCat( "ERROR", "Failed to %s (HRESULT 0x%08lx).", operation, ( unsigned long )result );
    renderLocal.failed = true;
}