// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"assimp-assets.h"};
  deps = {
    "//c8:c8-log",
    "//c8:vector",
    "//c8:string",
    "//c8/geometry:load-mesh",
    "//c8/geometry:mesh-types",
    "//c8/io:image-io",
    "//c8/pixels/render:object8",
    "//third_party/assimp:assimp",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb7e64070);

#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "c8/c8-log.h"
#include "c8/geometry/load-mesh.h"
#include "c8/geometry/mesh-types.h"
#include "c8/io/image-io.h"
#include "c8/pixels/render/assimp-assets.h"

namespace c8 {

namespace {

// Print mesh data for the scene, to show an idea of the data structures and their contents.
void printMeshData(const aiScene *aiscene) {
  C8Log("[assets] mNumMeshes: %d", aiscene->mNumMeshes);
  C8Log("[assets] mNumMaterials: %d", aiscene->mNumMaterials);
  C8Log("[assets] mNumAnimations: %d", aiscene->mNumAnimations);
  C8Log("[assets] mNumTextures: %d", aiscene->mNumTextures);
  C8Log("[assets] mNumLights: %d", aiscene->mNumLights);
  C8Log("[assets] mNumCameras: %d", aiscene->mNumCameras);
  C8Log("[assets] mMetaData: %p", aiscene->mMetaData);
  C8Log("[assets] mName: %s", aiscene->mName.C_Str());

  const auto *msh = aiscene->mMeshes[0];
  C8Log("[assets] msh->mMaterialIndex: %d", msh->mMaterialIndex);
  C8Log("[assets] msh->mNumVertices: %d", msh->mNumVertices);
  C8Log("[assets] msh->mNumFaces: %d", msh->mNumFaces);
  C8Log("[assets] msh->mNumUVComponents[0] %d", msh->mNumUVComponents[0]);
  C8Log("[assets] msh->GetNumColorChannels(): %d", msh->GetNumColorChannels());
  C8Log("[assets] msh->GetNumUVChannels(): %d: ", msh->GetNumUVChannels());
  C8Log("[assets] msh->HasBones(): %d", msh->HasBones());
  C8Log("[assets] msh->HasFaces(): %d", msh->HasFaces());
  C8Log("[assets] msh->HasNormals(): %d", msh->HasNormals());
  C8Log("[assets] msh->HasPositions(): %d", msh->HasPositions());
  C8Log("[assets] msh->HasTangentsAndBitangents(): %d", msh->HasTangentsAndBitangents());
  C8Log("[assets] msh->HasTextureCoords(0): %d", msh->HasTextureCoords(0));
  C8Log("[assets] msh->HasVertexColors(0): %d", msh->HasVertexColors(0));

  const auto *vertices = msh->mVertices;
  const auto ve = msh->mNumVertices - 1;
  C8Log("[assets] msh->mVertices[0]: (%f, %f, %f)", vertices[0].x, vertices[0].y, vertices[0].z);
  C8Log(
    "[assets] msh->mVertices[ve]: (%f, %f, %f)", vertices[ve].x, vertices[ve].y, vertices[ve].z);

  const auto *faces = msh->mFaces;
  const auto fe = msh->mNumFaces - 1;
  C8Log("[assets] msh->mFaces[0].mNumIndices: %d ", faces[0].mNumIndices);
  C8Log("[assets] msh->mFaces[0].mIndices[0]: %d ", faces[0].mIndices[0]);
  C8Log("[assets] msh->mFaces[0].mIndices[1]: %d ", faces[0].mIndices[1]);
  C8Log("[assets] msh->mFaces[0].mIndices[2]: %d ", faces[0].mIndices[2]);
  C8Log("[assets] msh->mFaces[fe].mNumIndices: %d ", faces[fe].mNumIndices);
  C8Log("[assets] msh->mFaces[fe].mIndices[0]: %d ", faces[fe].mIndices[0]);
  C8Log("[assets] msh->mFaces[fe].mIndices[1]: %d ", faces[fe].mIndices[1]);
  C8Log("[assets] msh->mFaces[fe].mIndices[2]: %d ", faces[fe].mIndices[2]);
  C8Log("[assets] msh->mTextureCoords[0][0].x: %f ", msh->mTextureCoords[0][0].x);
  C8Log("[assets] msh->mTextureCoords[0][0].y: %f ", msh->mTextureCoords[0][0].y);
  C8Log("[assets] msh->mTextureCoords[0][ve].x: %f ", msh->mTextureCoords[0][ve].x);
  C8Log("[assets] msh->mTextureCoords[0][ve].y: %f ", msh->mTextureCoords[0][ve].y);

  const auto *mtl = aiscene->mMaterials[msh->mMaterialIndex];
  C8Log("[assets] mtl->GetName(): %s", mtl->GetName().C_Str());
  C8Log("[assets] mtl->GetTextureCount(AMBIENT): %d", mtl->GetTextureCount(aiTextureType_AMBIENT));
  C8Log(
    "mtl->GetTextureCount(AMBIENT_OCCLUSION): %d",
    mtl->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION));
  C8Log(
    "[assets] mtl->GetTextureCount(BASE_COLOR): %d",
    mtl->GetTextureCount(aiTextureType_BASE_COLOR));
  C8Log("[assets] mtl->GetTextureCount(DIFFUSE): %d", mtl->GetTextureCount(aiTextureType_DIFFUSE));
  C8Log(
    "mtl->GetTextureCount(DIFFUSE_ROUGHNESS): %d",
    mtl->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS));
  C8Log(
    "[assets] mtl->GetTextureCount(DISPLACEMENT): %d",
    mtl->GetTextureCount(aiTextureType_DISPLACEMENT));
  C8Log(
    "mtl->GetTextureCount(EMISSION_COLOR): %d", mtl->GetTextureCount(aiTextureType_EMISSION_COLOR));
  C8Log(
    "[assets] mtl->GetTextureCount(EMISSIVE): %d", mtl->GetTextureCount(aiTextureType_EMISSIVE));
  C8Log("[assets] mtl->GetTextureCount(HEIGHT): %d", mtl->GetTextureCount(aiTextureType_HEIGHT));
  C8Log(
    "[assets] mtl->GetTextureCount(LIGHTMAP): %d", mtl->GetTextureCount(aiTextureType_LIGHTMAP));
  C8Log(
    "[assets] mtl->GetTextureCount(METALNESS): %d", mtl->GetTextureCount(aiTextureType_METALNESS));
  C8Log("[assets] mtl->GetTextureCount(NONE): %d", mtl->GetTextureCount(aiTextureType_NONE));
  C8Log(
    "mtl->GetTextureCount(NORMAL_CAMERA): %d", mtl->GetTextureCount(aiTextureType_NORMAL_CAMERA));
  C8Log("[assets] mtl->GetTextureCount(NORMALS): %d", mtl->GetTextureCount(aiTextureType_NORMALS));
  C8Log("[assets] mtl->GetTextureCount(OPACITY): %d", mtl->GetTextureCount(aiTextureType_OPACITY));
  C8Log(
    "[assets] mtl->GetTextureCount(REFLECTION): %d",
    mtl->GetTextureCount(aiTextureType_REFLECTION));
  C8Log(
    "[assets] mtl->GetTextureCount(SHININESS): %d", mtl->GetTextureCount(aiTextureType_SHININESS));
  C8Log(
    "[assets] mtl->GetTextureCount(SPECULAR): %d", mtl->GetTextureCount(aiTextureType_SPECULAR));
  C8Log("[assets] mtl->GetTextureCount(UNKNOWN): %d", mtl->GetTextureCount(aiTextureType_UNKNOWN));

  if (!aiscene->mNumTextures) {
    aiString texPath;
    mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
    C8Log("[assets] texPath: %s", texPath.C_Str());
    return;
  }

  const auto *texture = aiscene->mTextures[0];
  C8Log("[assets] texture->mHeight: %d", texture->mHeight);
  C8Log("[assets] texture->mWidth: %d", texture->mWidth);
  C8Log("[assets] texture->pcData: %p", texture->pcData);

  if (texture->mWidth && !texture->mHeight) {
    auto texBuf = readJpgToRGBA(reinterpret_cast<uint8_t *>(texture->pcData), texture->mWidth);
    C8Log("[assets] texBuf.pixels().rows(): %d", texBuf.pixels().rows());
    C8Log("[assets] texBuf.pixels().cols(): %d", texBuf.pixels().cols());
  }
}

// Extracts a renderable mesh from an aiScene assuming there is only one mesh with only one texture
// and it is diffuse and encoded with JPG encoding. This is not a general aiScene parser.
std::unique_ptr<Renderable> diffuseJpgMesh(const aiScene *aiscene) {
  if (!aiscene) {
    C8Log("[assets] WARNING: Couldn't decode scene");
    return nullptr;
  }

  const auto *aimesh = aiscene->mMeshes[0];
  MeshGeometry geo;
  geo.points.reserve(aimesh->mNumVertices);
  for (int i = 0; i < aimesh->mNumVertices; ++i) {
    const auto &p = aimesh->mVertices[i];
    geo.points.push_back({p.x, p.y, -p.z});  // Make left handed
  }
  geo.uvs.reserve(aimesh->mNumVertices);
  for (int i = 0; i < aimesh->mNumVertices; ++i) {
    const auto &p = aimesh->mTextureCoords[0][i];
    geo.uvs.push_back({p.x, 1.0f - p.y});  // We're inverting Vs here
  }
  geo.triangles.reserve(aimesh->mNumFaces);
  for (int i = 0; i < aimesh->mNumFaces; ++i) {
    const auto &p = aimesh->mFaces[i].mIndices;
    geo.triangles.push_back({p[2], p[1], p[0]});  // Switch winding order to match handedness.
  }

  auto mesh = ObGen::meshGeometry(geo);
  if (!aiscene->mNumTextures) {
    aiString texPath;
    aiscene->mMaterials[aimesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
    C8Log("[assets] WARNING: No texture data embedded in aiScene, load from '%s'", texPath.C_Str());
    return mesh;
  }

  const auto *aitex = aiscene->mTextures[0];

  // The texture height should be 0, indicating that an encoding is used.
  if (aitex->mHeight) {
    C8Log("[assets] WARNING: Texture data (%dx%d) is not encoded.", aitex->mWidth, aitex->mHeight);
    return mesh;
  }

  if (!aitex->mWidth) {
    C8Log("[assets] WARNING: Texture has no data");
    return mesh;
  }

  // The texture height 0 and has non-zero width, meaning
  mesh->setMaterial(MatGen::image());
  mesh->material().setColorTexture(
    TexGen::rgbaPixelBuffer(
      readJpgToRGBA(reinterpret_cast<uint8_t *>(aitex->pcData), aitex->mWidth)));
  return mesh;
}

}  // namespace

std::unique_ptr<Renderable> readDiffuseJpgMesh(const String &filename) {
  return diffuseJpgMesh(Assimp::Importer{}.ReadFile(filename.c_str(), 0));
}

std::unique_ptr<Renderable> readDiffuseJpgMeshGlb(const Vector<uint8_t> &data) {
  return diffuseJpgMesh(Assimp::Importer{}.ReadFileFromMemory(data.data(), data.size(), 0, "glb"));
}

std::unique_ptr<Renderable> readDiffuseJpgMeshFbx(const Vector<uint8_t> &data) {
  return diffuseJpgMesh(Assimp::Importer{}.ReadFileFromMemory(data.data(), data.size(), 0, "fbx"));
}

void printDebugMeshData(const String &filename) {
  printMeshData(Assimp::Importer{}.ReadFile(filename.c_str(), 0));
}

void printDebugMeshData(const Vector<uint8_t> &data, const String &fileFormat) {
  printMeshData(
    Assimp::Importer{}.ReadFileFromMemory(data.data(), data.size(), 0, fileFormat.c_str()));
}

}  // namespace c8
