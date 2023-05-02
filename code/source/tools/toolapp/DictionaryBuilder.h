//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __DICTIONARYBUILDER_H__
#define __DICTIONARYBUILDER_H__

#include "util/Dictionary.h"
#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "container/Array.h"
#include "string/String.h"

class DictionaryBuilder {
public:
    DictionaryBuilder();
    
    virtual ~DictionaryBuilder();
    
    void SetDictionaryType( uint64_t type );
    
    void AddEntry( const char * name, const void * data, size_t dataSize );
    
    void WriteToStream( ToolMemStream & str );
    
protected:
    struct Header {
        uint64_t            magic;
        uint64_t            type;
        
        uint32_t            version;
        uint32_t            dataSize;
        uint32_t            entryCount;
        uint32_t            offsEntries;
        uint32_t            offsNameArray;
        uint32_t            offsNameMap;
        uint32_t            offsData;
        uint32_t            pad;

    };
    
    struct Entry {
        uint32_t        offset;
        uint32_t        size;
    };
    
    void WriteHeader( ToolMemStream & str, const Header & header );
    
    
    uint64_t                type = 0;
    Array<uint64_t>     entryNameArray;
    Array<uint32_t>     entryNameMap;
    
    ToolMemStream           data;
    ToolMemStream           entries;
};


#endif
