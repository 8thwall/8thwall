// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Methods for loading splat data.

#pragma once

#include "c8/geometry/splat-io.h"
#include "c8/geometry/splat.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// Auto-detects the format and loads the splat data.
SplatRawData loadSplat(const String &filename);
SplatRawData loadSplat(const uint8_t *data, size_t size);

// Returns raw splat data in ply format
SplatRawData loadSplatDrcFile(const String &filename);
SplatRawData loadSplatDrcData(const uint8_t *data, size_t size);
SplatRawData loadSplatDrcData(const Vector<uint8_t> &data);

SplatRawData loadSplatPlyFile(const String &filename);
SplatRawData loadSplatPlyData(const uint8_t *data, size_t size);
SplatRawData loadSplatPlyData(const Vector<uint8_t> &data);

SplatRawData loadSplatSpzFile(const String &filename);
SplatRawData loadSplatSpzData(const uint8_t *data, size_t size);
SplatRawData loadSplatSpzData(const Vector<uint8_t> &data);

PackedGaussians loadSplatSpzFilePacked(const String &filename);
PackedGaussians loadSplatSpzDataPacked(const uint8_t *data, size_t size);
PackedGaussians loadSplatSpzDataPacked(const Vector<uint8_t> &data);

}  // namespace c8
