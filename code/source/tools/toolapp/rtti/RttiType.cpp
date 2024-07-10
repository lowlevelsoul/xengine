//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2024 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "RttiType.h"
#include "core/fh64.h"

RttiType *                  RttiType::initListHead = nullptr;
RttiType *                  RttiType::initListTail = nullptr;
RttiType::vector            RttiType::typeArray;
RttiType::map               RttiType::typeMap;

//=======================================================================================================================
RttiType::RttiType( const char * name_, RttiType * super_, RttiClassFactory factory_, const RttiProp * initProps_ ) {
    name = name_;
    super = super_;
    
    
    if ( initListHead != nullptr ) {
        initListTail->next = this;
    }
    else {
        initListHead = this;
    }
    
    initListTail = this;
}


//=======================================================================================================================
RttiType::~RttiType() {
    
}


//=======================================================================================================================
void RttiType::Initialise() {
    
    for ( RttiType * curr = initListHead; curr != nullptr; curr = curr->next ) {
        uint64_t nameHash = FH64_CalcFromCStr( curr->name );
        
        uint32_t index = ( uint32_t ) typeArray.size();
        typeArray.push_back( curr );
        typeMap[ nameHash ] = index;
        
        PublishProps( curr );
    }
}

//=======================================================================================================================
void RttiType::Finalise() {
    
}

//=======================================================================================================================
void RttiType::PublishProps( RttiType * type ) {
    
    for ( RttiType * currType = type; currType != nullptr; currType = currType->GetSuper() ) {
        
        for ( const RttiProp * currProp = currType->initProps; currProp->type != RttiPropType_Nil; ++currProp ) {
            uint32_t index = (uint32_t ) type->props.size();
            type->props.push_back( * currProp );
            type->propsMap[ FH64_CalcFromCStr( currProp->name ) ] = index;
        }
    }
}

//=======================================================================================================================
const RttiProp * RttiType::FindProp( const char * name ) const {
    uint64_t hash = FH64_CalcFromCStr( name );
    RttiProp::map::const_iterator findIt = propsMap.find( hash );
    if ( findIt == propsMap.end() ) {
        return nullptr;
    }
    
    return &props[ findIt->second ];
}

//=======================================================================================================================
RttiType * RttiType::Find( const char * name ) {
    uint64_t hash = FH64_CalcFromCStr( name );
    
    RttiType::map::iterator findIt = typeMap.find( hash );
    if ( findIt == typeMap.end() ) {
        return nullptr;
    }
    
    return typeArray[ findIt->second ];
}

//=======================================================================================================================
bool RttiType::IsOfType( const RttiType * theType ) const {
    for ( const RttiType * currType = this; currType != nullptr; currType = currType->GetSuper() ) {
        if ( currType == theType ) {
            return true;
        }
    }
    
    return false;
}
