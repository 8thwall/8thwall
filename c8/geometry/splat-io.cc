#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "splat-io.h",
  };
  deps = {
    "//c8/spz:load-spz",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x5aee4fb3);

#include "c8/geometry/splat-io.h"

namespace c8 {

// Loads Gaussian splat from a vector of bytes in packed format.
spz::GaussianCloud loadGaussianCloud(const std::vector<uint8_t> &data) {
  // Currently this loads without cooordiante conversion, assuming that callers expect the data
  // in RUB cooordinates, but we should update callers to be explicit about this.
  return spz::loadSpz(data, {});
}

// Loads Gaussian splat from a vector of bytes in packed format.
PackedGaussians loadSplatPacked(const std::string &filename) {
  return spz::loadSpzPacked(filename);
}

PackedGaussians loadSplatPacked(const uint8_t *data, int size) {
  return spz::loadSpzPacked(data, size);
}

PackedGaussians loadSplatPacked(const std::vector<uint8_t> &data) {
  return spz::loadSpzPacked(data);
}

// Loads Gaussian splat from a file in packed format
spz::GaussianCloud loadGaussianCloud(const std::string &filename) {
  // Currently this loads without cooordiante conversion, assuming that callers expect the data
  // in RUB cooordinates, but we should update callers to be explicit about this.
  return spz::loadSpz(filename, {});
}

// Saves Gaussian splat data in .ply format
void saveSplatToPLY(const spz::GaussianCloud &data, const std::string &filename) {
  // Currently this saves without cooordiante conversion, assuming that the data is already provided
  // in RDF cooordinates, but we should update callers to be explicit about this.
  spz::saveSplatToPly(data, {}, filename);
}

// Loads Gaussian splat data in .ply format
spz::GaussianCloud loadSplatFromPLY(const std::string &filename) {
  // Currently this loads without cooordiante conversion, assuming that callers expect the data
  // in RDF cooordinates, but we should update callers to be explicit about this.
  return spz::loadSplatFromPly(filename, {});
}

}  // namespace c8
