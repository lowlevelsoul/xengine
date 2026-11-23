/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

typedef struct render_window_metal_s {
    GLFWwindow* window;
} render_window_metal_t;

static render_window_metal_t theWindow;
static bool isWindowCreated = false;

/*==========================================================================================================*/
void RenderWindow_Create( render_params_t* params ) {
    if ( isWindowCreated ) {
        return;
    }

    if ( !glfwInit() ) {
        return;
    }
    
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE );

    theWindow.window = glfwCreateWindow( params->width, params->height, "Meh", NULL, NULL );
    if ( !theWindow.window ) {
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
render_window_t* Render_GetWindow( void ) {
    if ( !isWindowCreated ) {
        return NULL;
    }

    return ( render_window_t* )&theWindow;
}

/*==========================================================================================================*/
void * RenderWindow_GetNativeHandle() {
    NSWindow * nsWindow = glfwGetCocoaWindow( theWindow.window );
    return ( __bridge void * ) nsWindow;
}

/*==========================================================================================================*/
bool RenderWindow_PumpEvents( render_window_t* window ) {
    if ( !isWindowCreated )
        return false;

    glfwPollEvents();
    if ( glfwWindowShouldClose( (( render_window_metal_t *) window )->window ) ) {
        return false;
    }
    
    return true;
}
