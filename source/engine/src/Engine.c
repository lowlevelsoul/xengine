/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Mem.h"
#include "xe/Sys.h"
#include "xe/Render.h"
#include "xe/Fs.h"


/*==========================================================================================================*/
void Engine_Initialise(void) {
    Mem_Initialise();
    Sys_Initialise();
    FS_Create();
    
    render_params_t renderParams;
    renderParams.title = "Game";
    renderParams.width = 1280;
    renderParams.height = 720;
    renderParams.fullscreen = false;
    renderParams.maxFramesInFlight = 2;
    renderParams.vsync = true;
    
    Render_Create( &renderParams );
}


/*==========================================================================================================*/
void Engine_Finalise(void) {
    Render_Destroy();
    FS_Destroy();
    Sys_Finalise();
    Mem_Finalise();
}

/*==========================================================================================================*/
void Engine_MainLoop(void) {
    bool loop = true;
    do {
        loop = Render_PumpWindowEvents();
        if ( loop == true ) {
            
        }
        
    } while( loop == true );
}

/*==========================================================================================================*/
int32_t Engine_Main( int argc, char ** argv ) {
    Engine_Initialise();
    Engine_MainLoop();
    Engine_Finalise();
    return 1;
}
