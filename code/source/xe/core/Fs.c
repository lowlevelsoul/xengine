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

#include "Fs.h"
#include <assert.h>
#include <string.h>

/*---------------------------------------------------------------------------------------------------------------------------------------*/
const char* FS_getExt(const char* pathIn) {
    return strrchr(pathIn, '.');
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
boolean_t FS_appendPath(char* path, size_t pathBuffSize, const char* pathAppend) {
    size_t currPathLen, appendPathLen, pathNewLen;
    char pathTrailingChar, appendLeadingChar;
    boolean_t hasTrailingSep, hasLeadingSep;
    
    assert(path != NULL);
    assert(pathAppend != NULL);
    
    currPathLen = strlen(path);
    appendPathLen = strlen(pathAppend);
    
    pathTrailingChar = path[currPathLen-1];
    appendLeadingChar = pathAppend[0];
    
    hasTrailingSep = (pathTrailingChar == '/') || (pathTrailingChar == '\\');
    hasLeadingSep = (appendLeadingChar == '/') || (appendLeadingChar == '\\');
    
    pathNewLen = currPathLen + appendPathLen;
    
    if ((hasTrailingSep == TRUE) && (hasLeadingSep == TRUE)) {
        /* There is a leading and traing separator - so trim the separator
           from the pathh we're appending */
        ++pathAppend;
        --pathNewLen;
    } else if ((hasTrailingSep == FALSE) && (hasLeadingSep == FALSE)) {
        /* There is neither a trailing or leading path separator, s
           so we need to add one
         */
        if (currPathLen == pathBuffSize) {
            return FALSE;
        }
        
        path[currPathLen++] = FS_FolderSep();
        ++pathNewLen;
    }
    
    /* Check to make sure we have enough space to append the path, and if so,
       errr...append the path!
    */
    if (pathNewLen >= pathBuffSize) {
        return FALSE;
    }
    
    strcpy(&path[currPathLen], pathAppend);
    
    return TRUE;
}

