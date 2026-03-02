// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include <memory>

#include "c8/pixels/render/object8.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {
// Loads the Scaniverse .mar file and corresponding texture.
std::unique_ptr<Renderable> readMarAndTexture(const String &marPath, const String &texturePath);


// Loads a .glb file as a Renderable geometry.
// @param assetPath relative to c8 root path.
std::unique_ptr<Renderable> glbFileToMesh(const String &assetPath);

// Loads a .glb file data as a Renderable geometry.
// @param data data read from the glb file
std::unique_ptr<Renderable> glbDataToMesh(const uint8_t *bytes, int numBytes) ;
}  // namespace c8
