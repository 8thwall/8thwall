// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"geometry.h"};
  deps = {
    "//c8:color",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:map",
    "//c8:quaternion",
    "//c8/string:containers",
    "//c8/string:format",
    "//c8/string:join",
    "//c8:vector",
    "//c8/geometry:box3",
    "//c8/geometry:mesh-types",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x455ba5c7);

#include <algorithm>

#include "c8/map.h"
#include "c8/pixels/render/geometry.h"
#include "c8/string/containers.h"
#include "c8/string/format.h"
#include "c8/string/join.h"

namespace c8 {

Geometry &Geometry::setVertices(const Vector<HPoint3> &vertices) {
  return setVertices(vertices.data(), vertices.size());
}

Geometry &Geometry::setVertices(const HPoint3 *vertices, size_t count) {
  vertices_.resize(count);
  std::copy(vertices, vertices + count, vertices_.begin());
  verticesDirty_ = true;
  // Create a bounding box that is aligned with the local axis.
  boundingBox_ = Box3::from(vertices_);
  return *this;
}

Geometry &Geometry::setColor(Color color) {
  Vector<Color> colors;
  colors.resize(vertices_.size());
  std::fill(colors.begin(), colors.end(), color);
  return setColors(colors);
};

Geometry &Geometry::setColors(const Vector<Color> &colors) {
  return setColors(colors.data(), colors.size());
}

Geometry &Geometry::setColors(const Color *colors, size_t count) {
  colors_.resize(count);
  std::copy(colors, colors + count, colors_.begin());
  colorsDirty_ = true;
  return *this;
}

Geometry &Geometry::setNormals(const Vector<HVector3> &normals) {
  return setNormals(normals.data(), normals.size());
}

Geometry &Geometry::setNormals(const HVector3 *normals, size_t count) {
  normals_.resize(count);
  std::copy(normals, normals + count, normals_.begin());
  normalsDirty_ = true;
  return *this;
}

Geometry &Geometry::setUvs(const Vector<HVector2> &uvs) { return setUvs(uvs.data(), uvs.size()); }

Geometry &Geometry::setUvs(const HVector2 *uvs, size_t count) {
  uvs_.resize(count);
  std::copy(uvs, uvs + count, uvs_.begin());
  uvsDirty_ = true;
  return *this;
}

Geometry &Geometry::setTriangles(const Vector<MeshIndices> &triangles) {
  return setTriangles(triangles.data(), triangles.size());
}

Geometry &Geometry::setTriangles(const MeshIndices *triangles, size_t count) {
  triangles_.resize(count);
  std::copy(triangles, triangles + count, triangles_.begin());
  trianglesDirty_ = true;
  return *this;
}

Geometry &Geometry::setPoints(const Vector<PointIndices> &points) {
  return setPoints(points.data(), points.size());
}

Geometry &Geometry::setPoints(const PointIndices *points, size_t count) {
  points_.resize(count);
  std::copy(points, points + count, points_.begin());
  pointsDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstancePositions(const Vector<HPoint3> &positions) {
  return setInstancePositions(positions.data(), positions.size());
}

Geometry &Geometry::setInstancePositions(const HPoint3 *positions, size_t count) {
  instancePositions_.resize(count);
  std::copy(positions, positions + count, instancePositions_.begin());
  instancePositionsDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstanceRotations(const Vector<Quaternion> &rotations) {
  return setInstanceRotations(rotations.data(), rotations.size());
}

Geometry &Geometry::setInstanceRotations(const Quaternion *rotations, size_t count) {
  instanceRotations_.resize(count);
  std::copy(rotations, rotations + count, instanceRotations_.begin());
  instanceRotationsDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstanceScales(const Vector<HVector3> &scales) {
  return setInstanceScales(scales.data(), scales.size());
}

Geometry &Geometry::setInstanceScales(const HVector3 *scales, size_t count) {
  instanceScales_.resize(count);
  std::copy(scales, scales + count, instanceScales_.begin());
  instanceScalesDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstanceColors(const Vector<Color> &colors) {
  return setInstanceColors(colors.data(), colors.size());
}

Geometry &Geometry::setInstanceColors(const Color *colors, size_t count) {
  instanceColors_.resize(count);
  std::copy(colors, colors + count, instanceColors_.begin());
  instanceColorsDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstanceIds(const Vector<uint32_t> &ids) {
  return setInstanceIds(ids.data(), ids.size());
}

Geometry &Geometry::setInstanceIds(const uint32_t *ids, size_t count) {
  instanceIds_.resize(count);
  std::copy(ids, ids + count, instanceIds_.begin());
  instanceIdsDirty_ = true;
  return *this;
}

Geometry &Geometry::setInstanceCount(int count) {
  instanceCount_ = count;
  return *this;
}

Geometry &Geometry::setInterleavedVertexData(const InterleavedBufferData &data) {
  interleavedVertexData_ = data;  // Deep copy the data
  interleavedVertexDataDirty_ = true;
  return *this;
}

Geometry &Geometry::setInterleavedVertexData(const InterleavedBufferData &&data) {
  // Move copy the data
  interleavedVertexData_ = std::move(data);
  interleavedVertexDataDirty_ = true;
  return *this;
}

Geometry &Geometry::setInterleavedInstanceData(const InterleavedBufferData &data) {
  // Deep copy the data
  interleavedInstanceData_ = data;
  interleavedInstanceDataDirty_ = true;
  return *this;
}

Geometry &Geometry::setInterleavedInstanceData(const InterleavedBufferData &&data) {
  // Move copy the data
  interleavedInstanceData_ = std::move(data);
  interleavedInstanceDataDirty_ = true;
  return *this;
}

Geometry &Geometry::setClean() {
  verticesDirty_ = false;
  colorsDirty_ = false;
  normalsDirty_ = false;
  uvsDirty_ = false;
  pointsDirty_ = false;
  trianglesDirty_ = false;
  instancePositionsDirty_ = false;
  instanceRotationsDirty_ = false;
  instanceScalesDirty_ = false;
  instanceColorsDirty_ = false;
  instanceIdsDirty_ = false;
  return *this;
}

Geometry &Geometry::setRenderer(std::unique_ptr<GeometryRenderer> &&renderer) {
  renderer_ = std::move(renderer);
  return *this;
}

String Geometry::toString(const String &indent) const {
  TreeMap<int, Vector<String>> counts;
  counts[vertices_.size()].push_back("vertices");
  counts[colors_.size()].push_back("colors");
  counts[normals_.size()].push_back("normals");
  counts[uvs_.size()].push_back("uvs");
  counts[points_.size()].push_back("points");
  counts[triangles_.size()].push_back("triangles");

  Vector<std::pair<int, Vector<String>>> countEntries = {counts.begin(), counts.end()};

  auto countStrings = map<std::pair<int, Vector<String>>, String>(countEntries, [](const auto &e) {
    return format("%d: [%s]", e.first, strJoin(e.second, ", ").c_str());
  });
  std::reverse(countStrings.begin(), countStrings.end());

  Vector<String> dirty;
  if (verticesDirty_) {
    dirty.push_back("vertices");
  }
  if (colorsDirty_) {
    dirty.push_back("colors");
  }
  if (normalsDirty_) {
    dirty.push_back("normals");
  }
  if (uvsDirty_) {
    dirty.push_back("uvs");
  }
  if (pointsDirty_) {
    dirty.push_back("points");
  }
  if (trianglesDirty_) {
    dirty.push_back("triangles");
  }

  return strJoin<String>(
    {
      "counts:",
      format("  %s", strJoin(countStrings, format("\n%s  ", indent.c_str()).c_str()).c_str()),
      format("dirty: [%s]", strJoin(dirty, ", ").c_str()),
      "boundingBox:",
      format("  min: %s", boundingBox_.min.toString().c_str()),
      format("  max: %s", boundingBox_.max.toString().c_str()),
    },
    format("\n%s", indent.c_str()));
}

namespace GeoGen {

std::unique_ptr<Geometry> empty() { return std::make_unique<Geometry>(); }

std::unique_ptr<Geometry> quad(bool interleaved) {
  auto g = GeoGen::empty();
  const Vector<MeshIndices> triangles = {{0, 1, 2}, {2, 3, 0}};
  if (interleaved) {
    const int posDims = 4;
    const int normalDims = 4;
    const int uvDims = 3;
    const int stride = (posDims + normalDims + uvDims) * sizeof(float);
    g->setInterleavedVertexData({
      .data = dataToBytes<float>({
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      }),
      .attributes =
        {
          {"position", posDims, GpuDataType::FLOAT, false, stride, 0},
          {"normal", normalDims, GpuDataType::FLOAT, false, stride, posDims * sizeof(float)},
          {"uv", uvDims, GpuDataType::FLOAT, false, stride, (posDims + normalDims) * sizeof(float)},
        },
    });
    g->setTriangles(triangles);
  } else {
    g->setVertices(
       {{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}})
      .setTriangles(triangles)
      .setUvs({{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}});
  }
  return g;
}

std::unique_ptr<Geometry> stackedQuad(int count) {
  Vector<HPoint3> vertices;
  Vector<MeshIndices> triangles;
  vertices.reserve(4 * count);
  triangles.reserve(2 * count);
  uint32_t idx = 0;
  for (uint32_t i = 0; i < count; ++i, idx += 4) {
    vertices.push_back({-1.0f, -1.0f, 0.0f});
    vertices.push_back({-1.0f, 1.0f, 0.0f});
    vertices.push_back({1.0f, 1.0f, 0.0f});
    vertices.push_back({1.0f, -1.0f, 0.0f});
    triangles.push_back({idx + 0, idx + 1, idx + 2});
    triangles.push_back({idx + 2, idx + 3, idx + 0});
  }
  auto g = GeoGen::empty();
  g->setVertices(vertices);
  g->setTriangles(triangles);
  return g;
}

std::unique_ptr<Geometry> backQuad() {
  auto g = GeoGen::empty();
  g->setVertices(
     {{-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}})
    .setTriangles({{0, 1, 2}, {2, 3, 0}})
    .setUvs({{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}});
  return g;
}

std::unique_ptr<Geometry> frontQuad() {
  auto g = GeoGen::empty();
  g->setVertices(
     {{-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}})
    .setTriangles({{0, 1, 2}, {2, 3, 0}})
    .setUvs({{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}});
  return g;
}

// This can be used to draw to a portion of the viewport.  The parameters are all in pixel space.
// They are transformed into clip space by the pixelCamera().
std::unique_ptr<Geometry> pixelQuad(float x, float y, float width, float height) {
  auto g = GeoGen::empty();
  g->setVertices(
     {{x, y, 1.0f}, {x, y + height, 1.0f}, {x + width, y + height, 1.0f}, {x + width, y, 1.0f}})
    .setTriangles({{0, 1, 2}, {2, 3, 0}})
    .setUvs({{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}});
  return g;
}

std::unique_ptr<Geometry> quadPoints() {
  auto g = GeoGen::empty();
  g->setVertices(
     {{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}})
    .setPoints({{0}, {1}, {2}, {3}});
  return g;
}

std::unique_ptr<Geometry> cubeVertexOnlyMesh() {
  auto g = GeoGen::empty();
  g->setVertices({
    // clang-format off
    {-0.5f, -0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    // clang-format on
  });
  g->setTriangles({
    // positive x
    {7, 2, 6},
    {7, 3, 2},
    // negative x
    {4, 5, 1},
    {4, 1, 0},
    // positive y
    {5, 6, 1},
    {6, 2, 1},
    // negative y
    {4, 0, 7},
    {7, 0, 3},
    // positive z
    {4, 6, 5},
    {4, 7, 6},
    // negative z
    {0, 1, 2},
    {0, 2, 3},
  });
  return g;
}

std::unique_ptr<Geometry> cubeMesh() {
  auto g = GeoGen::empty();

  // Though a cube could be done with only 8 vertices, in order to do flat shading you cannot share
  // vertices between the corners of the quads because they need normals facing in different
  // directions.
  g->setVertices({
    // front
    {-0.5f, -0.5f, -0.5f},  // bl
    {-0.5f, 0.5f, -0.5f},   // tl
    {0.5f, 0.5f, -0.5f},    // tr
    {0.5f, -0.5f, -0.5f},   // br
    // back
    {0.5f, -0.5f, 0.5f},   // bl
    {0.5f, 0.5f, 0.5f},    // tl
    {-0.5f, 0.5f, 0.5f},   // tr
    {-0.5f, -0.5f, 0.5f},  // br
    // left
    {-0.5f, -0.5f, 0.5f},   // bl
    {-0.5f, 0.5f, 0.5f},    // tl
    {-0.5f, 0.5f, -0.5f},   // tr
    {-0.5f, -0.5f, -0.5f},  // br
    // right
    {0.5f, -0.5f, -0.5f},  // bl
    {0.5f, 0.5f, -0.5f},   // tl
    {0.5f, 0.5f, 0.5f},    // tr
    {0.5f, -0.5f, 0.5f},   // br
    // bottom
    {-0.5f, -0.5f, 0.5f},   // bl
    {-0.5f, -0.5f, -0.5f},  // tl
    {0.5f, -0.5f, -0.5f},   // tr
    {0.5f, -0.5f, 0.5f},    // br
    // top
    {-0.5f, 0.5f, -0.5f},  // bl
    {-0.5f, 0.5f, 0.5f},   // tl
    {0.5f, 0.5f, 0.5f},    // tr
    {0.5f, 0.5f, -0.5f},   // br
  });
  g->setTriangles({
    // front
    {0, 1, 2},
    {0, 2, 3},
    // back
    {4, 5, 6},
    {4, 6, 7},
    // left
    {8, 9, 10},
    {8, 10, 11},
    // right
    {12, 13, 14},
    {12, 14, 15},
    // bottom
    {16, 17, 18},
    {16, 18, 19},
    // top
    {20, 21, 22},
    {20, 22, 23},
  });
  g->setNormals({
    {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
  });
  g->setUvs({
    // front
    {0.25f, 0.33333f},  // bl
    {0.25f, 0.66666f},  // tl
    {0.5f, 0.66666f},   // tr
    {0.5f, 0.33333f},   // br
    // back
    {0.75f, 0.33333f},  // bl
    {0.75f, 0.66666f},  // tl
    {1.0f, 0.66666f},   // tr
    {1.0f, 0.33333f},   // br
    // left
    {0.0f, 0.33333f},   // bl
    {0.0f, 0.66666f},   // tl
    {0.25f, 0.66666f},  // tr
    {0.25f, 0.33333f},  // br
    // right
    {0.5f, 0.33333f},   // bl
    {0.5f, 0.66666f},   // tl
    {0.75f, 0.66666f},  // tr
    {0.75f, 0.33333f},  // br
    // bottom
    {0.25f, 0.0f},      // bl
    {0.25f, 0.33333f},  // tl
    {0.5f, 0.33333f},   // tr
    {0.5f, 0.0f},       // br
    // top
    {0.25f, 0.66666f},  // bl
    {0.25f, 1.0f},      // tl
    {0.5f, 1.0f},       // tr
    {0.5f, 0.66666f},   // br
  });

  return g;
}

std::unique_ptr<Geometry> curvyMesh(const CurvyMeshParams &params) {
  Vector<HPoint3> pts;
  Vector<HVector3> normals;
  Vector<HVector2> uvs;
  Vector<MeshIndices> inds;

  float segmentWidth = 1.0f / params.segments;
  float start = 0.5f * (1.0f - params.arc);

  auto bottomUv = HVector2(0.0f, 0.0f);
  auto topUv = HVector2(1.0f, 1.0f);
  auto bottomNormal = HVector3(0.0f, -1.0f, 0.0f);
  auto topNormal = HVector3(0.0f, 1.0f, 0.0f);

  // If the top and bottom are added, add the central point at the top and bottom.
  if (!params.open) {
    pts.push_back({0.0f, -0.5f, 0.0f});
    pts.push_back({0.0f, 0.5f, 0.0f});
    uvs.push_back(bottomUv);
    uvs.push_back(topUv);
    normals.push_back(bottomNormal);
    normals.push_back(topNormal);
  }

  int ptsPerSegment = params.open ? 2 : 4;
  for (int i = 0; i <= params.segments; ++i) {
    // Add vertices, normals, uvs and inds for the curvy face quad.
    auto bottomInd = static_cast<uint32_t>(uvs.size());
    auto topInd = bottomInd + 1;
    auto nextBottomInd = bottomInd + ptsPerSegment;
    auto nextTopInd = bottomInd + ptsPerSegment + 1;

    float u = i * segmentWidth;

    uvs.push_back({u, 0.0f});  // bottom
    uvs.push_back({u, 1.0f});  // top

    float theta = 2 * M_PI * (start + params.arc * u);
    float px = -std::sin(theta);
    float pz = std::cos(theta);
    pts.push_back({params.bottomRadius * px, -0.5f, params.bottomRadius * pz});
    pts.push_back({params.topRadius * px, 0.5f, params.topRadius * pz});
    auto normal = HVector3(px, params.bottomRadius - params.topRadius, pz).unit();
    normals.push_back(normal);
    normals.push_back(normal);

    //  Add inds for this segment. The last vertices will not have a quad.
    if (i < params.segments) {
      inds.push_back({bottomInd, topInd, nextBottomInd});
      inds.push_back({topInd, nextTopInd, nextBottomInd});
    }

    // Add vertices, normals, uvs and inds for the top and bottom.
    if (!params.open) {
      bottomInd += 2;
      topInd += 2;
      nextBottomInd += 2;
      nextTopInd += 2;
      pts.push_back({params.bottomRadius * px, -0.5f, params.bottomRadius * pz});
      pts.push_back({params.topRadius * px, 0.5f, params.topRadius * pz});
      uvs.push_back(bottomUv);
      uvs.push_back(topUv);
      normals.push_back(bottomNormal);
      normals.push_back(topNormal);

      //  Add inds for this segment. The last vertices will not have a segment.
      if (i < params.segments) {
        inds.push_back({0, bottomInd, nextBottomInd});
        inds.push_back({1, nextTopInd, topInd});
      }
    }
  }

  auto g = GeoGen::empty();
  g->setVertices(pts).setUvs(uvs).setNormals(normals).setTriangles(inds);
  return g;
}

std::unique_ptr<Geometry> sphere(float radius, int widthSegments, int heightSegments) {
  Vector<HPoint3> pts;
  Vector<HVector3> normals;
  Vector<HVector2> uvs;
  Vector<MeshIndices> inds;

  for (int lat = 0; lat <= heightSegments; lat++) {
    float theta = lat * M_PI / heightSegments;
    float sinT = std::sin(theta);
    float cosT = std::cos(theta);
    for (int lon = 0; lon <= widthSegments; lon++) {
      float phi = lon * M_PI * 2.f / widthSegments;
      float sinP = std::sin(phi);
      float cosP = std::cos(phi);

      pts.push_back(HPoint3{radius * cosP * sinT, radius * cosT, radius * sinP * sinT});
      normals.push_back({cosP * sinT, cosT, sinP * sinT});
      uvs.push_back(
        {static_cast<float>(lon) / widthSegments,
         static_cast<float>(lat) / heightSegments});
    }
  }

  for (int lat = 0; lat < heightSegments; lat++) {
    uint32_t first = (lat * (widthSegments + 1));
    uint32_t second = first + widthSegments + 1;
    for (int lon = 0; lon < widthSegments; lon++, first++, second++) {
      // Poles of sphere are connected to vertices as triangles, other rows are connected as quads
      if (lat != 0) {
        inds.push_back({first + 1, second, first});
      }
      if (lat != heightSegments - 1) {
        inds.push_back({second + 1, second, first + 1});
      }
    }
  }

  auto g = GeoGen::empty();
  g->setVertices(pts).setUvs(uvs).setNormals(normals).setTriangles(inds);
  return g;
}

std::unique_ptr<Geometry> sphere(float radius, int segments) {
  return sphere(radius, segments, segments / 2);
}

void flipTextureY(Geometry *geom) {
  auto uvs = geom->uvs();
  for (auto &uv : uvs) {
    uv = {uv.x(), 1.0f - uv.y()};
  }
  geom->setUvs(uvs);
}

void rotateCCW(Geometry *geom) {
  if (geom->uvs().size() <= 1) {
    // geometry is symmetric
    return;
  }

  Vector<HVector2> uvs;
  uvs.reserve(geom->uvs().size());
  for (int i = 1; i < geom->uvs().size(); i++) {
    uvs.push_back(geom->uvs()[i]);
  }
  uvs.push_back(geom->uvs()[0]);
  geom->setUvs(uvs);
}

}  // namespace GeoGen

}  // namespace c8
