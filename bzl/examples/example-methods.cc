// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/examples/example-methods.h"

namespace c8 {

const char *exampleString() { return "This is a native msg"; }
int exampleInt() { return 42; }

}  // namespace c8

extern "C" {

int c8_exampleInt() { return c8::exampleInt(); }

const char *c8_exampleString() { return c8::exampleString(); }

}  // extern "C"
