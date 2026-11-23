/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "RenderHeapPool_metal.h"
#import "Render_metal.h"
#include "xe/Sys.h"

/*==========================================================================================================*/
static void RenderHeapPool_CreateHeap( heap_pool_t* self_, size_t heapSize ) {
    Sys_FatalErrorOn( !self_->isCreated, "RenderHeapPool_CreateHeap called before heap pool was initialised" );
    Sys_FatalErrorOn( self_->numHeaps >= MAX_NUM_HEAPS, "RenderHeapPool_CreateHeap called with too many heaps" );

    MTLHeapDescriptor* heapDescriptor = [[MTLHeapDescriptor alloc] init];

    heapDescriptor.size = heapSize;
    heapDescriptor.storageMode = self_->storageMode;
    heapDescriptor.cpuCacheMode = self_->cacheMode;
    heapDescriptor.hazardTrackingMode = self_->hazardTrackingMode;

    self_->heaps[ self_->numHeaps ] = [renderLocal.device newHeapWithDescriptor:heapDescriptor];
    self_->numHeaps++;
}

/*==========================================================================================================*/
static bool RenderHeapPool_FindAvailPool( heap_pool_t* self_, size_t* poolIndexOut, MTLSizeAndAlign* layout ) {
    if ( self_->numHeaps == 0 ) {
        return false;
    }

    /* We'll try the last successful heap first, then iterate through the rest if needed */
    size_t curHeapIndex = self_->lastSuccessfulHeapIndex;
    bool canFit = [self_->heaps[ curHeapIndex ] maxAvailableSizeWithAlignment:layout->align] >= layout->size;
    if ( canFit ) {
        self_->lastSuccessfulHeapIndex = curHeapIndex;
        if ( poolIndexOut ) {
            *poolIndexOut = curHeapIndex;
        }

        return true;
    }

    /* Iterate through the rest of the heaps if the last successful heap couldn't fit */
    for ( size_t i = 0; i < self_->numHeaps; i++ ) {
        if ( i == curHeapIndex ) {
            continue;
        }

        if ( [self_->heaps[ i ] maxAvailableSizeWithAlignment:layout->align] >= layout->size ) {
            self_->lastSuccessfulHeapIndex = i;
            if ( poolIndexOut ) {
                *poolIndexOut = i;
            }

            return true;
        }
    }

    return false;
}

/*==========================================================================================================*/
static bool RenderHeapPool_FindAvailPoolForBuffer( heap_pool_t* self_, size_t* poolIndexOut, NSUInteger length,
                                                   MTLResourceOptions options ) {
    if ( self_->numHeaps == 0 ) {
        return false;
    }

    MTLSizeAndAlign layout = [renderLocal.device heapBufferSizeAndAlignWithLength:length options:options];
    return RenderHeapPool_FindAvailPool( self_, poolIndexOut, &layout );
}

/*==========================================================================================================*/
void RenderHeapPool_Initialise( heap_pool_t* self_, size_t heapSize, MTLStorageMode storageMode,
                                MTLCPUCacheMode cacheMode, MTLHazardTrackingMode hazardTrackingMode ) {
    if ( self_->isCreated ) {
        return;
    }

    self_->isCreated = true;
    self_->numHeaps = 0;
    self_->heapSize = heapSize;
    self_->lastSuccessfulHeapIndex = 0;
    self_->storageMode = storageMode;
    self_->cacheMode = cacheMode;
    self_->hazardTrackingMode = hazardTrackingMode;
}

/*==========================================================================================================*/
void RenderHeapPool_Finalise( heap_pool_t* self_ ) {
    if ( !self_->isCreated ) {
        return;
    }

    for ( size_t i = 0; i < self_->numHeaps; i++ ) {
        self_->heaps[ i ] = nil;
    }

    self_->numHeaps = 0;
    self_->isCreated = false;
}

/*==========================================================================================================*/
id<MTLHeap> RenderHeapPool_GetHeap( heap_pool_t* self_ ) {
    Sys_FatalErrorOn( !self_->isCreated, "RenderHeapPool_GetHeap called before heap pool was initialised" );
    Sys_FatalErrorOn( self_->numHeaps == 0, "RenderHeapPool_GetHeap called with no heaps available" );

    return self_->heaps[ self_->lastSuccessfulHeapIndex ];
}

/*==========================================================================================================*/
id<MTLBuffer> RenderHeapPool_CreateBuffer( heap_pool_t* self_, NSUInteger length, MTLResourceOptions options ) {
    Sys_FatalErrorOn( !self_->isCreated, "RenderHeapPool_CreateBuffer called before heap pool was initialised" );

    MTLResourceOptions resourceOptions = options;
    id<MTLBuffer> buffer = nil;

    bool foundHeap =
        RenderHeapPool_FindAvailPoolForBuffer( self_, &self_->lastSuccessfulHeapIndex, length, resourceOptions );
    if ( foundHeap == false ) {
        RenderHeapPool_CreateHeap( self_, self_->heapSize );
        self_->lastSuccessfulHeapIndex = self_->numHeaps - 1;
    }

    id<MTLHeap> heap = self_->heaps[ self_->lastSuccessfulHeapIndex ];
    buffer = [heap newBufferWithLength:length options:resourceOptions];

    return buffer;
}

/*==========================================================================================================*/
id<MTLTexture> RenderHeapPool_CreateTexture( heap_pool_t* self_, MTLTextureDescriptor* descriptor ) {
    Sys_FatalErrorOn( !self_->isCreated, "RenderHeapPool_CreateTexture called before heap pool was initialised" );

    id<MTLHeap> heap = RenderHeapPool_GetHeap( self_ );
    id<MTLTexture> texture = [heap newTextureWithDescriptor:descriptor];
    return texture;
}
