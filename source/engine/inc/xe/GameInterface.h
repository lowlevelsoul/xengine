/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef _GAMEINTERFACE_H
#define _GAMEINTERFACE_H

#include "xe/Platform.h"

typedef struct game_interface_s {
    void ( *GameInitialise )( void );
    void ( *GameThink )( void );
    void ( *GameFinalise )( void );
    const char* ( *GameGetTitle )( void );

} game_interface_t;

#endif // _GAMEINTERFACE_H