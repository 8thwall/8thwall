// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Encapsulation of client-specific OpenGL context methods, like EGL, CGL, and EAGL.

#pragma once

namespace c8 {

using ClientGlFunctionType = void();

// Get a function pointer to an OpenGL extension method, or return nullptr if not available. Note
// that this doesn't mean the method is necessarily available to this context. A client must also
// query glGetString(GL_EXTENSIONS) or eglQueryString(display, EGL_EXTENSIONS) to determine if an
// extension is supported by a particular context or display.
ClientGlFunctionType *clientGlGetProcAddress(char const *procName);

}  // namespace c8
