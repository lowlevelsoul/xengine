/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/GameInterface.h"
#include "xe/Render.h"
#include "xe/RenderBatch.h"

static void Game_Initialise( void );
static void Game_Finalise( void );
static void Game_Think( void );
static const char * Game_GetTitle( void );

/*==========================================================================================================*/
void Game_Publish( game_interface_t *gameInterface ) {
    gameInterface->GameInitialise = Game_Initialise;
    gameInterface->GameFinalise = Game_Finalise;
    gameInterface->GameThink = Game_Think;
    gameInterface->GameGetTitle = Game_GetTitle;
}

/*==========================================================================================================*/
static void Game_Initialise( void ) {
}

/*==========================================================================================================*/
static void Game_Finalise( void ) {
}

/*==========================================================================================================*/
static void Game_Think( void ) {
    batch_cmd_scene_t scene = { 0 };
    Render_SubmitScene( &scene );
}

/*==========================================================================================================*/
static const char * Game_GetTitle( void ) {
    return "Infestation";
}