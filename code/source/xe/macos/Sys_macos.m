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

#import <Foundation/Foundation.h>
#import "Sys_macos.h"
#import "Str.h"
#import <stdarg.h>

static sys_macos_t sys;
static boolean_t sysInit = bool_false;
static str_t printStrResult = NULL;

/*======================================================================================================================================= */
void Sys_Initialise(void) {
    if ( sysInit == bool_true ) {
        return;
    }
    
    kern_return_t res = mach_timebase_info( &sys.timeBaseInfo );
           
    /* Is the machine CPU clock constant, or does it change over time? */
    sys.timeIsMonotomic  = ( res == 0 ) ? bool_true : bool_false;

    if ( sys.timeIsMonotomic == bool_true ) {
        //fxprintf("System clock is monotomic\n");
        /* CPU clock is constant, so use the machine absolute time tick as our clock-tick */
        sys.machStartTime = mach_absolute_time();
    }
    else {
        //fxprintf("System clock is non-monotomic\n");
        /* CPU clock is not constant, so use the absolute system time */
        gettimeofday( &sys.tvStartTime, NULL );
    }
    
    Str_SetCapacity(&printStrResult, 1024 * 8 );
    
    sysInit = bool_true;
    
    xprintf( "=== Sys Init ===================\n" );
    xprintf( "    %s\n", (sys.timeIsMonotomic == bool_true) ? "Time is monotomic" : "Time is not monotomic" );
}

/*======================================================================================================================================= */
void Sys_Finalise(void) {
    if ( sysInit == bool_false ) {
        return;
    }
    
    sysInit = bool_false;
}

/*======================================================================================================================================= */
uint64_t Sys_GetTicks(void) {
    uint64_t ticks;
    
    if ( sys.timeIsMonotomic == true) {
        uint64_t now = mach_absolute_time();
        ticks = (uint64_t)((((now - sys.machStartTime) * sys.timeBaseInfo.numer) / sys.timeBaseInfo.denom) / 1000000);
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = (uint64_t)((now.tv_sec - sys.tvStartTime.tv_sec) * 1000 + (now.tv_usec - sys.tvStartTime.tv_usec) / 1000);
    }
    
    return ticks;
}

/*======================================================================================================================================= */
void Sys_Printf( const char * fmt, ... ) {
    /* initialize use of the variable argument array */
    va_list vaArgs;
    va_start( vaArgs, fmt );
    
    Str_VFormatArgs( &printStrResult, fmt, vaArgs );
    
    printf("%s", printStrResult );
}

/*======================================================================================================================================= */
void Sys_Breakpoint(void) {
    __builtin_trap();
}

/*======================================================================================================================================= */
void Sys_Exit( int code ) {
    exit(code);
}

/*======================================================================================================================================= */
void Sys_AssertPrintf( const char * file, int line, const char * fmt, ... ) {
    str_t msg = NULL;
    str_t header = NULL;
    Str_VFormat( &header, "!!!ASSERT!!!\nFile: %s\nLine %u\n", file, line );
    
    va_list vaArgs;
    va_start( vaArgs, fmt );
    
    Str_VFormatArgs( &msg, fmt , vaArgs );
    
    printf( "%s%s", header, msg );
    
    Str_Destroy( &msg );
    Str_Destroy( &header );
}
