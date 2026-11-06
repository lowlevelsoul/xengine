/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Sys.h"
#include "Str.h"
#include "Mem.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAX_NUM_PRINTCALLBACKS 8

static_assert( sizeof( sys_mux_t ) >= sizeof( CRITICAL_SECTION ), "sys_mux_t is too small for CRITICAL_SECTION" );

typedef struct sys_win_s {
    str_t               printString;
    str_t               tmpString;
    sys_mux_t           printMux;
    Sys_PrintCallback   printCallbacks[ MAX_NUM_PRINTCALLBACKS ];
    uint32_t            printCallbackCount;
} sys_win_t;

static sys_win_t sys;
static bool sysInitialised = false;

/*==========================================================================================================*/
static void Sys_DebuggerPrintCb( const char * message ) {
    OutputDebugStringA( message );
}

/*==========================================================================================================*/
void Sys_Initialise( void ) {
    if ( sysInitialised ) {
        return;
    }

    Mem_Zero( &sys, sizeof( sys ) );
    
    Sys_MuxCreate( &sys.printMux );
    sys.printCallbackCount = 0;

    Str_Reserve( &sys.printString, 4096 );
    Str_Reserve( &sys.tmpString, 4096 );

    if ( Sys_IsDebuggerPresent() ) {
        Sys_PrintRegisterCallback( Sys_DebuggerPrintCb );
    }

    sysInitialised = true;
}

/*==========================================================================================================*/
void Sys_Finalise( void ) {
    if ( !sysInitialised ) {
        return;
    }

    Sys_MuxDestroy( &sys.printMux );
    Str_Destroy( &sys.printString );
    Str_Destroy( &sys.tmpString );

    Mem_Zero( &sys, sizeof( sys ) );
    sysInitialised = false;
}

/*==========================================================================================================*/
uint64_t Sys_GetTicks( void ) {
    return GetTickCount64();
}

/*==========================================================================================================*/
void Sys_Breakpoint() {
    __debugbreak();
}

/*==========================================================================================================*/
void Sys_Exit(void) {
    ExitProcess(0);
}

/*==========================================================================================================*/
bool Sys_IsDebuggerPresent( void ) {
    return IsDebuggerPresent() != 0;
}

/*==========================================================================================================*/
void Sys_Sleep( uint32_t milliseconds ) {
    Sleep( milliseconds );
}

/*==========================================================================================================*/
static void Sys_DoPrintCallbacks(void) {
    for ( uint32_t i = 0; i < sys.printCallbackCount; ++i ) {
        if ( sys.printCallbacks[ i ] != NULL ) {
            sys.printCallbacks[ i ]( sys.printString );
        }
    }
}

/*==========================================================================================================*/
void Sys_PrintCat( const char* cat, const char* fmt, ... ) {

    Sys_MuxLock( &sys.printMux );
    
    va_list args;
    va_start( args, fmt );
    Str_FormatArg( &sys.tmpString, fmt, args );
    va_end( args );

    Str_Formatf( &sys.printString, "[%s]: ", cat );
    Str_Append( &sys.printString, sys.tmpString );
    Sys_DoPrintCallbacks();

    Sys_MuxUnlock( &sys.printMux );
}

/*==========================================================================================================*/
void Sys_AssertPrint( const char* file, int line, const char** fmt, ... ) {
    Sys_MuxLock( &sys.printMux );

    va_list args;
    va_start( args, fmt );
    Str_FormatArg( &sys.tmpString, fmt, args );
    va_end( args );

    Str_Formatf( &sys.printString, "[ASSERT] %s:%d: ", file, line );
    Str_Append( &sys.printString, sys.tmpString );
    Sys_DoPrintCallbacks();

    Sys_MuxUnlock( &sys.printMux );
}

/*==========================================================================================================*/
void Sys_AddPrintCallback( Sys_PrintCallback callback ) {
    if ( sys.printCallbackCount < MAX_NUM_PRINTCALLBACKS ) {
        sys.printCallbacks[ sys.printCallbackCount++ ] = callback;
    }
}

/*==========================================================================================================*/
/* Mux */
/*==========================================================================================================*/

/*==========================================================================================================*/
void Sys_MuxCreate( sys_mux_t* mux ) {
    InitializeCriticalSection( ( CRITICAL_SECTION* )mux->data );
}

/*==========================================================================================================*/
void Sys_MuxDestroy( sys_mux_t* mux ) {
    DeleteCriticalSection( ( CRITICAL_SECTION* )mux->data );
}

/*==========================================================================================================*/
void Sys_MuxLock( sys_mux_t* mux ) {
    EnterCriticalSection( ( CRITICAL_SECTION* )mux->data );
}

/*==========================================================================================================*/
bool Sys_MuxTryLock( sys_mux_t* mux ) {
    return TryEnterCriticalSection( ( CRITICAL_SECTION* )mux->data ) != 0;
}

/*==========================================================================================================*/
void Sys_MuxUnlock( sys_mux_t* mux ) {
    LeaveCriticalSection( ( CRITICAL_SECTION* )mux->data );
}
