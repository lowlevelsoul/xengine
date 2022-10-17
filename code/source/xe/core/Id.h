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

#ifndef __ID_H__
#define __ID_H__

#define ID_MUL_0 (uint64_t) (1)
#define ID_MUL_1 (uint64_t) (38)
#define ID_MUL_2 (uint64_t) ((ID_MUL_1) * (uint64_t) 38)
#define ID_MUL_3 (uint64_t) ((ID_MUL_2) * (uint64_t) 38)
#define ID_MUL_4 (uint64_t) ((ID_MUL_3) * (uint64_t) 38)
#define ID_MUL_5 (uint64_t) ((ID_MUL_4) * (uint64_t) 38)
#define ID_MUL_6 (uint64_t) ((ID_MUL_5) * (uint64_t) 38)
#define ID_MUL_7 (uint64_t) ((ID_MUL_6) * (uint64_t) 38)
#define ID_MUL_8 (uint64_t) ((ID_MUL_7) * (uint64_t) 38)
#define ID_MUL_9 (uint64_t) ((ID_MUL_8) * (uint64_t) 38)
#define ID_MUL_10 (uint64_t) ((ID_MUL_9) * (uint64_t) 38)
#define ID_MUL_11 (uint64_t) ((ID_MUL_10) * (uint64_t) 38)

#define ID_CHAR_0 0
#define ID_CHAR_1 1
#define ID_CHAR_2 2
#define ID_CHAR_3 3
#define ID_CHAR_4 4
#define ID_CHAR_5 5
#define ID_CHAR_6 6
#define ID_CHAR_7 7
#define ID_CHAR_8 8
#define ID_CHAR_9 9
#define ID_CHAR__ 10
#define ID_CHAR_SPACE 11
#define ID_CHAR_space 11

#define ID_CHAR_A 12
#define ID_CHAR_B 13
#define ID_CHAR_C 14
#define ID_CHAR_D 15
#define ID_CHAR_E 16
#define ID_CHAR_F 17
#define ID_CHAR_G 18
#define ID_CHAR_H 19
#define ID_CHAR_I 20
#define ID_CHAR_J 21
#define ID_CHAR_K 22
#define ID_CHAR_L 23
#define ID_CHAR_M 24
#define ID_CHAR_N 25
#define ID_CHAR_O 26
#define ID_CHAR_P 27
#define ID_CHAR_Q 28
#define ID_CHAR_R 29
#define ID_CHAR_S 30
#define ID_CHAR_T 31
#define ID_CHAR_U 32
#define ID_CHAR_V 33
#define ID_CHAR_W 34
#define ID_CHAR_X 35
#define ID_CHAR_Y 36
#define ID_CHAR_Z 37

#define ID_CHAR_a 12
#define ID_CHAR_b 13
#define ID_CHAR_c 14
#define ID_CHAR_d 15
#define ID_CHAR_e 16
#define ID_CHAR_f 17
#define ID_CHAR_g 18
#define ID_CHAR_h 19
#define ID_CHAR_i 20
#define ID_CHAR_j 21
#define ID_CHAR_k 22
#define ID_CHAR_l 23
#define ID_CHAR_m 24
#define ID_CHAR_n 25
#define ID_CHAR_o 26
#define ID_CHAR_p 27
#define ID_CHAR_q 28
#define ID_CHAR_r 29
#define ID_CHAR_s 30
#define ID_CHAR_t 31
#define ID_CHAR_u 32
#define ID_CHAR_v 33
#define ID_CHAR_w 34
#define ID_CHAR_x 35
#define ID_CHAR_y 36
#define ID_CHAR_z 37

#define MAKE_ID(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11)\
    (uint64_t) ((ID_CHAR_##c0 * ID_MUL_0) +\
                (ID_CHAR_##c1 * ID_MUL_1) +\
                (ID_CHAR_##c2 * ID_MUL_2) +\
                (ID_CHAR_##c3 * ID_MUL_3) +\
                (ID_CHAR_##c4 * ID_MUL_4) +\
                (ID_CHAR_##c5 * ID_MUL_5) +\
                (ID_CHAR_##c6 * ID_MUL_6) +\
                (ID_CHAR_##c7 * ID_MUL_7) +\
                (ID_CHAR_##c8 * ID_MUL_8) +\
                (ID_CHAR_##c9 * ID_MUL_9) +\
                (ID_CHAR_##c10 * ID_MUL_10) +\
                (ID_CHAR_##c11 * ID_MUL_11))

#endif
