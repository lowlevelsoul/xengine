/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Fs.h"
#include "xe/Str.h"
#include "xe/Mem.h"
#include "xe/Sys.h"


/*==========================================================================================================*/
X_API bool FS_MatchFilter(const char* fileName, const char* filter) {
    /* Filter defines a semi-colon separated list of extensions e.g., "txt;cpp" 
       If non-null, we'll use this to check against the extensions of the files found in the directory.
       We'll split the filter string by semi-colons and check if the file's extension matches any of them.
    */  

    char extBuffer[256];

    if(!filter) {
        return true;
    }

    const char* ext = strrchr(fileName, '.');
    if(!ext) {
        return false;
    }
    ext++; // skip the dot
    const char* f = filter;
    while(f && *f) {
        const char* semi = strchr(f, ';');
        size_t len = semi ? (size_t)(semi - f) : strlen(f);

        /* Copy the current filter segment into extBuffer for comparison */
        if(len >= sizeof(extBuffer)) {
            len = sizeof(extBuffer) - 1;
        }
        strncpy(extBuffer, f, len);
        extBuffer[len] = '\0';
        f = semi ? semi + 1 : NULL;

        if(strcasecmp( ext, extBuffer ) == 0) {
            return true;
        }
    }

    return false;
}