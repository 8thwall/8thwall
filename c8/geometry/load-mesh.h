// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathan@nianticlabs.com)
//
// Methods for loading mesh data from third party libraries, such as Draco, as a c8 MeshGeometry.

#pragma once

#include <optional>

#include "c8/geometry/mesh-types.h"
#include "tiny_gltf.h"

namespace c8 {

// Loads a draco file as a MeshGeometry.
// @param assetPath relative to c8 root path.
// @param isBase64Encoded bool specifying whether the file is a base64 encoded draco file or a
// binary draco file.
// @param narToXr. Convert a mesh from NAR coordinate space to xr (left-handed) coordinate system.
// @return MeshGeometry that contains the draco file's vertices, triangles, vertex colors, and
// computed vertex normals. It will not have uvs.
std::optional<MeshGeometry> dracoFileToMesh(
  const String &assetPath, bool isBase64Encoded, bool narToXr = false);

// Converts draco binary data into a MeshGeometry.
// @param dracoBinaryData. If your data is base64 encoded then you should have already decoded
// the data before calling this function.
// @param narToXr. Convert a mesh from NAR coordinate space to xr (left-handed) coordinate system.
// @return MeshGeometry that contains the draco file's vertices, triangles, vertex colors, and
// computed vertex normals. It will not have uvs
std::optional<MeshGeometry> dracoDataToMesh(Vector<uint8_t> *dracoBinaryData, bool narToXr = false);

// Loads a .mar file's binary data into a MeshGeometry.
// @param marData binary data for the .mar file.
// @return MeshGeometry that contains the file's vertices, triangles, vertex normals, and uvs. It
// will not have the texture or vertex colors.
std::optional<MeshGeometry> marDataToMesh(const Vector<uint8_t> &marData);
std::optional<MeshGeometry> marDataToMesh(const uint8_t *data, size_t size, bool narToXr = false);

// Loads a .mar file from Scaniverse as a MeshGeometry.
// @param assetPath relative to c8 root path.
// @return MeshGeometry that contains the file's vertices, triangles, vertex normals, and uvs. It
// will not have the texture or vertex colors.
std::optional<MeshGeometry> marFileToMesh(const String &assetPath);

// Loads a tinygltf model as a Renderable geometry.
// @param model tinygltf model loaded from a glb.
MeshGeometry tinyGLTFModelToMesh(const tinygltf::Model &model);

}  // namespace c8
