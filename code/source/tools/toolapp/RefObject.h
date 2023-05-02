//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __REFOBJECT_H__
#define __REFOBJECT_H__

#include <stdint.h>
#include <assert.h>

class RefObject {
public:
    RefObject() {
        m_refCount = 0;
    }

    virtual ~RefObject() {

    }

    void IncRef() {
        ++m_refCount;
    }

    void DecRef() {
        assert( m_refCount > 0 );
        --m_refCount;
        if (m_refCount == 0) {
            delete this;
        }
    }

protected:
    uint64_t		m_refCount;
};


#endif 
