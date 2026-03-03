// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "load-mesh.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:hpoint",
    "//c8/geometry:device-pose",
    "//c8/geometry:mesh",
    "//c8/geometry:mesh-types",
    "//c8/io:image-io",
    "//c8/pixels:base64",
    "//c8/stats:scope-timer",
    "@draco//:draco",
    "@tinygltf//:tinygltf",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x38b2549f);

#include <draco/compression/decode.h>
#include <draco/core/decoder_buffer.h>

#include <cstdint>
#include <fstream>
#include <optional>

#include "c8/c8-log.h"
#include "c8/color.h"
#include "c8/geometry/device-pose.h"
#include "c8/geometry/load-mesh.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/mesh.h"
#include "c8/hpoint.h"
#include "c8/io/file-io.h"
#include "c8/pixels/base64.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

// For reading in the triangle indices from the glb file.
constexpr const int GLB_COMPONENT_UNSIGNED_SHORT = 5123;
constexpr const int GLB_COMPONENT_UNSIGNED_INT = 5125;

template <class ComponentType>
void readGlbIndices(
  const tinygltf::Buffer &indicesBuffer,
  Vector<MeshIndices> &triangles,
  int indicesOffset,
  int triangleCount) {
  triangles.reserve(triangleCount);
  const ComponentType *gltfIndices =
    reinterpret_cast<const ComponentType *>(&indicesBuffer.data[indicesOffset]);

  for (size_t i = 0; i < triangleCount; i++) {
    triangles.push_back(
      {gltfIndices[2],
       gltfIndices[1],
       gltfIndices[0]});  // Switch winding order to match handedness.
    gltfIndices += 3;
  }
}

std::optional<MeshGeometry> dracoDataToMesh(Vector<uint8_t> *dracoBinaryData, bool narToXr) {
  ScopeTimer t("draco-data-to-mesh");
  draco::DecoderBuffer buffer;
  buffer.Init(reinterpret_cast<char *>(dracoBinaryData->data()), dracoBinaryData->size());

  draco::Decoder decoder;
  auto statusor = decoder.DecodeMeshFromBuffer(&buffer);
  if (!statusor.ok()) {
    C8Log("[load-mesh] %s", statusor.status().error_msg());
    return std::nullopt;
  }

  std::unique_ptr<draco::Mesh> dracoMesh = std::move(statusor).value();

  // Convert the draco::Mesh into a MeshGeometry
  MeshGeometry mesh;
  mesh.points.reserve(dracoMesh->num_points());

  // Copy over the points and colors
  const draco::PointAttribute *positionAttr =
    dracoMesh->GetNamedAttribute(draco::GeometryAttribute::POSITION, 0);
  float pts[3];
  for (int i = 0; i < dracoMesh->num_points(); ++i) {
    // Using GetMappedValue gives us the ordered point positions.  Using GetValue would return the
    // unordered point data.
    positionAttr->GetMappedValue(draco::PointIndex(i), &pts[0]);
    if (narToXr) {
      mesh.points.push_back(xrFromNar(HPoint3{pts[0], pts[1], pts[2]}));
    } else {
      mesh.points.push_back({pts[0], pts[1], pts[2]});
    }
  }

  // Copy over the colors.
  const draco::PointAttribute *colorAttr =
    dracoMesh->GetNamedAttribute(draco::GeometryAttribute::COLOR, 0);
  if (colorAttr) {
    // Vertex based colors instead of using a fragment shader.
    mesh.colors.reserve(dracoMesh->num_points());
    uint8_t colors[3];
    for (int i = 0; i < dracoMesh->num_points(); ++i) {
      colorAttr->GetMappedValue(draco::PointIndex(i), &colors[0]);
      mesh.colors.push_back({colors[0], colors[1], colors[2]});
    }
  } else {
    mesh.colors.reserve(dracoMesh->num_points());
    for (int i = 0; i < dracoMesh->num_points(); ++i) {
      mesh.colors.push_back({128, 128, 128});
    }
  }

  // Copy over UV coordinates
  const draco::PointAttribute *texCoordAttr =
    dracoMesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD, 0);
  if (texCoordAttr) {
    // Vertex based colors instead of using a fragment shader.
    mesh.uvs.reserve(dracoMesh->num_points());
    float uv[2];
    for (int i = 0; i < dracoMesh->num_points(); ++i) {
      texCoordAttr->GetMappedValue(draco::PointIndex(i), &uv[0]);
      mesh.uvs.push_back({uv[0], uv[1]});
    }
  }

  // Copy over the indices
  mesh.triangles.reserve(dracoMesh->num_faces());
  for (draco::FaceIndex fi(0); fi < dracoMesh->num_faces(); ++fi) {
    const auto &face = dracoMesh->face(fi);
    if (narToXr) {
      mesh.triangles.push_back({face[2].value(), face[1].value(), face[0].value()});
    } else {
      mesh.triangles.push_back({face[0].value(), face[1].value(), face[2].value()});
    }
  }

  // Compute vertex normals.
  computeVertexNormals(mesh.points, mesh.triangles, &mesh.normals);

  return mesh;
}

std::optional<MeshGeometry> dracoFileToMesh(
  const String &assetPath, bool isBase64Encoded, bool narToXr) {
  ScopeTimer t("draco-file-to-mesh");
  Vector<uint8_t> data;
  if (isBase64Encoded) {
    data = decode(readTextFile(assetPath));
  } else {
    data = readFile(assetPath);
  }

  return dracoDataToMesh(&data, narToXr);
}

// Original scaniverse function used as reference for this method:
// gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/niantic-ar/scaniverse/scaniverse-tools/-/blob/master/scans/loaders.py
std::optional<MeshGeometry> marDataToMesh(const Vector<uint8_t> &marData) {
  return marDataToMesh(marData.data(), marData.size(), true);
}

std::optional<MeshGeometry> marDataToMesh(const uint8_t *marData, size_t size, bool narToXr) {
  // Check whether there is enough data for a header.
  if (size < 16) {
    return std::nullopt;
  }

  // Read header
  int32_t magic = *reinterpret_cast<const int32_t *>(&marData[0]);
  int32_t version = *reinterpret_cast<const int32_t *>(&marData[4]);
  int32_t num_entries = *reinterpret_cast<const int32_t *>(&marData[12]);

  if (magic != 0x5252414d || version != 1) {
    // Unexpected header in multiarray file
    return std::nullopt;
  }

  // Read table of contents
  Vector<std::pair<int32_t, int32_t>> toc;
  size_t offset = 16;  // Skip the header

  // Check whether there is enough data for TOC
  if (size < (offset + 8 * num_entries)) {
    return std::nullopt;
  }

  for (int i = 0; i < num_entries; ++i) {
    int32_t typecode = *reinterpret_cast<const int32_t *>(&marData[offset]);
    int32_t length = *reinterpret_cast<const int32_t *>(&marData[offset + 4]);
    toc.emplace_back(typecode, length);
    offset += 8;
  }

  // If we're converting RUB to RUF, negate z values.
  float nz = narToXr ? -1.0f : 1.0f;

  MeshGeometry meshGeo;
  for (int i = 0; i < toc.size(); ++i) {
    const auto &[typecode, length] = toc[i];
    int dtype_size;
    switch (typecode) {
      case 0x12:
        dtype_size = 2;
        break;  // float16
      case 0x14:
        dtype_size = 4;
        break;  // float32
      case 0x21:
        dtype_size = 1;
        break;  // uint8
      case 0x22:
        dtype_size = 2;
        break;  // uint16
      case 0x34:
        dtype_size = 4;
        break;  // int32
      default:
        break;
    }

    // Check whether there is enough data left in the input.
    if (size < (offset + length)) {
      return std::nullopt;
    }

    switch (i) {
      case 0: {
        int numVertices = length / dtype_size / 3;
        meshGeo.points.reserve(numVertices);
        for (int j = 0; j < numVertices; ++j) {
          auto x = *reinterpret_cast<const float *>(&marData[offset]);
          auto y = *reinterpret_cast<const float *>(&marData[offset + dtype_size]);
          auto z = *reinterpret_cast<const float *>(&marData[offset + (dtype_size * 2)]);
          // Convert to left-handed coordinate system.
          meshGeo.points.push_back({x, y, nz * z});
          offset += dtype_size * 3;
        }
        break;
      }
      case 1: {
        int numTriangles = (length / dtype_size) / 3;
        meshGeo.triangles.reserve(numTriangles);
        for (int j = 0; j < numTriangles; ++j) {
          auto a = *reinterpret_cast<const uint32_t *>(&marData[offset]);
          auto b = *reinterpret_cast<const uint32_t *>(&marData[offset + dtype_size]);
          auto c = *reinterpret_cast<const uint32_t *>(&marData[offset + (dtype_size * 2)]);
          // Switch the winding order
          if (narToXr) {
            meshGeo.triangles.push_back({a, c, b});
          } else {
            meshGeo.triangles.push_back({a, b, c});
          }
          offset += dtype_size * 3;
        }
        break;
      }
      case 2: {
        int numNormals = (length / dtype_size) / 3;
        meshGeo.normals.reserve(numNormals);
        for (int j = 0; j < numNormals; ++j) {
          auto x = *reinterpret_cast<const float *>(&marData[offset]);
          auto y = *reinterpret_cast<const float *>(&marData[offset + dtype_size]);
          auto z = *reinterpret_cast<const float *>(&marData[offset + (dtype_size * 2)]);
          meshGeo.normals.push_back({x, y, nz * z});
          offset += dtype_size * 3;
        }
        break;
      }
      case 3: {
        int numColors = (length / dtype_size) / 3;
        meshGeo.colors.reserve(numColors);
        for (int j = 0; j < numColors; ++j) {
          auto r = *reinterpret_cast<const uint8_t *>(&marData[offset]);
          auto g = *reinterpret_cast<const uint8_t *>(&marData[offset + dtype_size]);
          auto b = *reinterpret_cast<const uint8_t *>(&marData[offset + (dtype_size * 2)]);
          meshGeo.colors.push_back({r, g, b});
          offset += dtype_size * 3;
        }
        break;
      }
      case 4: {
        int numUvs = (length / dtype_size) / 2;
        meshGeo.uvs.reserve(numUvs);
        for (int j = 0; j < numUvs; ++j) {
          auto u = *reinterpret_cast<const float *>(&marData[offset]);
          auto v = *reinterpret_cast<const float *>(&marData[offset + dtype_size]);
          meshGeo.uvs.push_back({u, v});
          offset += dtype_size * 2;
        }
        break;
      }
      default:
        break;
    }
  }

  if (meshGeo.normals.empty()) {
    computeVertexNormals(meshGeo.points, meshGeo.triangles, &meshGeo.normals);
  }

  return meshGeo;
}

std::optional<MeshGeometry> marFileToMesh(const String &assetPath) {
  // For demonstration, let's assume `mesh.mar` contains the data in binary format.
  std::ifstream file(assetPath, std::ios::binary);
  if (!file) {
    return std::nullopt;
  }

  // Read the file into a byte vector
  Vector<uint8_t> marData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  return marDataToMesh(marData);
}

MeshGeometry tinyGLTFModelToMesh(const tinygltf::Model &model) {
  MeshGeometry meshGeo;

  auto gltfMesh = model.meshes[0];
  auto gltfPrimitive = gltfMesh.primitives[0];

  // Copying gltf vertices.
  auto positionAccessor = model.accessors[gltfPrimitive.attributes["POSITION"]];
  auto positionBufferView = model.bufferViews[positionAccessor.bufferView];
  const tinygltf::Buffer &positionBuffer = model.buffers[positionBufferView.buffer];
  const float *gltfPositions = reinterpret_cast<const float *>(
    &positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

  meshGeo.points.reserve(positionAccessor.count);
  for (size_t i = 0; i < positionAccessor.count; i++) {
    meshGeo.points.push_back(
      {gltfPositions[0], gltfPositions[1], -gltfPositions[2]});  // Make left handed
    gltfPositions += 3;
  }

  // Copying gltf colors.
  auto colorAccessor = model.accessors[gltfPrimitive.attributes["COLOR"]];

  meshGeo.colors.reserve(positionAccessor.count);
  if (colorAccessor.count > 0) {
    auto colorBufferView = model.bufferViews[colorAccessor.bufferView];
    const tinygltf::Buffer &colorBuffer = model.buffers[colorBufferView.buffer];
    const uint8_t *gltfColors = reinterpret_cast<const uint8_t *>(
      &colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);

    for (size_t i = 0; i < colorAccessor.count; i++) {
      meshGeo.colors.push_back({gltfColors[0], gltfColors[1], gltfColors[2]});
      gltfColors += 3;
    }
  } else {
    for (size_t i = 0; i < colorAccessor.count; i++) {
      meshGeo.colors.push_back({128, 128, 128});
    }
  }

  // Copying over gltf UV coordinates.
  auto texAccessor = model.accessors[gltfPrimitive.attributes["TEXCOORD_0"]];

  if (texAccessor.count > 0) {
    auto texBufferView = model.bufferViews[texAccessor.bufferView];
    const tinygltf::Buffer &texBuffer = model.buffers[texBufferView.buffer];
    const float *gltfTex = reinterpret_cast<const float *>(
      &texBuffer.data[texBufferView.byteOffset + texAccessor.byteOffset]);

    meshGeo.uvs.reserve(positionAccessor.count);
    for (size_t i = 0; i < texAccessor.count; i++) {
      meshGeo.uvs.push_back({gltfTex[0], gltfTex[1]});
      gltfTex += 2;
    }
  }

  // Copying over gltf normals coordinates.
  auto normalsAccessor = model.accessors[gltfPrimitive.attributes["NORMAL"]];

  if (normalsAccessor.count > 0) {
    auto normalsBufferView = model.bufferViews[normalsAccessor.bufferView];
    const tinygltf::Buffer &normalsBuffer = model.buffers[normalsBufferView.buffer];
    const float *gltfNormals = reinterpret_cast<const float *>(
      &normalsBuffer.data[normalsBufferView.byteOffset + normalsAccessor.byteOffset]);

    meshGeo.normals.reserve(normalsAccessor.count);
    for (size_t i = 0; i < normalsAccessor.count; i++) {
      meshGeo.normals.push_back({gltfNormals[0], gltfNormals[1], -gltfNormals[2]});
      gltfNormals += 3;
    }
  }

  // Copy over triangle indices.
  auto indicesAccessor = model.accessors[gltfPrimitive.indices];
  auto indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
  const tinygltf::Buffer &indicesBuffer = model.buffers[indicesBufferView.buffer];

  // Indices component types can be found here:
  // https://github.com/KhronosGroup/glTF/blob/5c80263416ea0aab5bdd877054490d5629f21573/specification/2.0/schema/accessor.schema.json
  auto indicesOffset = indicesBufferView.byteOffset + indicesAccessor.byteOffset;
  auto triangleCount = indicesAccessor.count / 3;
  if (indicesAccessor.componentType == GLB_COMPONENT_UNSIGNED_SHORT) {
    readGlbIndices<uint16_t>(indicesBuffer, meshGeo.triangles, indicesOffset, triangleCount);
  } else if (indicesAccessor.componentType == GLB_COMPONENT_UNSIGNED_INT) {
    readGlbIndices<uint32_t>(indicesBuffer, meshGeo.triangles, indicesOffset, triangleCount);
  } else {
    C8Log(
      "[load-mesh] Triangle indices component %d type not supported.",
      indicesAccessor.componentType);
  }

  // Compute vertex normals if needed.
  if (meshGeo.normals.empty()) {
    computeVertexNormals(meshGeo.points, meshGeo.triangles, &meshGeo.normals);
  }

  return meshGeo;
}

}  // namespace c8
