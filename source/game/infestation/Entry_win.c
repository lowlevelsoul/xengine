/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern int Engine_Main( int argc, char *argv[] );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
    /* We'll have to pass the command line arguments to Engine_Main by taking the wide string arguments from WinMain and converting them to char** */
    return Engine_Main( 0, NULL );
}