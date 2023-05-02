//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

namespace PathUtil {

    std::string::size_type GetLastSeperator( const std::string & path );

    void AppendPath( std::string & lhs, const char * rhs );

    void RemoveExtension( std::string & path );

    bool GetLastPathElement( std::string & elementOut, const std::string & path );

    bool RemoveLastPathElement( std::string & path );

    void SplitPath( std::vector<std::string> & pathElements, const std::string & path );
}


#endif
