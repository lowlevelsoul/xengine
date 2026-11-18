/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include <d3d12.h>
#include <dxgi1_6.h>

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );

typedef struct render_context_d3d12_s {
    // D3D12 device, command queue, swap chain, etc. would be stored here
    ID3D12Device* device;
    ID3D12CommandQueue* commandQueue;
    IDXGISwapChain3* swapChain;

} render_context_d3d12_t;

static render_context_d3d12_t theContext;
static bool isContextCreated = false;

static void RenderContext_Create( render_params_t* params );

/*==========================================================================================================*/
void Render_Create( render_params_t* params ) {
    RenderWindow_Create( params );
    RenderContext_Create( params );
}

/*==========================================================================================================*/
void Render_Destroy( void ) {
    RenderWindow_Destroy();
    if ( isContextCreated ) {
        // D3D12 device, command queue, swap chain destruction would go here
        isContextCreated = false;
    }
}

/*==========================================================================================================*/
void RenderContext_Create( render_params_t* params ) {
    if ( isContextCreated ) {
        return;
    }

    // D3D12 device, command queue, swap chain creation would go here

    isContextCreated = true;
}
