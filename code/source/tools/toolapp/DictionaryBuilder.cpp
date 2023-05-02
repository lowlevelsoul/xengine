//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "toolapp/DictionaryBuilder.h"

//=======================================================================================================================
DictionaryBuilder::DictionaryBuilder() {
    
}

//=======================================================================================================================
DictionaryBuilder::~DictionaryBuilder() {
    
}

//=======================================================================================================================
void DictionaryBuilder::SetDictionaryType( uint64_t type_ ) {
    type = type_;
}

//=======================================================================================================================
void DictionaryBuilder::AddEntry( const char * name, const void * entryData, size_t entryDataSize ) {
    
    uint64_t nameHash = FH64_CalcFromString( name );
    
    // Add the entry name hash to the name map
    uint32_t entryCount = ( uint32_t ) entryNameArray.size();
    
    Array<uint64_t>::iterator_t insertPos = entryNameArray.InsertSorted( nameHash );
    int32_t insertPosIndex = (int32_t) ( insertPos - entryNameArray.begin() );
    entryNameMap.Insert( entryNameMap.begin() + insertPosIndex, entryCount );
    
    // Wrie the entry info
    uint32_t dataOffset = ( uint32_t ) data.Length();
    uint32_t dataSize = ( uint32_t ) entryDataSize;
    
    entries.Write( &dataOffset );
    entries.Write( &dataSize );
    
    // Write the entry data
    data.Write( ( const uint8_t * ) entryData, entryDataSize );
}

//=======================================================================================================================
void DictionaryBuilder::WriteToStream( ToolMemStream & str ) {
    Header header;
    memset( &header, 0, sizeof(header) );
    
    header.magic        = Dictionary::MAGIC;
    header.type         = type;
    header.version      = Dictionary::VERSION;
    header.entryCount   = (uint32_t) entryNameArray.size();
    
    uint64_t headerOffs = str.Tell();
    WriteHeader( str, header );
    
    header.offsNameArray = ( uint32_t ) ( str.Tell() - headerOffs );
    str.Write( &entryNameArray[0], entryNameArray.size() );
    
    header.offsNameMap = ( uint32_t ) ( str.Tell() - headerOffs );
    str.Write( &entryNameMap[0], entryNameMap.size() );
    
    header.offsEntries = ( uint32_t ) ( str.Tell() - headerOffs );
    str.Write( entries );
    
    header.offsData = ( uint32_t ) ( str.Tell() - headerOffs );
    str.Write( data );
    
    // Re-write the header
    uintptr_t endPos = str.Tell();
    str.Seek( headerOffs );
    WriteHeader( str, header );
    str.Seek( endPos );
}

//=======================================================================================================================
void DictionaryBuilder::WriteHeader( ToolMemStream & str, const Header & header ) {
    str.Write( &header.magic );
    str.Write( &header.type );
    
    str.Write( &header.version );
    str.Write( &header.dataSize );
    str.Write( &header.entryCount );
    str.Write( &header.offsEntries );
    str.Write( &header.offsNameArray );
    str.Write( &header.offsNameMap );
    str.Write( &header.offsData );
    str.Write( &header.pad );
}
