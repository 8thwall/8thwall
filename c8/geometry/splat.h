// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Data types for splats and common manipulations on them.

#pragma once

#include "c8/color.h"
#include "c8/geometry/octree.h"
#include "c8/geometry/splat-io.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/quaternion.h"
#include "c8/vector.h"

namespace c8 {

struct SplatMetadata {
  int32_t numPoints = 0;
  int32_t maxNumPoints = 0;
  int32_t shDegree = 0;
  int32_t antialiased = false;  // 32 bits to keep four-byte alignment.
  int32_t hasSkybox = false;    // 32 bits to keep four-byte alignment.
  float width = 0;
  float height = 0;
  float depth = 0;
  float centerX = 0;
  float centerY = 0;
  float centerZ = 0;
};

constexpr int SORT_SPLAT_NUM_BINS = 1 << 16;
struct SortSplatScratch {
  Vector<float> depths;
  std::array<float, SORT_SPLAT_NUM_BINS> binHistogram;
  Vector<int32_t> bins;
  std::array<int32_t, SORT_SPLAT_NUM_BINS> binPtrs;
  Vector<uint64_t> depthInds;
  Vector<uint8_t> keepInds;
};

struct SortSplatOptions {
  bool lowToHigh = false;
  float maxDistToOrigin = 0.0f;         // 0.0f means no max.
  float minDistToCamera = 0.0f;         // 0.0f means no min.
  float cullRayX = 0.0f;                // 0.0f means no culling.
  float cullRayY = 0.0f;                // 0.0f means no culling.
  float minSplatSize = 0.0f;            // 0.0f means no culling.
  bool euclideanSort = false;           // If true, sort by euclidean distance, otherwise by depth.
  SortSplatScratch *scratch = nullptr;  // Scratch space for sorting.
};

struct SplatRawData {
  SplatMetadata header;
  Vector<float> positions;
  Vector<float> scales;
  Vector<float> rotations;
  Vector<float> alphas;
  // These are the first spherical harmonics coefficients, so to convert to a color value in the
  // range, 0 to 1: value = 0.5 * 0.282095 * x
  Vector<float> colors;
  // Additional spherical harmonics coefficients for each color channel, tiled, i.e.:
  //   s1n1_r, s1n1_g, s1n1_b, s10_r, s10_g, s10_b, s1p1_r, s1p1_g, s1p1_b, s2n2_r, s2n2_g, ...
  Vector<float> sh;
};

struct SplatSkybox {
  ConstRGBA8888PlanePixels px;
  ConstRGBA8888PlanePixels nx;
  ConstRGBA8888PlanePixels py;
  ConstRGBA8888PlanePixels ny;
  ConstRGBA8888PlanePixels pz;
  ConstRGBA8888PlanePixels nz;
};

SplatSkybox skybox(const uint8_t *data, size_t size);

struct SplatSkyboxData {
  Vector<uint8_t> data;
  SplatSkybox skybox();
};

// A splat represented with collated attributes, each vector is the same length and a single
// element represents the attributes of one splat.
struct SplatAttributes {
  SplatMetadata header;
  Vector<HPoint3> positions;
  Vector<Quaternion> rotations;
  Vector<HVector3> scales;
  Vector<Color> colors;
  Vector<std::array<uint32_t, 2>> shRed;    // 15 components, sh1 5 bits each. remaining 4 bits each
  Vector<std::array<uint32_t, 2>> shGreen;  // 15 components, sh1 5 bits each. remaining 4 bits each
  Vector<std::array<uint32_t, 2>> shBlue;   // 15 components, sh1 5 bits each. remaining 4 bits each
  SplatSkyboxData skybox;
};

// A packed splat is 16 words, so we can pack them into 4 int32 color pixels (rgba)
struct PackedSplat {
  // Pixel 0
  std::array<float, 3> position = {};  // 3 words
  std::array<uint8_t, 4> color = {};   // 1 word

  // Pixel 1
  std::array<uint32_t, 3> encodedCoeffs = {};  // 3 words
  uint32_t padA = 0;                           // 1 word padding

  // Pixel 2
  std::array<uint32_t, 2> shR = {};  // 2 words
  std::array<uint32_t, 2> shG = {};  // 2 words

  // Pixel 3
  std::array<uint32_t, 2> shB = {};   // 2 words
  std::array<uint32_t, 2> padB = {};  // 2 words

  // Number of 4-word pixels needed to represent a single splat.
  static constexpr int stride() { return sizeof(PackedSplat) / (4 * sizeof(int32_t)); }
};

struct SplatTexture {
  SplatMetadata header;
  Vector<uint32_t> sortedIds;
  RGBA32PlanePixelBuffer texture;
  SplatSkyboxData skybox;
};

struct PositionColor {
  // Pixel 0
  std::array<float, 3> position = {};  // 3 words
  std::array<uint8_t, 4> color = {};   // 1 word
};

struct RotationScale {
  // Pixel 0
  std::array<uint32_t, 3> encodedCoeffs = {};  // 3 words
  uint32_t padA = 0;                           // 1 word padding
};

struct SH {
  // Pixel 0
  std::array<uint32_t, 2> sh1 = {};  // 2 words
  std::array<uint32_t, 2> sh2 = {};  // 2 words
};

struct SortedId {
  // Pixel 0
  uint32_t id;
};

struct SplatMultiTexture {
  SplatMetadata header;
  R32PlanePixelBuffer sortedIds;
  RGBA32PlanePixelBuffer positionColor;
  RGBA32PlanePixelBuffer rotationScale;
  RGBA32PlanePixelBuffer shRG;
  RGBA32PlanePixelBuffer shB;
  SplatSkyboxData skybox;
};

// Convert raw splat data into SplatAttributes, including implicit RUB to RUF coordinate conversion.
SplatAttributes splatAttributes(const SplatRawData &d, bool rubToRuf = true);

SplatAttributes splatAttributes(const PackedGaussians &splat, bool rubToRuf);

// Convert raw splat data into a GPU-loadable texture where each splat is represented by two pixels
// and is a serialized PackedSplat struct.
RGBA32PlanePixelBuffer splatTexture(const SplatAttributes &d);
R32PlanePixelBuffer sortedIds(const SplatAttributes &d);
RGBA32PlanePixelBuffer positionColor(const SplatAttributes &d);
RGBA32PlanePixelBuffer rotationScale(const SplatAttributes &d);
RGBA32PlanePixelBuffer sh(const SplatAttributes &d, int color);

// Sort splats by depth (far to near), using the camera matrix to compute depth.
SplatAttributes sortSplatAttributes(
  const SplatAttributes &d,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options);

SplatAttributes mortonSortSplatAttributes(const SplatAttributes &d);

// Sort splats by depth (far to near: high to low), using the camera matrix to compute depth, and
// return the sorted indices.
// @param lowToHigh sort instead from near to far. Set to true when using RUB coordinate system.
// @param out output vector. Can be reused across calls. Does not need to be initialized.
void sortSplatIds(
  const SplatAttributes &loadedAttributes,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options,
  Vector<uint32_t> *out);
// @param out output pixel buffer. Can be reused across calls. Does not need to be initialized.
// returns the number of visible splats
int32_t sortSplatIds(
  const SplatAttributes &d,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options,
  R32PlanePixelBuffer *out);

}  // namespace c8
