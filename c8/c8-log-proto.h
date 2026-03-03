// Copyright (c) 2016 8th Wall, Inc.
//
// Cross platform utilities for writing to stdout.

#pragma once

#include <capnp/dynamic.h>

namespace c8 {

void C8LogCapnpMessage(capnp::DynamicStruct::Reader value);
void C8LogCapnpMessage(capnp::DynamicList::Reader value);

}  // namespace c8
