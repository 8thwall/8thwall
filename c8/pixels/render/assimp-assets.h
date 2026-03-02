// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include <memory>

#include "c8/pixels/render/object8.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// Extracts a renderable mesh from a file (e.g. fbx, glb) assuming there is only one mesh with only
// one texture and it is diffuse and encoded with JPG encoding. This is not a general model parser.
// The filetype is inferred from the path name.
std::unique_ptr<Renderable> readDiffuseJpgMesh(const String &filename);

// Extracts a renderable mesh from file data (e.g. fbx, glb) assuming there is only one mesh with
// only one texture and it is diffuse and encoded with JPG encoding. This is not a general model
// parser. The file is assumed to be a GLB.
std::unique_ptr<Renderable> readDiffuseJpgMeshGlb(const Vector<uint8_t> &data);

// Extracts a renderable mesh from file data (e.g. fbx, glb) assuming there is only one mesh with
// only one texture and it is diffuse and encoded with JPG encoding. This is not a general model
// parser. The file is assumed to be an FBX.
std::unique_ptr<Renderable> readDiffuseJpgMeshFbx(const Vector<uint8_t> &data);

// Get internal mesh format information for debugging purposes.
void printDebugMeshData(const String &filename);
void printDebugMeshData(const Vector<uint8_t> &data, const String &fileFormat);

}  // namespace c8
