// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "glfw-extra.h"

#include <GLFW/glfw3.h>

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include "glfw-extra-apple.h"
// Uncomment the following in the future as support is added.
// #elif X11
// #define GLFW_EXPOSE_NATIVE_X11
// #elif WAYLAND
// #define GLFW_EXPOSE_NATIVE_WAYLAND
// #elif WINDOWS
// #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

// Return a pointer to a GLFW window's native platform window (or layer) that can be passed into
// eglGetPlatformDisplay and eglCreateWindowSurface.
void *nc_getGlfwNativeWindow(GLFWwindow *window) {
#if defined(__APPLE__)
  // Get a Metal layer from CoreAnimation and return it.
  return nc_getMetalLayer(glfwGetCocoaWindow(window));
// Uncomment the following in the future as support is added.
//#elif X11
//    // Do something with an x11_display and x11_window.
//    Display *x11_display = glfwGetX11Display();
//    Window x11_window = glfwGetX11Window(window);
//#elif WAYLAND
//    // Do something with a w_display and x11_window.
//    struct wl_display *wayland_display = glfwGetWaylandDisplay();
//    struct wl_surface *wayland_surface = glfwGetWaylandWindow(window);
//#elif WINDOWS
//    HWND hwnd = glfwGetWin32Window(window);
//    HINSTANCE hinstance = GetModuleHandle(NULL);
#else
#error "Unsupported platform"
  return NULL;
#endif
}
