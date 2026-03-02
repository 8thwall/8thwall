// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"assets.h"};
  deps = {
    "//c8:c8-log",
    "//c8:vector",
    "//c8:string",
    "//c8/geometry:load-mesh",
    "//c8/geometry:mesh-types",
    "//c8/io:image-io",
    "//c8/pixels/render:object8",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xa411a6f4);

#include "c8/c8-log.h"
#include "c8/geometry/load-mesh.h"
#include "c8/geometry/mesh-types.h"
#include "c8/io/image-io.h"
#include "c8/pixels/render/assets.h"

namespace c8 {

std::unique_ptr<Renderable> readMarAndTexture(const String &marPath, const String &texturePath) {
  auto meshGeo = marFileToMesh(marPath).value();
  auto imageData = readJpgToRGBA(texturePath);

  auto mesh = ObGen::meshGeometry(meshGeo);
  mesh->setMaterial(MatGen::image());
  mesh->material().setColorTexture(TexGen::rgbaPixelBuffer(std::move(imageData)));
  return mesh;
}

std::unique_ptr<Renderable> glbFileToMesh(const String &assetPath) {
  tinygltf::Model model;
  tinygltf::TinyGLTF tinygltfLoader;
  std::string err;
  std::string warn;

  bool ret = tinygltfLoader.LoadBinaryFromFile(&model, &err, &warn, assetPath.c_str());

  if (ret) {
    auto meshGeo = tinyGLTFModelToMesh(model);
    auto mesh = ObGen::meshGeometry(meshGeo);

    // Copy the texture image.
    if (model.images.size() > 0) {
      auto gltfImage = model.images[0];
      RGBA8888PlanePixelBuffer textureImageBuffer =
        RGBA8888PlanePixelBuffer(gltfImage.height, gltfImage.width);
      auto textureImagePix = textureImageBuffer.pixels();

      std::copy(gltfImage.image.begin(), gltfImage.image.end(), &*textureImagePix.pixels());

      mesh->setMaterial(MatGen::image());
      mesh->material().setColorTexture(TexGen::rgbaPixelBuffer(std::move(textureImageBuffer)));
    } else {
      mesh->setMaterial(MatGen::physical());
    }

    return mesh;
  } else {
    C8Log("[load-mesh] glbFileToMesh Error: %s", err.c_str());
    return nullptr;
  }
}

std::unique_ptr<Renderable> glbDataToMesh(const uint8_t *bytes, int numBytes) {
  tinygltf::Model model;
  tinygltf::TinyGLTF tinygltfLoader;
  std::string err;
  std::string warn;

  bool ret = tinygltfLoader.LoadBinaryFromMemory(&model, &err, &warn, bytes, numBytes);

  if (ret) {
    auto meshGeo = tinyGLTFModelToMesh(model);

    // Copy the texture image.
    auto gltfImage = model.images[0];
    RGBA8888PlanePixelBuffer textureImageBuffer =
      RGBA8888PlanePixelBuffer(gltfImage.height, gltfImage.width);
    auto textureImagePix = textureImageBuffer.pixels();

    std::copy(gltfImage.image.begin(), gltfImage.image.end(), &*textureImagePix.pixels());

    auto mesh = ObGen::meshGeometry(meshGeo);
    mesh->setMaterial(MatGen::image());
    mesh->material().setColorTexture(TexGen::rgbaPixelBuffer(std::move(textureImageBuffer)));

    return mesh;
  } else {
    C8Log("[load-mesh] glbFileToMesh Error: %s", err.c_str());
    return nullptr;
  }
}

}  // namespace c8
