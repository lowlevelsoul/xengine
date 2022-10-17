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

#include "mem/UnitHeap.h"
#include "core/Sys.h"

typedef struct unit_entry_s {
    struct unit_entry_s * prev;
    struct unit_entry_s * next;
} unit_entry_t;

typedef struct unit_heap_s {
    unit_entry_t *  listHead;
    unit_entry_t *  listTail;
    uint64_t        memory;
    size_t          unitSize;
} unit_heap_t;

/*=======================================================================================================================================*/
unit_heap_t * UnitHeap_Create( uintptr_t memory, size_t memSize, size_t unitSize ) {
    unitSize = ( unitSize >= sizeof( unit_entry_t ) ) ? unitSize : sizeof( unit_entry_t );
    
    size_t actualMemSize;
    size_t numUnits;
    uintptr_t memStart = Sys_Align( memory, 64 );
    uintptr_t currAddr;
    unit_heap_t * heap = ( unit_heap_t * ) memStart;
    memStart = Sys_Align( memStart + sizeof( unit_heap_t ), 64 );
    actualMemSize = memStart - memory;
    
    numUnits = actualMemSize / unitSize;
    currAddr = memStart;
    unit_entry_t * entry;
    unit_entry_t * prev = NULL;
    
    for ( int n = 0; n < numUnits; ++n ) {
        entry = (unit_entry_t *) currAddr;
        entry->prev = ( prev == NULL ) ? NULL : prev;
        entry->next = NULL;
        
        if ( prev != NULL ) {
            prev->next = entry;
        }
        
        prev = entry;
        currAddr += unitSize;
    }
    
    return heap;
}

/*=======================================================================================================================================*/
void UnitHeap_Destroy( unit_heap_t * self_ ) {
    
}

/*=======================================================================================================================================*/
void * UnitHeap_Alloc( unit_heap_t * self_ ) {
    return NULL;
}

/*=======================================================================================================================================*/
void UnitHeap_Free( unit_heap_t * self_ ) {
    NULL;
}
