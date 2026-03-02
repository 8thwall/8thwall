// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include "c8/geometry/splat.h"
#include "c8/hpoint.h"
#include "c8/model/model-config.h"
#include "c8/model/model-data.h"
#include "c8/quaternion.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class ModelManager {
public:
  // Default constructors.
  ModelManager() = default;
  ModelManager(ModelManager &&) = default;
  ModelManager &operator=(ModelManager &&) = default;

  // Disallow copying.
  ModelManager(const ModelManager &) = delete;
  ModelManager &operator=(const ModelManager &) = delete;

  void loadModel(const String &filename, const uint8_t *data, size_t size);
  void loadModel(
    const String &filename1,
    const uint8_t *data1,
    size_t size1,
    const String &filename2,
    const uint8_t *data2,
    size_t size2);

  void mergeModel(
    const String &filename,
    const uint8_t *data,
    size_t size,
    HPoint3 position,
    Quaternion rotation);

  bool updateView(
    const HPoint3 &cameraPos,
    const Quaternion &cameraRot,
    const HPoint3 &modelPos,
    const Quaternion &modelRot);

  int serializeModel(Vector<uint8_t> &data);

  // Pick which coordinate system you want the data exported
  void configure(const ModelManagerConfig &config) {
    config_ = config;
    forceUpdate_ = true;
  }

private:
  ModelData model_;
  ModelManagerConfig config_;
  // If we need to update the texture ids, we need to know the original attributes, but these
  // should not be included in the serialized model.
  SplatAttributes loadedAttributes_;
  HMatrix modelCamera_ = HMatrixGen::i();      // Camera in model space (inverse model-view matrix).
  HMatrix lastModelCamera_ = HMatrixGen::i();  // Last model camera with an update.
  SortSplatScratch sortScratch_;
  bool forceUpdate_ = false;
};

}  // namespace c8
