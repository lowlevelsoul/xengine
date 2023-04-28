/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#include "Str.h"
#include "mem/Mem.h"
#include "core/Sys.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define STR_MAGIC 0xb0b2feed
#define STR_ALLOC_BLOCK_SIZE 64

typedef struct str_header_s {
    uint32_t        magic;
    uint32_t        flags;
    size_t          length;
    size_t          capacity;
    char *          string;
} str_header_t;

#define Str_GetHeader(CSTR) (str_header_t*)(((uintptr_t)(CSTR)) - sizeof(str_header_t))

/*=======================================================================================================================================*/
size_t Str_CalcStrCapacity( size_t length ) {
    size_t cap = Sys_Align( length, STR_ALLOC_BLOCK_SIZE );
    return cap;
}

/*=======================================================================================================================================*/
/* Just allocs a new string object */
void Str_Alloc( str_header_t ** str, size_t capacity ) {
    size_t cap = Str_CalcStrCapacity( capacity );
    size_t totalAllocSize = sizeof(str_header_t) + cap;
    
    uint8_t * mem = (uint8_t*) Mem_Alloc( totalAllocSize );
    str_header_t * newStr = (str_header_t*) mem;
    
    newStr->magic       = STR_MAGIC;
    newStr->flags       = 0;
    newStr->length      = 0;
    newStr->capacity    = cap;
    newStr->string = (char *) (mem + sizeof(str_header_t));
    
    newStr->string[0] = 0;
    *str = newStr;
}

/*=======================================================================================================================================*/
void Str_EnsureCapacity( str_t * str, size_t desiredCapacity ) {
    if ( *str == NULL ) {
        /* String is not valid - just create a new string */
        str_header_t * strHeader;
        Str_Alloc( &strHeader, desiredCapacity );
        
        strHeader->length = 0;
        strHeader->string[ 0 ] = 0;
        
        *str = strHeader->string;
        return;
    }
    else {
        /* A dynamic string already exists. We might need to alloc more memory */
        
        /* Get the header. We'll use this quite a bit. */
        str_header_t * strHeader = Str_GetHeader( *str );
               
        if ( desiredCapacity < strHeader->capacity ) {
            /* We want less capacity than the string currently has, so do nothing.
               In order to reduce memory thrashing, we avoid another alloc/free here.
             */
            
            if ( desiredCapacity < strHeader->length ) {
                /* Capacity is also less than the length of the string - set a new string length */
                strHeader->length = desiredCapacity;
                strHeader->string[ desiredCapacity ] = 0;
            }
            
            return;
        }
        if ( desiredCapacity > strHeader->capacity ) {
            /* We want to increase the capacity of the original string, which means creating
               a new string object */
            str_header_t * newStr = NULL;
            Str_Alloc( &newStr, desiredCapacity );
        
            if ( strHeader->length > 0 ) {
                /* The existing string has data that we need to copy */
                strncpy( newStr->string, strHeader->string, strHeader->length );
                newStr->length = strHeader->length;
            }
            
            /* Set the returned string to point to the newly allocated string
               and destroy the old one */
            *str = newStr->string;
            Mem_Free( strHeader );
        }
    }
}

/*=======================================================================================================================================*/
void Str_EnsureLength( str_t * str, size_t desiredLength ) {
    if ( *str == NULL ) {
        /* String is not valid - just create a new string */
        str_header_t * strHeader;
        Str_Alloc( &strHeader, desiredLength + 1 );
        
        strHeader->length = desiredLength;
        strHeader->string[ desiredLength ] = 0;
        
        *str = strHeader->string;
        return;
    }
    else {
        /* String already exists */
        
        /* Get the header. We'll use this quite a bit. */
        str_header_t * strHeader = Str_GetHeader( *str );
        
        if ( desiredLength < strHeader->length ) {
            /* We're making the string smaller. We'll just trim the end of the
               string reather than allocating and freeing memory */
            strHeader->length = desiredLength;
            strHeader->string[ desiredLength ] = 0;
        }
        else if ( desiredLength > strHeader->length ) {
            /* String needs to grow. But do we need to allocate or can
               we just use the existing memory? */
            
            if ( desiredLength + 1 > strHeader->capacity ) {
                /* Need to allocate, which means a new string object */
                
                Str_EnsureCapacity( str, desiredLength + 1);
                
                /* Set the length of the new dynamic string */
                strHeader = Str_GetHeader( *str );
                strHeader->length = desiredLength;
                strHeader->string[ desiredLength ] = 0;
            }
            else {
                /* There is enough memory for the string to grow */
                strHeader->length = desiredLength;
                strHeader->string[ desiredLength ] = 0;
            }
        }
    }
}

/*=======================================================================================================================================*/
bool_t Str_IsEmpty( const str_t rhs ) {
    str_header_t * rhsHeader = Str_GetHeader(rhs);
    if ( rhs == NULL ) {
        return true;
    }
    
    assert( rhsHeader->magic == STR_MAGIC );
    
    return rhsHeader->length == 0;
}

/*=======================================================================================================================================*/
size_t Str_GetLength( const str_t rhs ) {
    if ( rhs != NULL ) {
        str_header_t * rhsHeader = Str_GetHeader(rhs);
        assert( rhsHeader->magic == STR_MAGIC );
        return rhsHeader->length;
    }
    
    return 0;
}

/*=======================================================================================================================================*/
void Str_Destroy( str_t * self_ ) {
    str_header_t * header = Str_GetHeader( *self_ );
    assert( header->magic = STR_MAGIC );
    header->magic = 0;
    
    Mem_Free( header );
    *self_ = NULL;
}

/*=======================================================================================================================================*/
void Str_SetLength( str_t * self_, size_t length ) {
    Str_EnsureLength( self_, length );
}

/*=======================================================================================================================================*/
void Str_SetCapacity( str_t * self_, size_t capacity ) {
    Str_EnsureCapacity( self_, capacity );
}

/*=======================================================================================================================================*/
void Str_CopyCStr( str_t * self_, const char * rhs ) {
    size_t rhsLen = ( rhs != NULL ) ? strlen( rhs ) : 0;
    if (rhsLen == 0 ) {
        return;
    }
    
    Str_EnsureLength( self_, rhsLen );
    strncpy( *self_, rhs, rhsLen );
}

/*=======================================================================================================================================*/
void Str_Copy( str_t * self_, const str_t rhs ) {
    const str_header_t * rhsHeader;
    if ( rhs == NULL ) {
        return;
    }
    
    rhsHeader = Str_GetHeader(rhs);
    Str_EnsureLength( self_, rhsHeader->length );
    strncpy( *self_, rhs, rhsHeader->length );
}

/*=======================================================================================================================================*/
void Str_Append( str_t * self_, const str_t rhs ) {
    size_t selfOrigLength = 0;
    size_t rhsLength = 0;
    size_t newLength = 0;
    
    if ( Str_IsEmpty( rhs ) == true ) {
        /* Nothing to do is rhs is empty */
        return;
    }
    
    if ( Str_IsEmpty( *self_ ) == true ) {
        /* Destination string is empty, so just copy */
        Str_Copy( self_, rhs );
        return;
    }
    /* If we get this far, we're appending a valid string in rhs to
       an existing string in self_
     */
    
    /* Resize self_ so we can add rhs */
    selfOrigLength = Str_GetLength( *self_ );
    rhsLength = Str_GetLength( rhs );
    newLength = selfOrigLength + rhsLength;
    
    Str_SetLength( self_, newLength );
    
    /* Copy over rhs to the correct point in self_ */
    strncpy( (*self_) + selfOrigLength, rhs, rhsLength );
}

/*=======================================================================================================================================*/
void Str_AppendCStr(str_t * self_, const char * rhs ) {
    size_t selfOrigLength = 0;
    size_t rhsLength = (rhs == NULL) ? 0 : strlen(rhs);
    size_t newLength = 0;
    
    if ( rhsLength == 0 ) {
        /* Nothing to do is rhs is empty */
        return;
    }
    
    if ( Str_IsEmpty( *self_ ) == true ) {
        /* Destination string is empty, so just copy */
        Str_CopyCStr( self_, rhs );
        return;
    }
    /* If we get this far, we're appending a valid string in rhs to
       an existing string in self_
     */
    
    /* Resize self_ so we can add rhs */
    selfOrigLength = Str_GetLength( *self_ );
    newLength = selfOrigLength + rhsLength;
    
    Str_SetLength( self_, newLength );
    
    /* Copy over rhs to the correct point in self_ */
    strncpy( (*self_) + selfOrigLength, rhs, rhsLength );
}


/*=======================================================================================================================================*/
void Str_Concat(str_t * dst, const str_t lhs, const str_t rhs ) {
    bool_t lhsEmpty = Str_IsEmpty( lhs );
    bool_t rhsEmpty = Str_IsEmpty( rhs );
    
    if ( lhsEmpty != rhsEmpty ) {
        /* Only one string is valid - we can just perform a copy */
        if ( lhsEmpty == true ) {
            Str_Copy( dst, rhs );
        }
        else {
            Str_Copy( dst, lhs );
        }
    }
    else if ( lhsEmpty == false && rhsEmpty == false ) {
        /* Both strings contain something -  we can concatonate */
        size_t lhsLength = Str_GetLength( lhs );
        size_t rhsLength = Str_GetLength( rhs );
        size_t newLength = lhsLength + rhsLength;
        
        Str_SetLength( dst, newLength );
        strncpy( *dst, lhs, lhsLength );
        strncpy( (*dst) + lhsLength, rhs, rhsLength );
    }
    
    /* Note - if both lhs & rhs are empty, we don't do anything */
}

/*=======================================================================================================================================*/
void Str_ConcatCStr(str_t * dst, const char * lhs, const char * rhs ) {
    size_t lhsLength = ( lhs == NULL ) ? 0 : strlen( lhs );
    size_t rhsLength = ( rhs == NULL ) ? 0 : strlen( rhs );
    
    if ( lhsLength == 0 && rhsLength != 0 ) {
        /* Both strings are empty - do nothing and exit */
        return;
    }
    else if ( lhsLength != 0 & rhsLength != 0 ) {
        /* Both strings contain something - concatonate */
        size_t newLength = lhsLength + rhsLength;
        
        Str_SetLength( dst, newLength );
        strncpy( *dst, lhs, lhsLength );
        strncpy( (*dst) + lhsLength, rhs, rhsLength );
    }
    else {
        /* Only one string is valid - just perform a copy */
        if ( lhsLength == 0 ) {
            Str_CopyCStr( dst, rhs );
        }
        else {
            Str_CopyCStr( dst, lhs );
        }
    }
}

/*=======================================================================================================================================*/
void Str_SubStr( str_t * lhs, const str_t rhs, uintptr_t start, size_t count ) {
    size_t rhsLength = Str_GetLength( rhs );
    assert( start + count <= rhsLength );
    assert( *lhs != rhs );
    
    Str_SetLength( lhs, count );
    strncpy( *lhs, rhs + start, count );
}

/*=======================================================================================================================================*/
void Str_Left( str_t * lhs, const str_t rhs, size_t size ) {
    Str_SubStr( lhs, rhs, 0, size );
}

/*=======================================================================================================================================*/
void Str_Right( str_t * lhs, const str_t rhs, size_t size ) {
    Str_SubStr( lhs, rhs, Str_GetLength( rhs ) - size , size );
}

/*=======================================================================================================================================*/
intptr_t Str_FindChar(const str_t lhs, const char rhs ) {
    size_t len = Str_GetLength( lhs );
    if ( len == 0 ) {
        return STR_NOT_FOUND;
    }
    
    for ( intptr_t n = 0; n < len; ++n ) {
        if ( lhs[n] == rhs ) {
            return n;
        }
    }
    
    return STR_NOT_FOUND;
}

/*=======================================================================================================================================*/
intptr_t Str_RFindChar(const str_t lhs, const char rhs ) {
    size_t len = Str_GetLength( lhs );
    if ( len == 0 ) {
        return STR_NOT_FOUND;
    }
    
    for ( intptr_t n = len-1; n >= 0; --n ) {
        if ( lhs[n] == rhs ) {
            return n;
        }
    }
    
    return STR_NOT_FOUND;
}

/*=======================================================================================================================================*/
void Str_ToLower(  str_t self_ ) {
    size_t len = Str_GetLength( self_ );
    for ( int n = 0; n < len; ++n ) {
        if (self_[n] >= 'A' && self_[n] <= 'Z' ) {
            self_[n] = 'a' + self_[n] - 'A';
        }
    }
}

/*=======================================================================================================================================*/
void Str_ToUpper(  str_t self_ ) {
    size_t len = Str_GetLength( self_ );
    for ( int n = 0; n < len; ++n ) {
        if (self_[n] >= 'a' && self_[n] <= 'z' ) {
            self_[n] = 'A' + self_[n] - 'a';
        }
    }
}

/*=======================================================================================================================================*/
void Str_ReplaceChar( str_t self_, char replaceChar, char replaceWith ) {
    size_t len = Str_GetLength( self_ );
    
    for ( int n = 0; n < len; ++n ) {
        if ( self_[n] == replaceChar ) {
            self_[n] = replaceWith;
        }
    }
}

/*=======================================================================================================================================*/
void Str_AppendPathCStr( str_t * self_, const char * pathItem ) {
    size_t pathItemLength = ( pathItem == NULL ) ? 0 : strlen( pathItem );
    if ( pathItemLength == 0 ) {
        return;
    }
    
    if ( Str_IsEmpty( *self_ ) == true ) {
        Str_CopyCStr( self_, pathItem );
        return;
    }
    
    /* If we get to here we have to properly append the path */
    str_header_t * selfHeader = Str_GetHeader( *self_ );
    bool_t selfHasSep = (*self_)[ selfHeader->length - 1 ] == '/' || (*self_)[ selfHeader->length - 1 ] == '\\';
    bool_t pathItemHasSep = pathItem[0] == '/' || pathItem[0] == '\\';
    
    if ( selfHasSep == pathItemHasSep ) {
        if ( selfHasSep == true ) {
            /* Both the string we're appending and the path item have a separator,
               and we only want one. So just strip the leading separator from pathItem and
               append */
            Str_AppendCStr( self_, pathItem + 1 );
        }
        else {
            /* Neither has a separator, so we need to add one of our own */
            Str_AppendCStr( self_, "/" );
            Str_AppendCStr( self_, pathItem );
        }
            
    }
    else {
        Str_AppendCStr( self_, pathItem );
    }
}


/*=======================================================================================================================================*/
void Str_AppendPath( str_t * self_, str_t rhs ) {
    if ( Str_IsEmpty( rhs ) == true ) {
        /* Rhs is empty - so just bail */
        return;
    }
    
    if ( Str_IsEmpty( *self_) == true ) {
        /* String we're appending is empty - just copy */
        Str_Copy( self_, rhs );
        return;
    }
    
    /* If we get to here we have to properly append the path */
    str_header_t * selfHeader = Str_GetHeader( *self_ );
    char selfLastChar = (*self_)[ selfHeader->length - 1 ];
    bool_t selfHasSep = selfLastChar == '/' || selfLastChar == '\\';
    bool_t rhsHasSep = rhs[0] == '/' || rhs[0] == '\\';
    
    if ( selfHasSep == rhsHasSep ) {
        if ( selfHasSep == true ) {
            /* Both the string we're appending and the path item have a separator,
               and we only want one. So just strip the leading separator from pathItem and
               append */
            
            /* !!! Nasty - we call the CStr version of append here as a hack !!! */
            Str_AppendCStr( self_, rhs + 1 );
        }
        else {
            /* Neither has a separator, so we need to add one of our own */
            Str_AppendCStr( self_, "/" );
            Str_Append( self_, rhs );
        }
            
    }
    else {
        /* Eiher one or the other strings has a separator - just append */
        Str_Append( self_, rhs );
    }
}


/*=======================================================================================================================================*/
intptr_t Str_PathFindExtension( const str_t self_ ) {
    if ( Str_IsEmpty( self_ ) == true ) {
        return STR_NOT_FOUND;
    }
    
    intptr_t pos = Str_RFindChar( self_, '.' );
    if ( pos != STR_NOT_FOUND ) {
        ++pos;
    }
    
    /* TODO: Scan the string after the extension to see if we have any path separators. */
    
    return pos;
}


/*=======================================================================================================================================*/
bool_t Str_PathRemoveLastElement( str_t * self_ ) {
    const char * lastSep0 = NULL;
    const char * lastSep1 = NULL;
    const char * lastSep = NULL;
    
    if ( *self_ == NULL ) {
        return false;
    }
    
    lastSep0 = strrchr( *self_, '\\' );
    lastSep1 = strrchr( *self_, '/' );
    
    if ( lastSep0 == NULL && lastSep1 == NULL ) {
        /* No final element. So return */
        return false;
    }
    
    if ( lastSep0 != NULL && lastSep1 != NULL ) {
        /* We found both seperator types. We need to find the last one */
        lastSep = ( lastSep0 > lastSep1 ) ? lastSep0 : lastSep1;
    }
    else {
        /* Only found one of the seperator types, so determine which one */
        lastSep = ( lastSep0 != NULL ) ? lastSep0 : lastSep1;
    }
    
    size_t newLength = ( size_t ) ( lastSep - *self_);
    Str_SetLength( self_, newLength );
    
    return true;
}


/*=======================================================================================================================================*/
bool_t Str_PathRemoveExtension( str_t * self_ ) {
    intptr_t extPos = Str_PathFindExtension( *self_ );
    if (extPos == STR_NOT_FOUND ) {
        return false;
    }
    
    Str_SetLength( self_, extPos - 1 );
    return true;
}

/*=======================================================================================================================================*/
bool_t Str_PathGetExtension( str_t * ext, const str_t path ) {
    intptr_t extPos = Str_PathFindExtension( path );
    if (extPos == STR_NOT_FOUND ) {
        return false;
    }
    
    /* !!! WARNING !!! We're using the slow copy just to see if this works */
    /* TODO: fix this to be more intelligent and use left/right/substr */
    Str_CopyCStr( ext, path+extPos);
    return true;
}


//==========================================================================================================================================
void Str_VFormatArgs( str_t * self_, const char * fmt, va_list vaArgs ) {
    /* initialize use of the variable argument array */
    va_list vaArgsCopy;

    /* reliably acquire the size
       from a copy of the variable argument array
       and a functionally reliable call to mock the formatting */
    va_copy( vaArgsCopy, vaArgs );
    size_t len = vsnprintf( NULL, 0, fmt, vaArgsCopy );
    va_end( vaArgsCopy );
    
    Str_SetLength( self_, len );
   
    /* return a formatted string without risking memory mismanagement
       and without assuming any compiler or platform specific behavior */
    vsnprintf( *self_, len + 1, fmt, vaArgs );
    va_end( vaArgs );
}

//==========================================================================================================================================
void Str_VFormat( str_t * self_, const char * fmt, ... ) {
    /* initialize use of the variable argument array */
    va_list vaArgs;
    va_start( vaArgs, fmt );
    
    Str_VFormatArgs( self_, fmt, vaArgs );
}
