/*

*/

#ifndef __GFX_H__
#define __GFX_H__

#include "core/Platform.h"

#define XE_GFX_BUFFER_DATA_SIZE 16
#define XE_GFX_VERTEX_DESC_DATA_SIZE 128

typedef struct gfx_buffer_s {
	uint64_t	data[ XE_GFX_BUFFER_DATA_SIZE ];
} gfx_vertex_desc_t;

typedef struct gfx_vertex_desc_s {
	uint64_t	data[ XE_GFX_VERTEX_DESC_DATA_SIZE ];
} gfx_vertex_desc_t;


XE_API void Gfx_BufferCreate( gfx_buffer_t* self_, size_t size, uint64_t flags );
XE_API void Gfx_BufferDestroy( gfx_buffer_t* self_ );
XE_API size_t Gfx_BufferGetSize( gfx_buffer_t* self_ );
XE_API void * Gfx_BufferMap( gfx_buffer_t * self_, uintptr_t start, size_t size );
XE_API void Gfx_BufferUnmap( gfx_buffer_t * self_ );



#endif
