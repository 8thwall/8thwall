// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Compress and decompress data.

#pragma once

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

bool compressGzipped(const String &data, Vector<uint8_t> *out);
bool compressGzipped(const Vector<uint8_t> &data, Vector<uint8_t> *out);
bool compressGzipped(const uint8_t *data, size_t size, Vector<uint8_t> *out);

bool decompressGzipped(const Vector<uint8_t> &compressed, String *out);
bool decompressGzipped(const Vector<uint8_t> &compressed, Vector<uint8_t> *out);
bool decompressGzipped(const uint8_t *compressed, size_t size, String *out);
bool decompressGzipped(const uint8_t *compressed, size_t size, Vector<uint8_t> *out);

// Decompress raw data without a gzip header.
bool decompressGzippedRaw(const Vector<uint8_t> &compressed, Vector<uint8_t> *out);
bool decompressGzippedRaw(const uint8_t *compressed, size_t size, Vector<uint8_t> *out);

}  // namespace c8
