// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cstring>
#include <memory>

#include "c8/color.h"
#include "c8/geometry/box3.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"
#include "c8/vector.h"

namespace c8 {

enum class GpuDataType { FLOAT, INT, UINT, BYTE };

struct AttributeLayout {
  String name;
  int numChannels;
  GpuDataType type;
  bool normalized;
  int stride;
  int offset;
};

struct InterleavedBufferData {
  Vector<uint8_t> data;
  Vector<AttributeLayout> attributes;
};

// Abstract base class for rendering this geometry. The implementation of this is specific to a
// gpu framework (e.g. vulkan vs. metal vs. opengl) and this will be set by the renderer.
class GeometryRenderer {
public:
  virtual void render() = 0;
  virtual ~GeometryRenderer() = default;

private:
  virtual unsigned long getGpuDataType(GpuDataType type) = 0;
};

class Geometry {
public:
  // Vertices of this geometry.
  const Vector<HPoint3> &vertices() const { return vertices_; }

  // Other optional properties describing a vertex. If set, there should be a 1:1 correspondence
  // with vertices.
  const Vector<Color> &colors() const { return colors_; }
  const Vector<HVector3> &normals() const { return normals_; }
  const Vector<HVector2> &uvs() const { return uvs_; }

  // Exactly one of triangles or points should be set. These are indices into the vertex array (and
  // colors, normals, etc.).
  const Vector<MeshIndices> &triangles() const { return triangles_; }
  const Vector<PointIndices> &points() const { return points_; }

  // Instance properties. If these are set, instanced rendering will be used, with one instance per
  // element in the array. All instanced fields should be the same length.
  const Vector<HPoint3> &instancePositions() const { return instancePositions_; }
  const Vector<Quaternion> &instanceRotations() const { return instanceRotations_; }
  const Vector<HVector3> &instanceScales() const { return instanceScales_; }
  const Vector<Color> &instanceColors() const { return instanceColors_; }
  const Vector<uint32_t> &instanceIds() const { return instanceIds_; }

  // Interleaved vertex data. If set, this will be used for rendering instead of the individual
  // vertex fields. If instance data is set, this will be used for instanced rendering.
  const InterleavedBufferData &interleavedVertexData() const { return interleavedVertexData_; }
  const InterleavedBufferData &interleavedInstanceData() const { return interleavedInstanceData_; }

  // Setters for various fields. These will mark the corresponding field as dirty until setClean is
  // called.
  Geometry &setVertices(const Vector<HPoint3> &vertices);
  Geometry &setColor(Color color);  // Convience for setting all vertices to the same color.
  Geometry &setColors(const Vector<Color> &colors);
  Geometry &setNormals(const Vector<HVector3> &normals);
  Geometry &setUvs(const Vector<HVector2> &uvs);
  Geometry &setTriangles(const Vector<MeshIndices> &triangles);
  Geometry &setPoints(const Vector<PointIndices> &points);

  Geometry &setVertices(const HPoint3 *vertices, size_t count);
  Geometry &setColors(const Color *colors, size_t count);
  Geometry &setNormals(const HVector3 *normals, size_t count);
  Geometry &setUvs(const HVector2 *uvs, size_t count);
  Geometry &setTriangles(const MeshIndices *triangles, size_t count);
  Geometry &setPoints(const PointIndices *points, size_t count);

  // Setters for instanced rendering fields. If these are set, instanced rendering will be used,
  // with one instance per element in the array. All instanced fields should be the same length if
  // set. These will mark the corresponding field as dirty until setClean is called.
  Geometry &setInstancePositions(const Vector<HPoint3> &positions);
  Geometry &setInstanceRotations(const Vector<Quaternion> &rotations);
  Geometry &setInstanceScales(const Vector<HVector3> &scales);
  Geometry &setInstanceColors(const Vector<Color> &colors);
  Geometry &setInstanceIds(const Vector<uint32_t> &ids);

  Geometry &setInstancePositions(const HPoint3 *positions, size_t count);
  Geometry &setInstanceRotations(const Quaternion *rotations, size_t count);
  Geometry &setInstanceScales(const HVector3 *scales, size_t count);
  Geometry &setInstanceColors(const Color *colors, size_t count);
  Geometry &setInstanceIds(const uint32_t *ids, size_t count);

  // Setters for interleaved vertex data. This is a more efficient way to store vertex data, as it
  // allows for better cache coherency. This will mark the interleaved data as dirty until
  // setClean is called.
  Geometry &setInterleavedVertexData(const InterleavedBufferData &data);
  Geometry &setInterleavedVertexData(const InterleavedBufferData &&data);

  Geometry &setInterleavedInstanceData(const InterleavedBufferData &data);
  Geometry &setInterleavedInstanceData(const InterleavedBufferData &&data);

  // Set the number of instances to render. This can be used to override the number of instances
  // that would be rendered based on the instance data, or if no additional per-instance data is
  // needed.
  Geometry &setInstanceCount(int count);
  int instanceCount() const { return instanceCount_; }

  // Mark all fields as clean.
  Geometry &setClean();

  // Check which fields are dirty.
  bool verticesDirty() const { return verticesDirty_; }
  bool colorsDirty() const { return colorsDirty_; }
  bool normalsDirty() const { return normalsDirty_; }
  bool uvsDirty() const { return uvsDirty_; }
  bool trianglesDirty() const { return trianglesDirty_; }
  bool pointsDirty() const { return pointsDirty_; }
  bool instancePositionsDirty() const { return instancePositionsDirty_; }
  bool instanceRotationsDirty() const { return instanceRotationsDirty_; }
  bool instanceScalesDirty() const { return instanceScalesDirty_; }
  bool instanceColorsDirty() const { return instanceColorsDirty_; }
  bool instanceIdsDirty() const { return instanceIdsDirty_; }
  bool interleavedVertexDataDirty() const { return interleavedVertexDataDirty_; }
  bool interleavedInstanceDataDirty() const { return interleavedInstanceDataDirty_; }

  // Get the renderer for this geometry. This is set by the Renderer, and its implementation is
  // specific to the renderer.
  GeometryRenderer *renderer() { return renderer_.get(); }
  Geometry &setRenderer(std::unique_ptr<GeometryRenderer> &&renderer);

  // Returns a box that contains all the points of this geometry.  Note that this bounding box is
  // aligned with the geometry's local axis and is returned in local space.  To transform this box
  // into world space, use:
  //   auto worldBox = object8.geometry().boundingBox().transform(object8.world());
  Box3 boundingBox() const { return boundingBox_; };

  String toString(const String &indent) const;
  String toString() const { return toString(""); }

private:
  Vector<HPoint3> vertices_;  // Required

  Vector<Color> colors_;      // Optional.
  Vector<HVector3> normals_;  // Optional.
  Vector<HVector2> uvs_;      // Optional.

  // Exactly one of the following should be set:
  Vector<MeshIndices> triangles_;  // For mesh.
  Vector<PointIndices> points_;    // For points.

  // Instance properties.
  Vector<HPoint3> instancePositions_;
  Vector<Quaternion> instanceRotations_;
  Vector<HVector3> instanceScales_;
  Vector<Color> instanceColors_;
  Vector<uint32_t> instanceIds_;

  InterleavedBufferData interleavedVertexData_;
  InterleavedBufferData interleavedInstanceData_;

  int instanceCount_ = 0;

  bool verticesDirty_ = false;
  bool colorsDirty_ = false;
  bool normalsDirty_ = false;
  bool uvsDirty_ = false;
  bool trianglesDirty_ = false;
  bool pointsDirty_ = false;
  bool instancePositionsDirty_ = false;
  bool instanceRotationsDirty_ = false;
  bool instanceScalesDirty_ = false;
  bool instanceColorsDirty_ = false;
  bool instanceIdsDirty_ = false;
  bool interleavedVertexDataDirty_ = false;
  bool interleavedInstanceDataDirty_ = false;

  Box3 boundingBox_;
  std::unique_ptr<GeometryRenderer> renderer_;
};

// Factory methods for creating Object8 instances.
namespace GeoGen {

// Basic factory methods for creating a geometry with default data.
// TODO(nb): should these be shared_ptr?
std::unique_ptr<Geometry> empty();

// Get a basic quad at z=0, with x,y corners at -1:1.
std::unique_ptr<Geometry> quad(bool interleaved);
std::unique_ptr<Geometry> quadPoints();

// Get a stack of quads at x,y corners at -1:1 and z = 0.
// This is useful for batch processing of instances, where each quad is a separate sub-instance and
// (gl_VertexId / 4) is used to identify the sub-instances.
std::unique_ptr<Geometry> stackedQuad(int count);

// Create a 1.0f width cube with only vertices.
// This geometry requires smaller memory and is faster to draw.
// Typical usage will be the skybox cube.
std::unique_ptr<Geometry> cubeVertexOnlyMesh();

// Create a basic cube
std::unique_ptr<Geometry> cubeMesh();

struct CurvyMeshParams {
  float topRadius = 1.0f;     // Radius of the top, compared to height.
  float bottomRadius = 1.0f;  // Radius of the bottom, compared to height.
  float arc = 1.0f;           // Percent of the cylinder that is filled.
  bool open = false;          // Whether to fill the top and bottom.
  int segments = 8;
};

// Create a 1-unit tall curvy mesh (cylinder, cone, truncated cone) with the y-axis through the
// origin and with the specified parameters.
std::unique_ptr<Geometry> curvyMesh(const CurvyMeshParams &params);

// Get a basic quad at z=1, with x,y corners at -1:1. This is the back plane of clip-space, and
// when used in conjunction with Object8::setIgnoreProjection(true), this can be used to draw
// directly to the back of the whole viewport.
std::unique_ptr<Geometry> backQuad();

// Get a basic quad at z=-1, with x,y corners at -1:1. This is the front plane of clip-space, and
// when used in conjunction with Object8::setIgnoreProjection(true), this can be used to draw
// directly to the front of the whole viewport.
std::unique_ptr<Geometry> frontQuad();

// This can be used to draw to a portion of the viewport.  The parameters are all in pixel space.
std::unique_ptr<Geometry> pixelQuad(float x, float y, float width, float height);

// Creates a sphere with the specified radius centered at origin and the given number of
// horizontal (widthSegments) and vertical (heightSegments) segments along the perimeter
std::unique_ptr<Geometry> sphere(float radius, int widthSegments, int heightSegments);

// Creates a sphere with the specified radius centered at origin and has `segments` horizontal
// and `segments / 2` vertical segments
std::unique_ptr<Geometry> sphere(float radius, int segments);

void flipTextureY(Geometry *geom);

// Rotate the quad geometry counter clockwise by rotating the list of UVs
void rotateCCW(Geometry *geom);

// Convert a vector of data to a vector of bytes.
// This is useful for interleaved vertex data.
template <typename T>
Vector<uint8_t> dataToBytes(const Vector<T> &data) {
  size_t dataBytes = data.size() * sizeof(T);
  Vector<uint8_t> bytes(dataBytes);
  std::memcpy(bytes.data(), data.data(), dataBytes);

  return bytes;
}

}  // namespace GeoGen

}  // namespace c8
