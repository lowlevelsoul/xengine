//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __REFPOINTER_H__
#define __REFPOINTER_H__

template<class __object__>
class RefPointer {
public:
    RefPointer() {
        m_object = nullptr;
    }

    RefPointer( const RefPointer& rhs ) {
        m_object = nullptr;
        SetObject( rhs.m_object );
    }

    RefPointer( __object__* rhs ) {
        m_object = nullptr;
        SetObject( rhs );
    }

    ~RefPointer() {
        SetObject( nullptr );
    }

    void SetObject(__object__* rhs) {
        if (m_object != nullptr) {
            m_object->DecRef();
            m_object = nullptr;
        }

        if (rhs != nullptr) {
            m_object = rhs;
            m_object->IncRef();
        }
    }

    RefPointer& operator=(const RefPointer& rhs) {
        SetObject( rhs.m_object );
        return *this;
    }

    RefPointer& operator=( __object__* rhs ) {
        SetObject( rhs );
        return *this;
    }

    __object__* Get() { return m_object;  }
    const __object__* Get() const { return m_object; }

    operator __object__*() { return m_object;  }
    operator const __object__*() const { return m_object; }
    __object__* operator->() { return m_object;  }
    const __object__* operator->() const { return m_object; }

    bool operator!() const { return m_object == nullptr; }
    bool operator==( const __object__* rhs ) const { return m_object == rhs; }
    bool operator!=(const __object__* rhs) const { return m_object != rhs; }
    bool operator<=( const __object__* rhs ) const { return m_object <= rhs; }
    bool operator>=( const __object__* rhs ) const { return m_object >= rhs; }
    bool operator<( const __object__* rhs ) const { return m_object < rhs; }
    bool operator>( const __object__* rhs ) const { return m_object > rhs; }

protected:
    __object__*		m_object;
};

#endif 
