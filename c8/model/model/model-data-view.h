// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include "c8/geometry/mesh-types.h"
#include "c8/geometry/splat.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/vector.h"

namespace c8 {

template <typename T>
struct VectorView {
  const T *data = nullptr;
  size_t size = 0;
};

struct MeshGeometryView {
  VectorView<HPoint3> points;
  VectorView<Color> colors;
  VectorView<HVector3> normals;
  VectorView<HVector2> uvs;
  VectorView<MeshIndices> triangles;
};

struct MeshDataView {
  MeshGeometryView geometry;
  ConstRGBA8888PlanePixels texture;
};

struct SplatAttributesView {
  SplatMetadata header;
  VectorView<HPoint3> positions;
  VectorView<Quaternion> rotations;
  VectorView<HVector3> scales;
  VectorView<Color> colors;
  VectorView<std::array<uint32_t, 2>> shRed;
  VectorView<std::array<uint32_t, 2>> shGreen;
  VectorView<std::array<uint32_t, 2>> shBlue;
  SplatSkybox skybox;
};

struct SplatTextureView {
  SplatMetadata header;
  VectorView<uint32_t> sortedIds;
  ConstRGBA32PlanePixels texture;
  VectorView<uint8_t> interleavedAttributeData;  // This is another view on the texture data.
  SplatSkybox skybox;
};

struct SplatMultiTextureView {
  SplatMetadata header;
  ConstR32PlanePixels sortedIds;
  ConstRGBA32PlanePixels positionColor;
  ConstRGBA32PlanePixels rotationScale;
  ConstRGBA32PlanePixels shRG;
  ConstRGBA32PlanePixels shB;
  SplatSkybox skybox;
};

struct PointCloudGeometryView {
  VectorView<HPoint3> points;
  VectorView<Color> colors;
  VectorView<HVector3> normals;
};

}  // namespace c8
