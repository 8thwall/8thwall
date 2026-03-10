// Copyright © 2023 Niantic, Inc. All rights reserved.
#pragma once

#include "c8/spz/load-spz.h"

namespace c8 {

// Represents a single inflated gaussian. Each gaussian has 236 bytes. Although the data is easier
// to intepret in this format, it is not more precise than the packed format, since it was inflated.
using UnpackedGaussian = spz::UnpackedGaussian;

// Represents a single low precision gaussian. Each gaussian has exactly 64 bytes, even if it does
// not have full spherical harmonics.
using PackedGaussian = spz::PackedGaussian;

// Represents a full splat with lower precision. Each splat has at most 64 bytes, although splats
// with fewer spherical harmonics degrees will have less. The data is stored non-interleaved.
using PackedGaussians = spz::PackedGaussians;

// Loads Gaussian splat from a vector of bytes in packed format.
spz::GaussianCloud loadGaussianCloud(const std::vector<uint8_t> &data);

// Loads Gaussian splat from a vector of bytes in packed format.
PackedGaussians loadSplatPacked(const std::string &filename);
PackedGaussians loadSplatPacked(const uint8_t *data, int size);
PackedGaussians loadSplatPacked(const std::vector<uint8_t> &data);

// Loads Gaussian splat from a file in packed format
spz::GaussianCloud loadGaussianCloud(const std::string &filename);

// Saves Gaussian splat data in .ply format
void saveSplatToPLY(const spz::GaussianCloud &data, const std::string &filename);

// Loads Gaussian splat data in .ply format
spz::GaussianCloud loadSplatFromPLY(const std::string &filename);

}  // namespace c8
