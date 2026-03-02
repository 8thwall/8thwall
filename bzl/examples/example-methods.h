// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Example native calls to use through Apple C++/Swift bridge, Windows DLL export, and Android JNI
// bridge.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

const char *c8_exampleString();
int c8_exampleInt();

#ifdef __cplusplus
}  // extern "C"
#endif
