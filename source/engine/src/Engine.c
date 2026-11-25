/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Mem.h"
#include "xe/Sys.h"
#include "xe/Fs.h"
#include "xe/AtomicString.h"
#include "xe/Render.h"
#include "xe/GameInterface.h"

typedef struct engine_s {
    game_interface_t gameInterface;
} engine_t;

static engine_t engine;
static bool engineInitialised = false;
static void Engine_PublishGame();
extern void Game_Publish( game_interface_t* gameInterface );

/*==========================================================================================================*/
static void Engine_Initialise( void ) {
    if ( engineInitialised == true ) {
        return;
    }

    Engine_PublishGame();

    Mem_Initialise();
    AStr_Initialise();
    Sys_Initialise();
    FS_Initialise();

    render_params_t renderParams;
    renderParams.width = 800;
    renderParams.height = 600;
    renderParams.fullscreen = false;
    renderParams.vsync = true;
    renderParams.maxNumFramesInFlight = 2;
    renderParams.renderMemHeapSize = 1024 * 1024 * 4;

    Render_Initialise( &renderParams );

    if ( engine.gameInterface.GameInitialise ) {
        engine.gameInterface.GameInitialise();
    }

    engineInitialised = true;
}

/*==========================================================================================================*/
static void Engine_Finalise( void ) {
    if ( engineInitialised == false ) {
        return;
    }

    if ( engine.gameInterface.GameFinalise ) {
        engine.gameInterface.GameFinalise();
    }

    Render_Finalise();
    FS_Finalise();
    Sys_Finalise();
    AStr_Finalise();
    Mem_Finalise();

    engineInitialised = false;
}

/*==========================================================================================================*/
static void Engine_Think( void ) {
    if ( engineInitialised == false ) {
        return;
    }

    render_window_t* rw = Render_GetWindow();
    bool windowOpen = RenderWindow_PumpEvents( rw );
    if ( !windowOpen ) {
        return;
    }

    if ( engine.gameInterface.GameThink ) {
        engine.gameInterface.GameThink();
    }
}

/*==========================================================================================================*/
static void Engine_Run( void ) {
    Engine_Initialise();

    while ( engineInitialised ) {
        Engine_Think();
    }

    Engine_Finalise();
}

/*==========================================================================================================*/
void Engine_PublishGame() {
    Game_Publish( &engine.gameInterface );
}

/*==========================================================================================================*/
int Engine_Main( int argc, char* argv[] ) {
    Engine_Run();
    return 0;
}