// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

namespace c8 {

struct ModelConfig {
  enum CoordinateSpace {
    UNSPECIFIED = 0,
    RUF = 1,
    RUB = 2,
    RDF = 3,
  };

  CoordinateSpace coordinateSpace = CoordinateSpace::RUF;
};

struct ModelManagerConfig {
  ModelConfig::CoordinateSpace coordinateSpace = ModelConfig::CoordinateSpace::RUF;
  float splatResortRadians = 0.18f;  // 10 degrees;
  float splatResortMeters = 0.2f;    // 7.5 inches;
  float bakeSkyboxMeters = 0.0f;     // 0.0f means no skybox baking.
  bool sortTexture = false;
  bool multiTexture = false;
  bool preferTexture = true;
  float pointPruneDistance = 0.0f;  // 0.0f means no pruning
  float pointFrustumLimit = 1.7f;   // 120 Degree FOV. 0 means no frustum culling.
  float pointSizeLimit = 1e-4f;     // 0.3mm @ 1m.
  float pointMinDistance = 0.0f;    // minimum distance to camera. 0.0f is no culling.
  bool useOctree = false;
};

}  // namespace c8
