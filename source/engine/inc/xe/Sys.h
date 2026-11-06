/*
==============================================================================================================
==============================================================================================================
*/

#ifndef __SYS_H__
#define __SYS_H__

#include "Platform.h"

#define Sys_AlignValue( value, alignment ) ( ( ( value ) + ( ( alignment ) - 1 ) ) & ~( ( alignment ) - 1 ) )

typedef void ( *Sys_PrintCallback )( const char* message );

X_API void Sys_Initialise( void );
X_API void Sys_Finalise( void );
X_API uint64_t Sys_GetTicks( void );
X_API void Sys_Breakpoint();
X_API void Sys_Sleep( uint32_t milliseconds );
X_API void Sys_PrintCat( const char* cat, const char* fmt, ... );
X_API void Sys_AssertPrint( const char* file, int line, const char** fmt, ... );
X_API void Sys_AddPrintCallback( Sys_PrintCallback callback );

#if defined( DEBUG ) || defined( _DEBUG )
#define Sys_Assert( COND ) ( void )( ( COND ) || ( Sys_AssertPrint( __FILE__, __LINE__, #COND ), Sys_Breakpoint(), 0 ) )
#else
#define Sys_Assert( COND )
#endif // DEBUG || _DEBUG

#endif // __SYS_H__