/*
==============================================================================================================
==============================================================================================================
*/

#ifndef __SYS_H__
#define __SYS_H__

#include "Platform.h"

#define Sys_AlignValue( value, alignment ) ( ( ( value ) + ( ( alignment ) - 1 ) ) & ~( ( alignment ) - 1 ) )

typedef void ( *Sys_PrintCallback )( const char* message );

typedef struct sys_mux_s {
    uint64_t data[ 6 ];
} sys_mux_t;


X_API void Sys_Initialise( void );
X_API void Sys_Finalise( void );
X_API uint64_t Sys_GetTicks( void );
X_API void Sys_Breakpoint(void);
X_API void Sys_Exit(void);
X_API bool Sys_IsDebuggerPresent( void );
X_API void Sys_Sleep( uint32_t milliseconds );
X_API void Sys_PrintCat( const char* cat, const char* fmt, ... );
X_API void Sys_AssertPrint( const char* file, int line, const char** fmt, ... );
X_API void Sys_AddPrintCallback( Sys_PrintCallback callback );

X_API void Sys_MuxCreate( sys_mux_t* mux );
X_API void Sys_MuxDestroy( sys_mux_t* mux );
X_API void Sys_MuxLock( sys_mux_t* mux );
X_API bool Sys_MuxTryLock( sys_mux_t* mux );
X_API void Sys_MuxUnlock( sys_mux_t* mux );


#if defined( DEBUG ) || defined( _DEBUG )
#define Sys_Assert( COND ) ( void )( ( COND ) || ( Sys_AssertPrint( __FILE__, __LINE__, #COND ), Sys_Breakpoint(), 0 ) )
#define Sys_FatalErrorOn( COND, ... ) ( void )( ( !(COND) ) || ( Sys_PrintCat( "FATAL_ERROR", __VA_ARGS__ ), Sys_Breakpoint(), 0 ) )
#define Sys_FatalError( ... ) ( void )( Sys_PrintCat( "FATAL_ERROR", __VA_ARGS__ ), Sys_Breakpoint(), 0 )
#define Sys_Warning( ... ) ( void )( Sys_PrintCat( "WARNING", __VA_ARGS__ ) )
#else
#define Sys_Assert( COND )
#define Sys_FatalErrorOn( COND, ... )
#define Sys_FatalError( ... )
#endif // DEBUG || _DEBUG

#endif // __SYS_H__