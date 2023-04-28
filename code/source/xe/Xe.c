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
#include "Fs.h"
#include "Xe.h"
#include "resource/Resource.h"
#include "ecs/Ecs.h"

//CVAR_INT(app_dispWidth, "Display width for the application", 640);
//CVAR_INT(app_dispHeight, "Display height for the application", 480);
//CVAR_INT(app_dispFullscreen, "1 = fullscreen, 0 = windowed", 0);
//CVAR_STRING(app_title, "Title for the app window", "XEngine");

typedef struct engine_s {
    game_interface_t gameInterface;
    mem_allocator_t * gameAllocator;
    uint64_t lastTick;
    bool_t firstFrame;
} engine_t;

engine_t engine;

/*=======================================================================================================================================*/
void XE_Initialise(void) {
    
    engine.gameAllocator = Game_GetAllocator();
    engine.firstFrame = true;
    
    Mem_Initialise( engine.gameAllocator );
    Sys_Initialise();
    //CVAR_initialise();
    FS_Initialise();
    Resource_Initialise();
    Ecs_Initialise();
    
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
    
    Ecs_Finalise();
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
    engine.gameInterface.draw( deltaTime );
}

