//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __AMRBUILDER_H__
#define __AMRBUILDER_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "texturebuilder/Image.h"

class AmrBuilder {
public:
    AmrBuilder();
    
    ~AmrBuilder();
    
    Image * Build( Image * occlusion, Image * mettalic, Image * roughness );
    
    bool VerifySizes( Image * occlusion, Image * mettalic, Image * roughness );
    
};

#endif
