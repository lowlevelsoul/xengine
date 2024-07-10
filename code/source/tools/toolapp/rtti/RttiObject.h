//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2024 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef RttiObject_h
#define RttiObject_h

#include "RttiType.h"

class RttiUnknown {
public:
    RttiUnknown() {
        
    }
    
    ~RttiUnknown() {
        
    }
    
    static RttiType * GetClassType() { return nullptr; }
};


class RttiObject : public RttiUnknown {
    RTTI_CLASS( RttiObject, RttiUnknown )
public:
    
    RttiObject();
    
    virtual ~RttiObject();
};


#endif /* RttiObject_hpp */
