//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#ifndef __MODELCONVAPP_H__
#define __MODELCONVAPP_H__

#include "modelbuilder/ModelBuilder.h"
#include "modelbuilder/SkeletonBuilder.h"

class ModelBuilderApp : public ToolApp {
public:
    enum MODE {
        MODE_MODEL,
        MODE_SKELETON,
        MODE_INFO
    };
    
	ModelBuilderApp();

	virtual ~ModelBuilderApp();

	virtual bool HandleArg( Arg * arg, int32_t argId );

	virtual bool Process();

	virtual bool DisplayBuiltinArgHelp( TOOL_ARG_ID argId );

	void DisplayHelpText( int32_t argId );

	void BuildModel();

public:
    MODE                            m_mode;
	ModelBuilder::Options	        m_buildOptions;
	float				            m_scale;
	bool				            m_flipFaces;
	bool				            m_logInfo;
	bool				            m_genNormals;
	bool				            m_cullMixamo;
};


#endif
