//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __TOOLSTREAM_H__
#define __TOOLSTREAM_H__

#include <vector>

#define TOOL_STREAM_WRITE(__type__)\
    size_t Write(const __type__ * buffer, size_t count = 1) {\
        size_t rawWriteSize = sizeof(__type__) * count;\
        size_t rawWriteAmt = WriteRaw((const uint8_t*) buffer, rawWriteSize);\
        return rawWriteAmt / sizeof(__type__);\
    }

class ToolMemStream {
protected:
    std::vector<uint8_t>        m_stream;
    uintptr_t                   m_ptr;
public:
    ToolMemStream();
    
    ~ToolMemStream();
    
    bool Seek(uintptr_t pos);
    
    size_t Length() const;
    
    uintptr_t Tell() const;
    
    void PadToAlignment(uintptr_t alignment);
    
    size_t Write( const ToolMemStream & str );
    
    TOOL_STREAM_WRITE(char)
    TOOL_STREAM_WRITE(int8_t)
    TOOL_STREAM_WRITE(uint8_t)
    TOOL_STREAM_WRITE(int16_t)
    TOOL_STREAM_WRITE(uint16_t)
    TOOL_STREAM_WRITE(int32_t)
    TOOL_STREAM_WRITE(uint32_t)
    TOOL_STREAM_WRITE(int64_t)
    TOOL_STREAM_WRITE(uint64_t)
    TOOL_STREAM_WRITE(float)
    TOOL_STREAM_WRITE(double)
    
    const uint8_t* GetStream() const { return m_stream.size()==0 ? nullptr : &m_stream[0]; }
    
protected:
    size_t WriteRaw(const uint8_t* buffer, size_t size);
};

#endif
