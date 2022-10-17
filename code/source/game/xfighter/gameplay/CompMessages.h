/*
===========================================================================================================================================

    Copyright 2022 James Steele

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

#ifndef __COMPMESSAGES_H__
#define __COMPMESSAGES_H__

#include "ecs/EcsTypes.h"

typedef enum comp_message_e {
    MSG_LOC_ROT =  32,
    MSG_TRANSORM,
} comp_message_t;

typedef struct msg_loc_rot_s {
    ecs_msg_t       header;
    vec3_t          location;
    quat_t          rotation;
} msg_loc_rot_t;

typedef struct msg_transform_s {
    ecs_msg_t       header;
    mat4_t          transform;
} msg_transform_t;

#endif
