// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Checks if an OpenGL extension is available to the context.

#pragma once

namespace c8 {

// Returns true if an OpenGl extension is available in the current GL context.
bool hasGlExtension(const char *extensionName);

}  // namespace c8
