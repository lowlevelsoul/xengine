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

#ifndef __CVAR_H__
#define __CVAR_H__

#include "core/Common.h"

#define CVAR_INT(VAR, DESC, VAL) cvar_t VAR = {#VAR, DESC, CVAR_TYPE_INT, .defaultValue.intVal=VAL, 0}
#define CVAR_FLOAT(VAR, DESC, VAL) cvar_t VAR = {#VAR, DESC, CVAR_TYPE_FLOAT, .defaultValue.floatVal=VAL, 0}
#define CVAR_STRING(VAR, DESC, VAL) cvar_t VAR = {#VAR, DESC, CVAR_TYPE_STRING, .defaultValue.stringVal=VAL, 0}

#define CVAR_TYPE_INT 1
#define CVAR_TYPE_FLOAT 2
#define CVAR_TYPE_STRING 3
#define CVAR_TYPE_MASK 0x3

/**
    A struct that holds information about a CVar. CVar is short for "console variable" and this concept is taken directly from Quake2, where
    they are used a as global variables for holding configuration information, and passing value to various systems. A CVar has a global instance
    that is declared by one of the CVAR macros (CVAR_INT, CVAR_FLOAT, CVAR_STRING) and then registered with the CVar system by calling
    CVAR_register().
 
    Once a CVar is registered, it can be set in a command file or simple set in code (externing the desired cvar_t instance).
 
 */
typedef struct cvar_s {
    const char*     name;		/**< Name of the c-var */
    const char*     desc;
    uint64_t        flags;		/**< Flags denoting type and behavior */
    
    union {
        int64_t         intVal;
        float           floatVal;
        const char*     stringVal;
    } defaultValue;
    
    union {
        int64_t  intVal;
        float    floatVal;
        char*    stringVal;
    } value;
} cvar_t;

#ifdef __cplusplus
extern "C" {
#endif

void CVAR_initialise(void);
void CVAR_finalise(void);

cvar_t* CVAR_find(const char* name);
void CVAR_register(cvar_t* cvar);

void CVAR_setFromInt(cvar_t* cvar, int64_t val);
void CVAR_setFromFloat(cvar_t* cvar, float val);
void CVAR_setFromString(cvar_t* cvar, const char* string);

void CVAR_setNamedFromInt(const char* name, int64_t val);
void CVAR_setNamedFromFloat(const char* name, float val);
void CVAR_setNamedFromString(const char* name, const char* string);

int64_t CVAR_getInt(cvar_t* cvar);
float CVAR_getFloat(cvar_t* cvar);
const char* CVAR_getString(cvar_t* cvar);
uint32_t CVAR_getType(cvar_t * cvar);

#ifdef __cplusplus
}
#endif




#endif
