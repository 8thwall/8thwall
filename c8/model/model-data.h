// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include "c8/geometry/mesh-types.h"
#include "c8/geometry/splat.h"
#include "c8/model/model-data-view.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/vector.h"

namespace c8 {

struct MeshData {
  MeshGeometry geometry;
  RGBA8888PlanePixelBuffer texture;
};

struct ModelData {
  std::unique_ptr<MeshData> mesh;
  std::unique_ptr<SplatAttributes> splatAttributes;
  std::unique_ptr<SplatTexture> splatTexture;
  std::unique_ptr<SplatMultiTexture> splatMultiTexture;
  std::unique_ptr<PointCloudGeometry> pointCloud;
  std::unique_ptr<SplatOctreeNode> octree;
};

// Serialize model data to a single buffer that can be transferred between threads. This is
// optimized for reading data out in memory blocks to avoid extraneous copies.
// This is optimized to minimize the amount of times data is copied. E.g. compared to capnp, we
// copy data once instead of twice (once for a builder and once for a reader).
int serialize(const ModelData &model, Vector<uint8_t> *outBuffer);

// Read serialized model data directly from a serialized buffer.
bool hasMesh(const uint8_t *serializedModelDataPtr);
MeshDataView meshView(const uint8_t *serializedModelDataPtr);

bool hasSplatAttributes(const uint8_t *serializedModelDataPtr);
SplatAttributesView splatAttributesView(const uint8_t *serializedModelDataPtr);

bool hasSplatTexture(const uint8_t *serializedModelDataPtr);
SplatTextureView splatTexture(const uint8_t *serializedModelDataPtr);

bool hasSplatMultiTexture(const uint8_t *serializedModelDataPtr);
SplatMultiTextureView splatMultiTexture(const uint8_t *serializedModelDataPtr);

bool hasPointCloud(const uint8_t *serializedModelDataPtr);
PointCloudGeometryView pointCloudView(const uint8_t *serializedModelDataPtr);

}  // namespace c8
