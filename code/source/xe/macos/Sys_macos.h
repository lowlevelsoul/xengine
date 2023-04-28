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

#ifndef __SYS_MACOS_H__
#define __SYS_MACOS_H__

#include <sys/time.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
#include <errno.h>
#include "core/Sys.h"

#define SYS_MAX_LISTENERS 8

typedef struct sys_macos_s {
    bool_t                   timeIsMonotomic;
    mach_timebase_info_data_t   timeBaseInfo;
    uint64_t                    machStartTime;
    struct timeval              tvStartTime;
    
    sys_print_listener_t        printListeners[ SYS_MAX_LISTENERS ];
    void *                      printContexts[ SYS_MAX_LISTENERS ];
    size_t                      printListenerCount;
} sys_macos_t;

#endif
