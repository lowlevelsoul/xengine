/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "xe/Mem.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

typedef struct render_window_s {
    GLFWwindow* glfw_window;
} render_window_t;

/*==========================================================================================================*/
render_window_t* RenderWindow_Create( int32_t width, int32_t height, bool fullscreen, const char* title ) {
    render_window_t* window = ( render_window_t* )Mem_Alloc( sizeof( render_window_t ) );
    if ( !window ) {
        return NULL;
    }

    if ( !glfwInit() ) {
        Mem_Free( window );
        return NULL;
    }

    /* Set GLFW window hints here if needed, e.g., OpenGL version, profile, etc.
     We don't want opengl - we'll be createing a Vulkan or other context instead */
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE );

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
    window->glfw_window = glfwCreateWindow( width, height, title, monitor, NULL );
    if ( !window->glfw_window ) {
        glfwTerminate();
        Mem_Free( window );
        return NULL;
    }

    return window;
}

/*==========================================================================================================*/
void RenderWindow_Destroy( render_window_t* window ) {
    if ( window->glfw_window ) {
        glfwDestroyWindow( window->glfw_window );
        window->glfw_window = NULL;
    }
    Mem_Free( window );
    glfwTerminate();
}

/*==========================================================================================================*/
void* RenderWindow_GetNativeHandle( render_window_t* window ) {
    return glfwGetCocoaWindow( window->glfw_window );
}

/*==========================================================================================================*/
X_API bool RenderWindow_PumpWindowEvents( render_window_t* window ) {
    if ( glfwWindowShouldClose( window->glfw_window ) ) {
        return false;
    }

    glfwPollEvents();
    return true;
}

/*==========================================================================================================*/
X_API void RenderWindow_GetWindowSize( render_window_t* window, float* width, float* height ) {
    int w, h;
    glfwGetWindowSize( glfwGetCurrentContext(), &w, &h );
    *width = ( float )w;
    *height = ( float )h;
}

/*==========================================================================================================*/
X_API void RenderWindow_GetWindowScale( render_window_t* window, float* scaleX, float* scaleY ) {
    glfwGetWindowContentScale( window->glfw_window, scaleX, scaleY );
}
