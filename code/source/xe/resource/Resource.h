//
//  Res.h
//  xengine
//
//  Created by James Steele on 09.10.22.
//

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "core/Common.h"
#include "Fs.h"

typedef struct resource_s  {
    uint64_t        data[8];
} resource_t;

typedef struct resource_factory_s {
    const char *        desc;
    void                (*load)( resource_t * self_, file_t * file, const char * path );
    void *              (*alloc)(void);
    void                (*free)( void * data );
} resource_factory_t;

XE_API void Resource_Initialise( void );
XE_API void Resource_Finalise( void );
XE_API void Resource_RegisterFactory( resource_factory_t * factory, const char * extOverride );
XE_API resource_t * Resource_Load( const char * path );
XE_API resource_t * Find( const char * path );
XE_API void * Resource_GetData( resource_t * self_ );

#define DEFINE_RESOURCE_FACTORY( NAME, FUNC, STRUCT )\
static void         FUNC##Resource_Load( resource_t * self_, file_t * file, const char * path );\
static void *       FUNC##Resource_Alloc( void );\
static void         FUNC##Resource_Free( void * data );\
resource_factory_t STRUCT##_resource_factory_inst = {\
    NAME,\
    FUNC##Resource_Load,\
    FUNC##Resource_Alloc,\
    FUNC##Resource_Free\
};\
resource_factory_t * STRUCT##_resource_factory = &STRUCT##_resource_factory_inst;
    

#endif 
