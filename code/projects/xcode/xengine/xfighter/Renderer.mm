//
//  Renderer.m
//  xfighter
//
//  Created by James Steele on 05.10.22.
//

#import <simd/simd.h>

#import "Renderer.h"

#include "Xe.h"
#include "render/Render3d.h"
#include "Fs.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/Texture.h"

@implementation Renderer
{
    id<MTLDevice> _device;
}

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;
{
    self = [super init];
    if(self) {
        _device = view.device;
        view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        view.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        view.sampleCount = 1;
        
        XE_Initialise();
        
        render_params_t renderParams;
        renderParams.displayWidth = -1;
        renderParams.displayHeight = -1;
        renderParams.maxBuffersInflight = 2;
        renderParams.nativeView = (__bridge void *) view;
        
        Render_Initialise( &renderParams );
        XE_GameInitialise();
    }
    
    return self;
}



- (void)drawInMTKView:(nonnull MTKView *)view {
    XE_Think();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
}

@end
