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

#include "core/CVar.h"
#include "Crc32.h"
#include "core/Bsearch.h"

#include <assert.h>
#include <string.h>


#if 0
#include "tlsf/tlsf.h"

#define CVAR_CAPACITY 1024
#define CVAR_STRING_MEMORY_SIZE 1024 * 1024 * 1

typedef struct cvar_system_s {
    
    cvar_t*         cvars[CVAR_CAPACITY];
    uint32_t        cvarHashes[CVAR_CAPACITY];
    size_t          cvarCount;
    
    void*           stringMemory;
    size_t          stringMemorySize;
    tlsf_t          stringMemoryHeap;
} cvar_system_t;

static cvar_system_t cvarSystemLocal;
static cvar_system_t* cvarSystem = NULL;

static void CVAR_add(cvar_t* cvar);
static boolean_t CVAR_findHashIndex(int32_t* index, uint32_t value, const uint32_t* valueArray, size_t arraySize);

/*---------------------------------------------------------------------------------------------------------------------------------------*/
static char* CVAR_mallocString(size_t strLen) {
    void* mem = tlsf_malloc(cvarSystem->stringMemoryHeap, strLen+1);
    assert(mem != NULL);
    return (char*) mem;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
static void CVAR_freeSring(char* string){
    if (string == NULL) {
        return;
    }
    
    tlsf_free(cvarSystem->stringMemoryHeap, string);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_initialise(void) {
    assert(cvarSystem == NULL);
    
    cvarSystem = &cvarSystemLocal;
    memset(cvarSystem, 0, sizeof(cvarSystemLocal));
    
    cvarSystem->stringMemory = malloc(CVAR_STRING_MEMORY_SIZE);
    cvarSystem->stringMemorySize = CVAR_STRING_MEMORY_SIZE;
    cvarSystem->stringMemoryHeap = tlsf_create_with_pool(cvarSystem->stringMemory, cvarSystem->stringMemorySize);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_finalise(void) {
    assert(cvarSystem != NULL);
    free(cvarSystem->stringMemory);
    cvarSystem = NULL;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t CVAR_getType(cvar_t * cvar) {
    return (uint32_t) (cvar->flags & CVAR_TYPE_MASK);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
cvar_t* CVAR_find(const char* name) {
    int32_t index = -1;
    uint32_t hash = CRC_calcFromString(name);
    
    boolean_t found = CVAR_findHashIndex(&index, hash, cvarSystem->cvarHashes, cvarSystem->cvarCount);
    if (found == FALSE) {
        return NULL;
    }
    
    return cvarSystem->cvars[index];
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_register(cvar_t* cvar) {
    assert(cvarSystem != NULL);
    assert(cvar != NULL);
    assert(CVAR_find(cvar->name) == FALSE);
    
    CVAR_add(cvar);
    
    switch(CVAR_getType(cvar)) {
        case CVAR_TYPE_INT:
            CVAR_setFromInt(cvar, cvar->defaultValue.intVal);
            break;
            
        case CVAR_TYPE_FLOAT:
            CVAR_setFromFloat(cvar, cvar->defaultValue.floatVal);
            break;
            
        case CVAR_TYPE_STRING:
            CVAR_setFromString(cvar, cvar->defaultValue.stringVal);
            break;
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_add(cvar_t* cvar) {
    int32_t index = -1;
    uint32_t hash = CRC_calcFromString(cvar->name);
    
    /* Search for the name has - it shoukd be unique, so should not be found. In this case
      the find function returns the insertion index */
    boolean_t found = CVAR_findHashIndex(&index, hash, cvarSystem->cvarHashes, cvarSystem->cvarCount);
    assert(found == FALSE);
    
    /* Move all of the hashes/cvar ptrs above the insertion point up one place */
    for(int32_t i= (int32_t)cvarSystem->cvarCount; i > index; --i) {
        cvarSystem->cvarHashes[i] = cvarSystem->cvarHashes[i-1];
        cvarSystem->cvars[i] = cvarSystem->cvars[i-1];
    }
    
    /* Set the cvar ptr and hash and update the number of cvars*/
    cvarSystem->cvarHashes[index] = hash;
    cvarSystem->cvars[index] = cvar;
    ++cvarSystem->cvarCount;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
boolean_t CVAR_findHashIndex(int32_t* index, uint32_t value, const uint32_t* valueArray, size_t arraySize) {
    return BSEARCH_uint32(index, value, valueArray, arraySize);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setFromInt(cvar_t* cvar, int64_t val) {
    uint32_t type = CVAR_getType(cvar);
    
    if (type == CVAR_TYPE_INT) {
        cvar->value.intVal = val;
    } else if (type == CVAR_TYPE_FLOAT) {
        cvar->value.floatVal = (float) val;
    }
}
        
/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setFromFloat(cvar_t* cvar, float val) {
    uint32_t type = CVAR_getType(cvar);
    
    if (type == CVAR_TYPE_INT) {
        cvar->value.intVal = (int64_t) val;
    } else if (type == CVAR_TYPE_FLOAT) {
        cvar->value.floatVal = val;
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setFromString(cvar_t* cvar, const char* string) {
    uint32_t type = CVAR_getType(cvar);
    
    if (type == CVAR_TYPE_INT) {
        cvar->value.intVal = atoi(string);
    } else if (type == CVAR_TYPE_FLOAT) {
        cvar->value.floatVal = (float) atof(string);
    } else if (type == CVAR_TYPE_STRING) {
        CVAR_freeSring(cvar->value.stringVal);
        cvar->value.stringVal = CVAR_mallocString(strlen(string));
        strcpy(cvar->value.stringVal, string);
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setNamedFromInt(const char* name, int64_t val) {
    cvar_t* cvar = CVAR_find(name);
    if (cvar != NULL) {
        CVAR_setFromInt(cvar, val);
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setNamedFromFloat(const char* name, float val){
    cvar_t* cvar = CVAR_find(name);
    if (cvar != NULL) {
        CVAR_setFromFloat(cvar, val);
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void CVAR_setNamedFromString(const char* name, const char* string){
    cvar_t* cvar = CVAR_find(name);
    if (cvar != NULL) {
        CVAR_setFromString(cvar, string);
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
int64_t CVAR_getInt(cvar_t* cvar) {
    uint32_t type = CVAR_getType(cvar);
    int64_t val = 0;
    
    if (type == CVAR_TYPE_INT) {
        val = cvar->value.intVal;
    } else if (type == CVAR_TYPE_FLOAT) {
        val = (int64_t) cvar->value.floatVal;
    } else if (type == CVAR_TYPE_STRING) {
        val = atoi(cvar->value.stringVal);
    }
    
    return val;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
float CVAR_getFloat(cvar_t* cvar) {
    uint32_t type = CVAR_getType(cvar);
    float val = 0;
    
    if (type == CVAR_TYPE_INT) {
        val = (float) cvar->value.intVal;
    } else if (type == CVAR_TYPE_FLOAT) {
        val = cvar->value.floatVal;
    } else if (type == CVAR_TYPE_STRING) {
        val = (float) atof(cvar->value.stringVal);
    }
    
    return val;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
const char* CVAR_getString(cvar_t* cvar) {
    uint32_t type = CVAR_getType(cvar);
    const char* val = NULL;
    
    if (type == CVAR_TYPE_STRING) {
        val = cvar->value.stringVal;
    }
    
    return val;
}

#endif
