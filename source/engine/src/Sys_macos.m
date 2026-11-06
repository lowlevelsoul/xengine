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
} sys_t;

sys_t sys;
bool sysInitialised = false;

/*==========================================================================================================*/
X_API void Sys_Initialise( void ) {
    
    if ( sysInitialised == true ) {
        return;
    }

    sys.timeIsMonotomic = false;
    sysInitialised = true;
               
        //fxprintf("==== System Init ====\n");

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

}

/*==========================================================================================================*/
X_API void Sys_Sleep( uint32_t milliseconds ) {
    usleep( milliseconds * 1000 );
}

/*==========================================================================================================*/
X_API void Sys_PrintCat( const char* cat, const char* fmt, ... ) {

}

/*==========================================================================================================*/
X_API void Sys_AssertPrint( const char* file, int line, const char** fmt, ... ) {

}

/*==========================================================================================================*/
X_API void Sys_AddPrintCallback( Sys_PrintCallback callback ) {

}