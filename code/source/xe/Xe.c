/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#include "core/Sys.h"
#include "mem/Mem.h"
#include "core/CVar.h"
#include "core/Fs.h"
#include "Xe.h"
#include "resource/Resource.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/MaterialResource.h"
#include "render/Texture.h"

//CVAR_INT(app_dispWidth, "Display width for the application", 640);
//CVAR_INT(app_dispHeight, "Display height for the application", 480);
//CVAR_INT(app_dispFullscreen, "1 = fullscreen, 0 = windowed", 0);
//CVAR_STRING(app_title, "Title for the app window", "XEngine");

#define MEM_STATS_FREQUENCY 120

typedef struct engine_s {
    game_interface_t    gameInterface;
    mem_allocator_t *   gameAllocator;
    uint64_t            lastTick;
    bool_t              firstFrame;
    uint64_t            memStatFrameCount;
} engine_t;

engine_t engine;

/*=======================================================================================================================================*/
void XE_Initialise(void) {
    
    engine.gameAllocator = Game_GetAllocator();
    engine.firstFrame = true;
    engine.memStatFrameCount = MEM_STATS_FREQUENCY;
    
    Mem_Initialise( engine.gameAllocator );
    Sys_Initialise();
    //CVAR_initialise();
    FS_Initialise();
    Resource_Initialise();
    
    Resource_RegisterFactory( model_resource_factory, "bmdl" );
    Resource_RegisterFactory( texture_resource_factory, "png" );
    Resource_RegisterFactory( texture_resource_factory, "tga" );
    Resource_RegisterFactory( texture_resource_factory, "jpg" );
    Resource_RegisterFactory( texture_resource_factory, "btex" );
    Resource_RegisterFactory( material_resource_factory, "mat" );
    Resource_RegisterFactory( material_resource_factory, "bmat" );
        
    Game_Create( &engine.gameInterface );
}

/*=======================================================================================================================================*/
void XE_GameInitialise() {
    engine.gameInterface.initialise();
}

/*=======================================================================================================================================*/
void XE_Finalise(void) {
    engine.gameInterface.finalise();
    Game_Destroy( &engine.gameInterface );
    
    Resource_Finalise();
    FS_Finalise();
    //CVAR_finalise();
}

/*=======================================================================================================================================*/
void XE_RegisterCVars(void) {
    //CVAR_register(&app_dispWidth);
    //CVAR_register(&app_dispHeight);
    //CVAR_register(&app_dispFullscreen);
    //CVAR_register(&app_title);
}

/*=======================================================================================================================================*/
void XE_Think(void) {
    float deltaTime = 1.0f / 60.0f;
    if ( engine.firstFrame == true ) {
        engine.firstFrame = false;
        engine.lastTick = Sys_GetTicks();
    }
    else {
        uint64_t currTick = Sys_GetTicks();
        uint64_t deltaTick = currTick - engine.lastTick;
        deltaTime = (float)deltaTick / 1000.0f;
        engine.lastTick = currTick;
    }
    
    engine.gameInterface.think( deltaTime );
    
    --engine.memStatFrameCount;
    if (  engine.memStatFrameCount == 0 ) {
        mem_stats_t stats;
        Mem_GetStats( &stats );
        double allocMB = (double) stats.sizeAlloc / (1024.0f * 1024.0f);
        xprintf("==Mem Stats==\n    Num Allocs %lu\n    Num Frees %lu\n    Total Allocated %4.4lf\n", stats.numAllocs, stats.numFrees, allocMB );
        engine.memStatFrameCount = MEM_STATS_FREQUENCY;
    }
    
    engine.gameInterface.draw( deltaTime );
}

