// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#pragma once

#include <functional>

#include "c8/geometry/mesh-types.h"

namespace c8 {

using MeshDecodedCb = std::function<void(const String &, MeshGeometry &&)>;

void forceDecodeMeshBlockingCpp();

void decodeMesh(
  const String &meshId, const String &encodedBase64Mesh, const MeshDecodedCb &callback);

}  // namespace c8
