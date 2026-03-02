// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "model-manager.h",
  };
  deps = {
    ":model-config",
    ":model-data",
    ":splat-skybox",
    "//c8/model:multi-array",
    "//c8:c8-log",
    "//c8:hpoint",
    "//c8:quaternion",
    "//c8:scope-exit",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:box3",
    "//c8/geometry:egomotion",
    "//c8/geometry:load-mesh",
    "//c8/geometry:load-splat",
    "//c8/geometry:octree",
    "//c8/geometry:splat",
    "//c8/geometry:vectors",
    "//c8/io:image-io",
    "//c8/pixels:base64",
    "//c8/stats:scope-timer",
    "//c8/string:contains",
    "@json",
    "@tinygltf",
  };
}
cc_end(0xc1f1b8e4);

#include <nlohmann/json.hpp>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/geometry/box3.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/load-mesh.h"
#include "c8/geometry/load-splat.h"
#include "c8/geometry/vectors.h"
#include "c8/io/image-io.h"
#include "c8/model/model-manager.h"
#include "c8/model/multi-array.h"
#include "c8/model/splat-skybox.h"
#include "c8/pixels/base64.h"
#include "c8/scope-exit.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/contains.h"
#include "tiny_gltf.h"

namespace c8 {

namespace {

bool updateSplat(
  const ModelManagerConfig &config,
  const HMatrix &modelCamera,
  const SplatAttributes &loadedAttributes,
  ModelData *model,
  SortSplatScratch *sortScratch) {
  if (model->splatAttributes != nullptr) {
    *(model->splatAttributes) = sortSplatAttributes(
      loadedAttributes,
      *model->octree,
      modelCamera,
      {.lowToHigh = (config.coordinateSpace == ModelConfig::CoordinateSpace::RUB),
       .maxDistToOrigin = config.pointPruneDistance,
       .minDistToCamera = config.pointMinDistance,
       .cullRayX = config.pointFrustumLimit,
       .cullRayY = config.pointFrustumLimit,
       .minSplatSize = config.pointSizeLimit,
       .scratch = sortScratch});
    return true;
  }

  if (model->splatTexture != nullptr) {
    if (config.sortTexture) {
      // TODO: sort and then fill these directly into the existing texture, rather than creating new
      // intermediate buffers.
      auto newSplat = sortSplatAttributes(
        loadedAttributes,
        *model->octree,
        modelCamera,
        {.lowToHigh = (config.coordinateSpace == ModelConfig::CoordinateSpace::RUB),
         .maxDistToOrigin = config.pointPruneDistance,
         .minDistToCamera = config.pointMinDistance,
         .cullRayX = config.pointFrustumLimit,
         .cullRayY = config.pointFrustumLimit,
         .minSplatSize = config.pointSizeLimit,
         .scratch = sortScratch});
      model->splatTexture->header = newSplat.header;
      model->splatTexture->sortedIds = {};
      model->splatTexture->texture = splatTexture(newSplat);
    } else {
      sortSplatIds(
        loadedAttributes,
        *model->octree,
        modelCamera,
        {.lowToHigh = (config.coordinateSpace == ModelConfig::CoordinateSpace::RUB),
         .maxDistToOrigin = config.pointPruneDistance,
         .minDistToCamera = config.pointMinDistance,
         .cullRayX = config.pointFrustumLimit,
         .cullRayY = config.pointFrustumLimit,
         .minSplatSize = config.pointSizeLimit,
         .scratch = sortScratch},
        &model->splatTexture->sortedIds);
      model->splatTexture->header.numPoints = model->splatTexture->sortedIds.size();
    }
    return true;
  }

  if (model->splatMultiTexture != nullptr) {
    model->splatMultiTexture->header.numPoints = sortSplatIds(
      loadedAttributes,
      *model->octree,
      modelCamera,
      {.lowToHigh = (config.coordinateSpace == ModelConfig::CoordinateSpace::RUB),
       .maxDistToOrigin = config.pointPruneDistance,
       .minDistToCamera = config.pointMinDistance,
       .cullRayX = config.pointFrustumLimit,
       .cullRayY = config.pointFrustumLimit,
       .minSplatSize = config.pointSizeLimit,
       .scratch = sortScratch},
      &model->splatMultiTexture->sortedIds);
    return true;
  }

  return false;
}

void initSplat(
  const ModelManagerConfig &config, SplatAttributes &loadedAttributes, ModelData *model) {
  *model = {};

  model->octree = std::make_unique<SplatOctreeNode>(loadedAttributes.positions, 0);
  if (config.useOctree) {
    for (int i = 0; i < loadedAttributes.positions.size(); i++) {
      model->octree->insert(loadedAttributes.positions, i);
    }
  }

  if (!config.preferTexture) {
    model->splatAttributes = std::make_unique<SplatAttributes>();
    *model->splatAttributes = loadedAttributes;
  }

  if (config.preferTexture && !config.multiTexture) {
    model->splatTexture = std::make_unique<SplatTexture>();
    model->splatTexture->header = loadedAttributes.header;
    model->splatTexture->texture = splatTexture(loadedAttributes);
    if (config.sortTexture) {
      model->splatTexture->sortedIds = {};
    } else {
      model->splatTexture->sortedIds.resize(loadedAttributes.positions.size());
      std::iota(model->splatTexture->sortedIds.begin(), model->splatTexture->sortedIds.end(), 0);
    }
    model->splatTexture->skybox = loadedAttributes.skybox;
  }

  if (config.preferTexture && config.multiTexture) {
    model->splatMultiTexture = std::make_unique<SplatMultiTexture>();
    model->splatMultiTexture->header = loadedAttributes.header;
    model->splatMultiTexture->sortedIds = sortedIds(loadedAttributes);
    model->splatMultiTexture->positionColor = positionColor(loadedAttributes);
    model->splatMultiTexture->rotationScale = rotationScale(loadedAttributes);
    model->splatMultiTexture->shRG = sh(loadedAttributes, 0);
    model->splatMultiTexture->shB = sh(loadedAttributes, 2);
    model->splatMultiTexture->skybox = loadedAttributes.skybox;
  }
}

void rufToRub(MeshGeometry *mesh) {
  for (auto &p : mesh->points) {
    p = {p.x(), p.y(), -p.z()};
  }
  for (auto &n : mesh->normals) {
    n = {n.x(), n.y(), -n.z()};
  }
  for (auto &t : mesh->triangles) {
    t = {t.c, t.b, t.a};
  }
}

void rdfToRuf(MeshGeometry *mesh) {
  for (auto &p : mesh->points) {
    p = {p.x(), -p.y(), p.z()};
  }
  for (auto &n : mesh->normals) {
    n = {n.x(), -n.y(), n.z()};
  }
  for (auto &u : mesh->uvs) {
    u = {u.x(), 1.0f - u.y()};
  }
}

void rufToRub(PointCloudGeometry *pointCloud) {
  for (auto &p : pointCloud->points) {
    p = {p.x(), p.y(), -p.z()};
  }
  for (auto &n : pointCloud->normals) {
    n = {n.x(), n.y(), -n.z()};
  }
}

bool loadDrcMesh(
  const uint8_t *data, size_t size, ModelData *model, ModelConfig::CoordinateSpace space) {
  ScopeTimer t("decode-drc");
  MeshGeometry geo = {};
  Vector<uint8_t> meshBytes(data, data + size);
  auto maybeDecodedMesh = dracoDataToMesh(&meshBytes, false);
  if (
    !maybeDecodedMesh.has_value()
    || (maybeDecodedMesh.has_value() && maybeDecodedMesh->triangles.size() == 0)) {
    return false;
  }
  model->mesh = std::make_unique<MeshData>();
  model->mesh->geometry = std::move(std::move(*maybeDecodedMesh));
  if (space == ModelConfig::CoordinateSpace::RDF) {
    rdfToRuf(&model->mesh->geometry);
  } else {
    // Convert from RUB to RUF (same as RUF to RUB)
    rufToRub(&model->mesh->geometry);
  }
  return true;
}

bool loadSpz(
  const ModelManagerConfig &config,
  const HMatrix &modelCamera,
  const uint8_t *data,
  size_t size,
  ModelData *model,
  SplatAttributes *loadedAttributes,
  SortSplatScratch *sortScratch) {
  ScopeTimer t("decode-packed-splat");
  // Data is in RUB
  *loadedAttributes = splatAttributes(
    loadSplatSpzDataPacked(data, size),
    config.coordinateSpace != ModelConfig::CoordinateSpace::RUB);

  *loadedAttributes = mortonSortSplatAttributes(*loadedAttributes);

  if (config.bakeSkyboxMeters > 0.0f) {
    // TODO: Pass coordinate space, configure skybox resolution.
    *loadedAttributes = bakeSplatSkybox(*loadedAttributes, config.bakeSkyboxMeters, 512);
  }

  initSplat(config, *loadedAttributes, model);
  updateSplat(config, modelCamera, *loadedAttributes, model, sortScratch);
  return true;
}

bool loadSplatGeneric(
  const ModelManagerConfig &config,
  const HMatrix &modelCamera,
  const uint8_t *data,
  size_t size,
  ModelData *model,
  SplatAttributes *loadedAttributes,
  SortSplatScratch *sortScratch) {
  ScopeTimer t("decode-splat");
  // Data is in RUB
  *loadedAttributes = splatAttributes(
    loadSplat(data, size), config.coordinateSpace != ModelConfig::CoordinateSpace::RUB);

  if (config.bakeSkyboxMeters > 0.0f) {
    // TODO: Pass coordinate space, configure skybox resolution.
    *loadedAttributes = bakeSplatSkybox(*loadedAttributes, config.bakeSkyboxMeters, 512);
  }

  initSplat(config, *loadedAttributes, model);
  updateSplat(config, modelCamera, *loadedAttributes, model, sortScratch);
  return true;
}

bool loadGlb(const uint8_t *data, size_t size, ModelData *model) {
  ScopeTimer t("decode-glb");
  tinygltf::Model gltf;
  tinygltf::TinyGLTF tinygltfLoader;
  std::string err;
  std::string warn;
  bool ret = tinygltfLoader.LoadBinaryFromMemory(&gltf, &err, &warn, data, size);

  if (!ret) {
    C8Log("[model-manager] glbDataToMesh Error: %s", err.c_str());
    return false;
  }

  model->mesh = std::make_unique<MeshData>();
  model->mesh->geometry = tinyGLTFModelToMesh(gltf);

  auto gltfImage = gltf.images[0];
  model->mesh->texture = RGBA8888PlanePixelBuffer(gltfImage.height, gltfImage.width);
  auto textureImagePix = model->mesh->texture.pixels();

  std::copy(gltfImage.image.begin(), gltfImage.image.end(), &*textureImagePix.pixels());

  return true;
}

bool loadMar(const uint8_t *data, size_t size, ModelData *model) {
  ScopeTimer t("decode-mar");
  auto mesh = marDataToMesh(data, size, true);

  if (!mesh) {
    C8Log("WARNING: Invalid mar data");
    return false;
  }

  model->mesh = std::make_unique<MeshData>();
  model->mesh->geometry = std::move(*mesh);
  model->mesh->texture = RGBA8888PlanePixelBuffer(1, 1);
  model->mesh->texture.pixels().pixels()[0] = Color::PURPLE.r();
  model->mesh->texture.pixels().pixels()[1] = Color::PURPLE.g();
  model->mesh->texture.pixels().pixels()[2] = Color::PURPLE.b();
  model->mesh->texture.pixels().pixels()[3] = Color::PURPLE.a();

  return true;
}

bool loadPtz(const uint8_t *data, size_t size, ModelData *model) {
  ScopeTimer t("decode-ptz");
  Vector<uint8_t> buffer(data, data + size);
  // TODO: Add support for compressed point clouds. If point clouds used gzip encoding something
  // like the following should work, but it doesn't seem to be the case.
  // if (!MultiArray::isMultiArray(buffer)) {
  //   Vector<uint8_t> buffer2;
  //   auto success = decompressGzipped(buffer, &buffer2);
  //   if (!success) {
  //     C8Log("WARNING: Invalid zip data for file '%s', not loading", filename.c_str());
  //     return;
  //   }
  //   buffer = std::move(buffer2);
  // }

  if (!MultiArray::isMultiArray(buffer)) {
    C8Log("WARNING: Invalid multiarray content.");
    return false;
  }

  MultiArray array = MultiArray(buffer);

  if (array.count() != 3) {
    C8Log("WARNING: Invalid point cloud data.");
    return false;
  }

  const auto points = array.floatBuffer(0);
  const auto normals = array.floatBuffer(1);
  const auto colors = array.ucharBuffer(2);

  // The point cloud is in RUB, so we need to negate the Z coordinate.
  float nz = -1.0f;

  model->pointCloud = std::make_unique<PointCloudGeometry>();

  // Each point is represented by 3 floats (x, y, z), so the total points is positions.size() / 3
  model->pointCloud->points.resize(points.size() / 3);
  for (int i = 0, j = 0; i < points.size(); ++j, i += 3) {
    model->pointCloud->points[j] = HPoint3(points[i], points[i + 1], nz * points[i + 2]);
  }

  model->pointCloud->normals.resize(normals.size() / 3);
  for (int i = 0, j = 0; i < normals.size(); ++j, i += 3) {
    model->pointCloud->normals[j] = HVector3(normals[i], normals[i + 1], nz * normals[i + 2]);
  }

  model->pointCloud->colors.resize(colors.size() / 3);
  for (int i = 0, j = 0; i < colors.size(); ++j, i += 3) {
    model->pointCloud->colors[j] = Color(colors[i], colors[i + 1], colors[i + 2]);
  }

  return true;
}

bool loadMeshJson(const uint8_t *data, size_t size, ModelData *model) {
  ScopeTimer t("decode-mesh-json");
  auto dataChars = reinterpret_cast<const char *>(data);
  auto json = nlohmann::json::parse(dataChars, dataChars + size);
  if (!json.contains("meshData") || !json.contains("textureData")) {
    C8Log("WARNING: Invalid mesh json data.");
    return false;
  }
  auto meshBytesStr = json["meshData"].get<std::string>();
  auto textureBytesStr = json["textureData"].get<std::string>();

  auto meshBytes = decode(meshBytesStr);
  auto textureBytes = decode(textureBytesStr);

  auto maybeDecodedMesh = dracoDataToMesh(&meshBytes, true);
  if (maybeDecodedMesh.has_value() && maybeDecodedMesh->triangles.size() == 0) {
    return false;
  }
  model->mesh = std::make_unique<MeshData>();
  model->mesh->geometry = std::move(std::move(*maybeDecodedMesh));
  // For VPS meshes, we need to flip the UVs.
  for (auto &uv : model->mesh->geometry.uvs) {
    uv = {uv.x(), 1.0f - uv.y()};
  }
  model->mesh->texture = readJpgToRGBA(textureBytes.data(), textureBytes.size());

  return true;
}

}  // namespace

void ModelManager::loadModel(
  const String &filename1,
  const uint8_t *data1,
  size_t size1,
  const String &filename2,
  const uint8_t *data2,
  size_t size2) {
  ScopeTimer t("model-manager-load-model-multi-file");
  String modelFilename;
  const uint8_t *modelData;
  size_t modelSize;
  String imageFilename;
  const uint8_t *imageData;
  size_t imageSize;

  if (endsWith(filename1, ".jpg") || endsWith(filename1, ".png")) {
    imageFilename = filename1;
    imageData = data1;
    imageSize = size1;
    modelFilename = filename2;
    modelData = data2;
    modelSize = size2;
  } else if (endsWith(filename2, ".jpg") || endsWith(filename2, ".png")) {
    imageFilename = filename2;
    imageData = data2;
    imageSize = size2;
    modelFilename = filename1;
    modelData = data1;
    modelSize = size1;
  } else {
    C8Log("WARNING: Loading two files but one is not an image.");
    return;
  }

  loadModel(modelFilename, modelData, modelSize);

  if (model_.mesh == nullptr) {
    C8Log("WARNING: Loading two files but the model is not a mesh.");
    model_ = {};
    return;
  }

  if (endsWith(imageFilename, ".jpg")) {
    ScopeTimer t1("read-jpg-to-rgba");
    model_.mesh->texture = readJpgToRGBA(imageData, imageSize);
  } else if (endsWith(imageFilename, ".png")) {
    ScopeTimer t1("read-png-to-rgba");
    model_.mesh->texture = readPngToRGBA(imageData, imageSize);
  }
}

void ModelManager::loadModel(const String &filename, const uint8_t *data, size_t size) {
  ScopeTimer t("model-manager-load-model");
  // On exit, convert the model to RUB if requested and it's not already converted.
  SCOPE_EXIT([&] {
    if (config_.coordinateSpace == ModelConfig::CoordinateSpace::RUF) {
      // Model is already in RUF, nothing to do.
      return;
    }
    // Convert Mesh
    if (model_.mesh != nullptr) {
      rufToRub(&model_.mesh->geometry);
      return;
    }
    // Convert Point Cloud
    if (model_.pointCloud != nullptr) {
      rufToRub(model_.pointCloud.get());
      return;
    }
    // Model is a splat, we already did the conversion at splat decode time.
  });

  model_ = {};

  // TODO: Infer file type from data instead of filename.
  if (endsWith(filename, ".drc") && loadDrcMesh(data, size, &model_, config_.coordinateSpace)) {
    return;
  }
  // Couldn't decode mesh. Trying to load splat now.

  if (
    endsWith(filename, ".spz")
    && loadSpz(config_, modelCamera_, data, size, &model_, &loadedAttributes_, &sortScratch_)) {
    return;
  }

  if (
    (endsWith(filename, ".ply") || endsWith(filename, ".drc"))
    && loadSplatGeneric(
      config_, modelCamera_, data, size, &model_, &loadedAttributes_, &sortScratch_)) {
    return;
  }

  if (endsWith(filename, ".glb") && loadGlb(data, size, &model_)) {
    return;
  }

  if (endsWith(filename, ".mar") && loadMar(data, size, &model_)) {
    return;
  }

  if (endsWith(filename, ".ptz") && loadPtz(data, size, &model_)) {
    return;
  }

  if (endsWith(filename, ".json") && loadMeshJson(data, size, &model_)) {
    return;
  }

  C8Log("WARNING: Unable to load file '%s', not loading", filename.c_str());
}

void ModelManager::mergeModel(
  const String &filename, const uint8_t *data, size_t size, HPoint3 position, Quaternion rotation) {
  ScopeTimer t("model-manager-merge-model");

  // TODO: Infer file type from data instead of filename.
  if (endsWith(filename, ".ply") || endsWith(filename, ".spz") || endsWith(filename, ".drc")) {
    if (loadedAttributes_.positions.empty()) {
      C8Log("[model-manager] WARNING: Merging a splat with no existing splat data.");
      return;
    }
    ScopeTimer t1("decode-splat");
    // Data is in RUB
    auto attributes = splatAttributes(
      loadSplat(data, size), config_.coordinateSpace != ModelConfig::CoordinateSpace::RUB);

    auto transform = cameraMotion(position, rotation);
    for (int i = 0; i < attributes.positions.size(); i++) {
      auto splatTransform = cameraMotion(attributes.positions[i], attributes.rotations[i]);
      auto newSplatTransform = transform * splatTransform;
      attributes.positions[i] = asPoint(trsTranslation(newSplatTransform));
      attributes.rotations[i] = trsRotation(newSplatTransform);
    }

    C8Log(
      "Merging %d splats with %d splats",
      loadedAttributes_.positions.size(),
      attributes.positions.size());

    loadedAttributes_.positions.insert(
      loadedAttributes_.positions.end(), attributes.positions.begin(), attributes.positions.end());
    loadedAttributes_.rotations.insert(
      loadedAttributes_.rotations.end(), attributes.rotations.begin(), attributes.rotations.end());
    loadedAttributes_.scales.insert(
      loadedAttributes_.scales.end(), attributes.scales.begin(), attributes.scales.end());
    loadedAttributes_.colors.insert(
      loadedAttributes_.colors.end(), attributes.colors.begin(), attributes.colors.end());
    loadedAttributes_.shRed.insert(
      loadedAttributes_.shRed.end(), attributes.shRed.begin(), attributes.shRed.end());
    loadedAttributes_.shGreen.insert(
      loadedAttributes_.shGreen.end(), attributes.shGreen.begin(), attributes.shGreen.end());
    loadedAttributes_.shBlue.insert(
      loadedAttributes_.shBlue.end(), attributes.shBlue.begin(), attributes.shBlue.end());

    C8Log("Now there are %d splats", loadedAttributes_.positions.size());
    initSplat(config_, loadedAttributes_, &model_);
    updateSplat(config_, modelCamera_, loadedAttributes_, &model_, &sortScratch_);
    return;
  }
}

bool ModelManager::updateView(
  const HPoint3 &cameraPos,
  const Quaternion &cameraRot,
  const HPoint3 &modelPos,
  const Quaternion &modelRot) {
  ScopeTimer t("model-manager-update-view");
  if (
    model_.splatAttributes == nullptr && model_.splatTexture == nullptr
    && model_.splatMultiTexture == nullptr) {
    // not a splat, no need to sort.
    return false;
  }

  bool forceUpdate = forceUpdate_;
  forceUpdate_ = false;

  // Get the camera pose in the model's coordinate system.
  HMatrix cameraPose = cameraMotion(cameraPos, cameraRot);
  HMatrix modelPose = cameraMotion(modelPos, modelRot);
  modelCamera_ = egomotion(modelPose, cameraPose);

  auto modelCameraRot = trsRotation(modelCamera_);
  auto lastModelCameraRot = trsRotation(lastModelCamera_);

  auto modelCameraPos = trsTranslation(modelCamera_);
  auto lastModelCameraPos = trsTranslation(lastModelCamera_);

  auto distThresh = config_.splatResortMeters * config_.splatResortMeters;

  if (
    !forceUpdate && modelCameraRot.radians(lastModelCameraRot) < config_.splatResortRadians
    && modelCameraPos.sqdist(lastModelCameraPos) < distThresh) {
    return false;
  }

  lastModelCamera_ = modelCamera_;

  // Now that we're just updating, we don't need to pass the texture around anymore.
  if (model_.splatTexture != nullptr) {
    model_.splatTexture->texture = {};
    model_.splatTexture->skybox = {};
  }

  if (model_.splatMultiTexture != nullptr) {
    model_.splatMultiTexture->positionColor = {};
    model_.splatMultiTexture->rotationScale = {};
    model_.splatMultiTexture->shRG = {};
    model_.splatMultiTexture->shB = {};
    model_.splatMultiTexture->skybox = {};
    // don't set sortedIds to empty, we still need it
  }

  return updateSplat(config_, modelCamera_, loadedAttributes_, &model_, &sortScratch_);
}

int ModelManager::serializeModel(Vector<uint8_t> &data) {
  ScopeTimer t("model-manager-serialize-model");
  return serialize(model_, &data);
}

}  // namespace c8
