/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Sys.h"
#import <Foundation/Foundation.h>
#import <new>
#import <pthread.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <time.h>

typedef struct sys_s{
    bool                        timeIsMonotomic;
    mach_timebase_info_data_t   timeBaseInfo;
    uint64_t                    machStartTime;
    struct timeval              tvStartTime;    

    str_t                       printString;
    str_t                       tmpString;
    sys_mux_t                   printMux;
    Sys_PrintCallback           printCallbacks[ MAX_NUM_PRINTCALLBACKS ];
    uint32_t                    printCallbackCount;
} sys_t;

sys_t sys;
bool sysInitialised = false;

static_assert( sizeof( sys_mux_t ) >= sizeof( pthread_mutex_t ), "sys_mux_t is too small for pthread_mutex_t" );

/*==========================================================================================================*/
static void Sys_DebuggerPrintCb( const char * msg ) {
    printf( "%s\n", msg );
}

/*==========================================================================================================*/
X_API void Sys_Initialise( void ) {
    
    if ( sysInitialised == true ) {
        return;
    }

    Str_Reserve( &sys.printString, 4096 );
    Str_Reserve( &sys.tmpString, 4096 );
    Sys_MuxInit( &sys.printMux );

    if ( Sys_IsDebuggerPresent() ) {
        Sys_AddPrintCallback( Sys_DebuggerPrintCb );
    }

    sys.timeIsMonotomic = false;

    kern_return_t res = mach_timebase_info( &sys.timeBaseInfo );
            
    // Is the machine CPU clock constant, or does it change over time?
    sys.timeIsMonotomic  = ( res == 0 );

    if ( sys.timeIsMonotomic ) {
        //fxprintf("System clock is monotomic\n");
        // CPU clock is constant, so use the machine absolute time tick as our clock-tick
        sys.machStartTime = mach_absolute_time();
    }
    else {
        //fxprintf("System clock is non-monotomic\n");
        // CPU clock is not constant, so use the absolute system time
        gettimeofday( &sys.tvStartTime, NULL );
    }

    sysInitialised = true;
}

/*==========================================================================================================*/
X_API void Sys_Finalise( void ) {

}

/*==========================================================================================================*/
X_API uint64_t Sys_GetTicks( void ) {
    if ( sys.timeIsMonotomic ) {
        uint64_t now = mach_absolute_time();
        return ( now - sys.machStartTime ) * sys.timeBaseInfo.numer / sys.timeBaseInfo.denom;
    } else {
        struct timeval now;
        gettimeofday( &now, NULL );
        return ( now.tv_sec - sys.tvStartTime.tv_sec ) * 1000 + ( now.tv_usec - sys.tvStartTime.tv_usec ) / 1000;
    }
}

/*==========================================================================================================*/
X_API void Sys_Breakpoint() {
    __builtin_trap(); // Trigger a breakpoint on macOS
}

/*==========================================================================================================*/
X_API void Sys_Sleep( uint32_t milliseconds ) {
    usleep( milliseconds * 1000 );
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
bool Sys_IsDebuggerPresent( void ) {
    return false; // macOS does not have a direct equivalent to IsDebuggerPresent
}


/*==========================================================================================================*/
/* Mux */
/*==========================================================================================================*/

/*==========================================================================================================*/
void Sys_MuxCreate( sys_mux_t* mux ) {
    pthread_mutex_init( ( pthread_mutex_t* )mux->data, NULL );
}

/*==========================================================================================================*/
void Sys_MuxDestroy( sys_mux_t* mux ) {
    pthread_mutex_destroy( ( pthread_mutex_t* )mux->data );
}

/*==========================================================================================================*/
void Sys_MuxLock( sys_mux_t* mux ) {
    pthread_mutex_lock( ( pthread_mutex_t* )mux->data );
}

/*==========================================================================================================*/
bool Sys_MuxTryLock( sys_mux_t* mux ) {
    return pthread_mutex_trylock( ( pthread_mutex_t* )mux->data ) == 0;
}

/*==========================================================================================================*/
void Sys_MuxUnlock( sys_mux_t* mux ) {
    pthread_mutex_unlock( ( pthread_mutex_t* )mux->data );
}
