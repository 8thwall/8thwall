// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Example native calls to include in dynamically linked libraries (i.e., DLLs).

#pragma once

#include "bzl/macro/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

C8_PUBLIC const char *c8_exampleStringMethod();
C8_PUBLIC int c8_exampleIntMethod();

#ifdef __cplusplus
}
#endif
