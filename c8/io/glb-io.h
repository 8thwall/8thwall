// Copyright (c) 2025 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Read/write glb files. This is a minimal implementation, and not all GLB features are supported.

#pragma once

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

struct GlbRawData {
  String json;
  Vector<uint8_t> bin;

  // Returns true if the GLB has no data, which might indicate that the file was invalid.
  bool empty() const { return json.empty() && bin.empty(); }
};

// Low level methods that read basic data out of the GLB file without interpreting it.
GlbRawData readGlbRaw(const String &filename);
GlbRawData readGlbRaw(const Vector<uint8_t> &data);
GlbRawData readGlbRaw(const uint8_t *data, size_t size);

}  // namespace c8
