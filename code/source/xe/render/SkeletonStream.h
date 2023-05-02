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

#ifndef __XE_SKELETONSTREAM_H__
#define __XE_SKELETONSTREAM_H__

// Version 1 : Initial version
// Version 2 : added human readable joint names
// Version 3 : added offset transform
#define SKELETON_STREAM_VERSION 3

typedef struct skeleton_stream_s {
    uint32_t            version;
    uint32_t            flags;
    uint32_t            jointCount;
    uint32_t            offsJointParents;
    uint32_t            offsJointHashArray;
    uint32_t            offsJointHashMap;
    uint32_t            offsJointMatrices;
    uint32_t            offsJointBindMatrices;
    uint32_t            offsNameStrings;
    uint32_t            offsNames;
    uint32_t            offsOffsetTransform;      // A 4x4 matrix representing the offset transform
    uint32_t            pad;
} skeleton_stream_t;
        
/*
        template<typename __type__>
        const __type__ * GetPointer( uint32_t offset ) const {
            return (const __type__*) (((uintptr_t) this) + offset);
        }
        
        const int32_t * GetJointParents() const { return GetPointer<int32_t>( m_offsJointParents ); }
        const uint64_t * GetJointHashArray() const { return GetPointer<uint64_t>( m_offsJointHashArray ); }
        const uint32_t * GetJointHashMap() const { return GetPointer<uint32_t>( m_offsJointHashMap ); }
        const fx::math::Mat4 * GetJointMatrices() const { return GetPointer<fx::math::Mat4>( m_offsJointMatrices ); }
        const fx::math::Mat4 * GetJointBindMatrices() const { return GetPointer<fx::math::Mat4>( m_offsJointBindMatrices ); }
        const char * GetStrings() const { return GetPointer<char>( m_offsNameStrings ); }
        const uint32_t * GetJointNames() const { return GetPointer<uint32_t>( m_offsNames ); }
        
        const char * GetJointName( uint32_t index ) const {
            assert(index < m_jointCount);
            const char * str = &(GetStrings()[ GetJointNames()[ index ] ]);
            return str;
        }
        
        const fx::math::Mat4 * GetOffsetTransform() const { return GetPointer<fx::math::Mat4>( m_offsOffsetTransform ); }
    };
}
 */

#endif
