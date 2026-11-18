/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "glfw/glfw3.h"

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
bool RenderWindow_PumpEvents( render_window_t* window ) {
    if ( !isWindowCreated )
        return false;

    glfwPollEvents();
    return true;
}
