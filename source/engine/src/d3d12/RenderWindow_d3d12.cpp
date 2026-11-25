/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "xe/Sys.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3.h"
#include "glfw/glfw3native.h"

typedef struct render_window_d3d12_s {
    GLFWwindow* window;
} render_window_d3d12_t;

static render_window_d3d12_t theWindow;
static bool isWindowCreated = false;

/*==========================================================================================================*/
void RenderWindow_Create( render_params_t* params ) {
    if ( isWindowCreated ) {
        return;
    }

    if ( !glfwInit() ) {
        Sys_PrintCat( "ERROR", "Failed to initialise GLFW." );
        return;
    }

    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

    theWindow.window = glfwCreateWindow( params->width, params->height, "Meh", NULL, NULL );
    if ( !theWindow.window ) {
        Sys_PrintCat( "ERROR", "Failed to create render window." );
        glfwTerminate();
        return;
    }

    isWindowCreated = true;
}

/*==========================================================================================================*/
void RenderWindow_Destroy( void ) {
    if ( !isWindowCreated ) {
        return;
    }

    glfwDestroyWindow( theWindow.window );
    glfwTerminate();
    isWindowCreated = false;
}

/*==========================================================================================================*/
void* RenderWindow_GetNativeHandle( void ) {
    if ( !isWindowCreated ) {
        return NULL;
    }

    return glfwGetWin32Window( theWindow.window );
}

/*==========================================================================================================*/
render_window_t* Render_GetWindow( void ) {
    if ( !isWindowCreated ) {
        return NULL;
    }

    return ( render_window_t* )&theWindow;
}

/*==========================================================================================================*/
bool RenderWindow_PumpEvents( render_window_t* window ) {
    if ( !isWindowCreated )
        return false;

    glfwPollEvents();
    return true;
}
