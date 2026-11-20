/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef RENDER_D3D12_H
#define RENDER_D3D12_H

#include "xe/Render.h"
#include "xe/Sys.h"
#include "D3D12MemAlloc.h"
#include <d3d12.h>
#include <dxgi1_6.h>

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );
extern HWND RenderWindow_GetNativeHandle( void );

#define MAX_NUM_BUFFER_COMPLETE_ITEMS 1024

typedef struct render_frame_d3d12_s {
    ID3D12CommandAllocator* allocator;
    UINT64 fenceValue;
} render_frame_d3d12_t;

typedef struct render_buffer_write_complete_s {
    render_gpu_buffer_t* srcBuffer; /* Source buffer - can be CPU or GPU */
    UINT64 fenceValue;              /* Fence value indicating when the write is complete */
} render_buffer_write_complete_t;

typedef struct render_context_d3d12_s {
    ID3D12Device* device;
    D3D12MA::Allocator* memoryAllocator;
    ID3D12CommandQueue* commandQueue;
    IDXGISwapChain3* swapChain;
    ID3D12DescriptorHeap* rtvHeap;
    ID3D12GraphicsCommandList* commandList;
    ID3D12Fence* fence;
    ID3D12Resource* backBuffers[ DXGI_MAX_SWAP_CHAIN_BUFFERS ];
    render_frame_d3d12_t frames[ DXGI_MAX_SWAP_CHAIN_BUFFERS ];

    render_buffer_write_complete_t bufferWriteCompleteItems[ MAX_NUM_BUFFER_COMPLETE_ITEMS ];
    UINT bufferWriteCompleteCount;
    UINT liveBufferCount;

    HANDLE fenceEvent;
    HANDLE frameLatencyEvent;
    UINT rtvDescriptorSize;
    UINT bufferCount;
    UINT frameCount;
    UINT nextFrame;
    UINT64 lastFenceValue;
    UINT64 pendingSignalValue;
    bool vsync;
    bool failed;
} render_context_d3d12_t;

typedef struct render_gpu_buffer_s {
    ID3D12Resource* resource;
    D3D12MA::Allocation* allocation;
    D3D12_RESOURCE_STATES state;
    size_t size; /* Size of the buffer in bytes */
    void* mappedMemory;
    uint32_t mapCount;
    bool gpuOnly;
} render_gpu_buffer_t;

extern render_context_d3d12_t renderLocal;
bool Render_WaitForBufferUse( void );
void Render_GpuBufferDestroyNow( render_gpu_buffer_t* buffer );

#endif // RENDER_D3D12_H