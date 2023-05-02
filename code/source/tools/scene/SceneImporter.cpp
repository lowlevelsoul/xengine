//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "SceneImporter.h"

const SceneImporter::Options SceneImporter::Options::DEFAULT;

//=======================================================================================================================
SceneImporter::SceneImporter( const Options & options_ ) {
    options = options_;
}

//=======================================================================================================================
SceneImporter::~SceneImporter() {
    
}
