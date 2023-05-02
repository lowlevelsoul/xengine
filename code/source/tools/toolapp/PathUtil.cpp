//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#include "toolapp/PathUtil.h"

namespace PathUtil {


    //======================================================================================================================
    std::string::size_type GetLastSeperator( const std::string & path ) {
        std::string::size_type sep1Pos = path.rfind('/');
        std::string::size_type sep2Pos = path.rfind('\\');
        std::string::size_type lastSepPos = std::string::npos;
        
        if ( sep1Pos == std::string::npos && sep1Pos == std::string::npos ) {
            return std::string::npos;
        }
        else if ( sep1Pos == std::string::npos ) {
            lastSepPos = sep2Pos;
        }
        else if ( sep2Pos == std::string::npos ) {
            lastSepPos = sep1Pos;
        }
        else {
            lastSepPos = ( sep1Pos > sep2Pos ) ? sep1Pos : sep2Pos;
        }
        
        return lastSepPos;
    }

    //======================================================================================================================
    void AppendPath( std::string & lhs, const char * rhs ) {
        if ( lhs.empty() == false ) {
            bool lhsHasSep = lhs.back() == '/' || lhs.back() == '\\';
            bool rhsHasSep = rhs[0] == '/' || rhs[0] == '\\';
            
            if ( lhsHasSep == false && rhsHasSep == false ) {
                lhs.push_back('/');
                lhs.append( rhs );
            }
            else if ( lhsHasSep == true && rhsHasSep == true ) {
                lhs.append( rhs+1 );
            }
            else {
                lhs.append( rhs );
            }
        }
        else {
            lhs = rhs;
        }
     }

    //======================================================================================================================
    void RemoveExtension( std::string & path ) {
        std::string::size_type pos = path.rfind('.');
        if ( pos == std::string::npos ) {
            return;
        }
        
        path.resize( pos );
    }

    //======================================================================================================================
    bool GetLastPathElement( std::string & elementOut, const std::string & path ) {
        std::string::size_type lastSepPos = GetLastSeperator( path );
        if ( lastSepPos == std::string::npos ) {
            return false;
        }
        
        std::string::size_type elementStart = lastSepPos + 1;
        elementOut = path.substr( elementStart, path.size() - elementStart );
        
        return true;
    }

    //======================================================================================================================
    bool RemoveLastPathElement( std::string & path ) {
        std::string::size_type lastSepPos = GetLastSeperator( path );
        if ( lastSepPos == std::string::npos ) {
            return false;
        }
        
        path.resize( lastSepPos - 1 );
        return true;        
    }

    //======================================================================================================================
    void SplitPath( std::vector<std::string> & pathElements, const std::string & path ) {
        
        std::string::const_iterator elStart = path.begin();
        std::string::const_iterator elEnd = path.begin();
        
        while ( elStart < path.end() ) {
            if (*elEnd == '\\' || *elEnd == '/' ) {
                // Found a path seperator, so let's add the string to the list
                if ( elEnd != elStart ) {
                    // Only add a path element if the string isn't zero-length
                    std::string element;
                    
                    std::string::size_type startPos = elStart - path.begin();
                    std::string::size_type endPos = elEnd - path.begin();
                    std::string::size_type length = endPos - startPos;
                    
                    element = path.substr( startPos, length );
                    
                    if ( element == ".." && pathElements.empty() == false ) {
                        // If we have a parent folder spec, remove the last folder that we encountered
                        pathElements.pop_back();
                    }
                    else if ( element != "." ) {
                        // Add the path element if it isn't just a current folder specifier
                        pathElements.push_back( element );
                    }
                }
                
                ++elEnd;
                elStart = elEnd;
            }
            
            ++elEnd;
        }
    }



}
