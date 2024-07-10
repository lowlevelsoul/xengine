//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//======================================================================================================================

#include "toolapp/ToolMemStream.h"

//======================================================================================================================
ToolMemStream::ToolMemStream() {
    m_ptr = 0;
}

//======================================================================================================================
ToolMemStream::~ToolMemStream() {
    
}

//======================================================================================================================
void ToolMemStream::Save( const char * filename ) {
    file_t file;
    bool_t openOk = FS_FileOpen( &file, filename, "wb");
    xerror( openOk != bool_true, "Error opening %s for writing\n", filename );
    
    FS_FileWrite( &file, &m_stream[0], 1, m_stream.size() );
    FS_FileClose( & file );
}

//======================================================================================================================
size_t ToolMemStream::WriteRaw(const uint8_t* buffer, size_t size) {
    uintptr_t currBufferPos = 0;
    
    while ((m_ptr < m_stream.size()) &&  (currBufferPos < size)) {
        m_stream[m_ptr] = buffer[currBufferPos];
        ++m_ptr;
        ++currBufferPos;
    }
    
    while(currBufferPos < size) {
        m_stream.push_back(buffer[currBufferPos]);
        ++currBufferPos;
        ++m_ptr;
    }
    
    return currBufferPos;
}

//======================================================================================================================
bool ToolMemStream::Seek(uintptr_t pos) {
    if ( pos <= this->m_stream.size() ) {
        m_ptr = pos;
        return true;
    }
    
    return false;
}

//======================================================================================================================
size_t ToolMemStream::Length() const {
    return m_stream.size();
}

//======================================================================================================================
uintptr_t ToolMemStream::Tell() const {
    return m_ptr;
}

//======================================================================================================================
size_t ToolMemStream::Write( const ToolMemStream & str ) {
    return Write( str.GetStream(), str.Length());
}

//======================================================================================================================
void ToolMemStream::PadToAlignment(uintptr_t alignment) {
    
    uintptr_t pos = Tell();
    uintptr_t rem = pos % alignment;
    
    if (rem != 0) {
        uint8_t padByte = 0;
        uintptr_t amt = alignment - rem;
        
        for(uintptr_t i=0; i < amt; ++i) {
            Write(&padByte);
        }
    }
}
