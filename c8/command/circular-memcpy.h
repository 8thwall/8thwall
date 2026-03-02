// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Circular memcpy functions for use with the CommandBuffer and TransferBuffer classes.

#pragma once

namespace c8 {

// Copy `size` bytes from `src` to `dst`, wrapping around the buffer if necessary. Returns a pointer
// to the next location to try further stores.
char *circularMemcpyStore(
  char *dst, const char *src, std::size_t size, char *bufferStart, const char *bufferEnd);

// Read `size` bytes from `src` to `dst`, wrapping around the buffer if necessary. Returns a pointer
// to the next location to try further loads.
const char *circularMemcpyLoad(
  char *dst, const char *src, std::size_t size, const char *bufferStart, const char *bufferEnd);

}  // namespace c8
