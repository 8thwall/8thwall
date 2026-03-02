// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"material.h"};
  deps = {
    ":texture",
    "//c8:hmatrix",
    "//c8:map",
    "//c8:string",
    "//c8/string:format",
    "//c8/string:join",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xdf105a8d);

#include "c8/pixels/render/material.h"
#include "c8/string/format.h"
#include "c8/string/join.h"

namespace c8 {

namespace {

String renderSideString(RenderSide s) {
  switch (s) {
    case RenderSide::FRONT:
      return "FRONT";
    case RenderSide::BACK:
      return "BACK";
    case RenderSide::BOTH:
      return "BOTH";
    default:
      return "[unknown]";
  }
}

String blendFunctionString(BlendFunction s) {
  switch (s) {
    case BlendFunction::NORMAL:
      return "NORMAL";
    case BlendFunction::ADDITIVE:
      return "ADDITIVE";
    default:
      return "NORMAL";
  }
}

}  // namespace

Material &Material::setShader(const String &shader) {
  shader_ = shader;
  return *this;
}

Material &Material::setTransparent(bool transparent) {
  transparent_ = transparent;
  return *this;
}

Material &Material::setOpacity(float opacity) {
  opacity_ = opacity;
  return *this;
}

Material &Material::setRenderSide(RenderSide renderSide) {
  renderSide_ = renderSide;
  return *this;
}

Material &Material::setBlendFunction(BlendFunction blendFunction) {
  blendFunction_ = blendFunction;
  return *this;
}

Material &Material::setDepthMask(bool depthMask) {
  depthMask_ = depthMask;
  return *this;
}

Material &Material::setTestDepth(bool testDepth) {
  testDepth_ = testDepth;
  return *this;
}

Material &Material::setColorTexture(std::unique_ptr<Texture> &&colorTexture) {
  colorTexture_ = std::move(colorTexture);
  return *this;
}

Material &Material::setDepthTexture(std::unique_ptr<Texture> &&depthTexture) {
  depthTexture_ = std::move(depthTexture);
  return *this;
}

const Texture* Material::texture(const String &name) const {
  auto it = textures_.find(name);
  if (it != textures_.end()) {
    return it->second.get();
  }
  return nullptr;
}

Texture* Material::texture(const String &name) {
  auto it = textures_.find(name);
  if (it != textures_.end()) {
    return it->second.get();
  }
  return nullptr;
}

Material &Material::setTexture(const String &uniform, std::unique_ptr<Texture> &&texture) {
  auto it = textures_.find(uniform);
  if (it != textures_.end()) {
    it->second = std::move(texture);
  } else {
    textureNames_.push_back(uniform);
    textures_[uniform] = std::move(texture);
  }
  return *this;
}

Material &Material::setBinder(std::unique_ptr<MaterialBinder> &&binder) {
  binder_ = std::move(binder);
  return *this;
}

Material &Material::set(const String &uniform, int a) {
  int1_[uniform] = {a};
  return *this;
}

Material &Material::set(const String &uniform, int a, int b) {
  int2_[uniform] = {a, b};
  return *this;
}

Material &Material::set(const String &uniform, int a, int b, int c) {
  int3_[uniform] = {a, b, c};
  return *this;
}

Material &Material::set(const String &uniform, int a, int b, int c, int d) {
  int4_[uniform] = {a, b, c, d};
  return *this;
}

Material &Material::set(const String &uniform, float a) {
  float1_[uniform] = {a};
  return *this;
}

Material &Material::set(const String &uniform, float a, float b) {
  float2_[uniform] = {a, b};
  return *this;
}

Material &Material::set(const String &uniform, float a, float b, float c) {
  float3_[uniform] = {a, b, c};
  return *this;
}

Material &Material::set(const String &uniform, float a, float b, float c, float d) {
  float4_[uniform] = {a, b, c, d};
  return *this;
}

Material &Material::set(const String &uniform, std::array<int, 1> val) {
  int1_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<int, 2> val) {
  int2_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<int, 3> val) {
  int3_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<int, 4> val) {
  int4_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<float, 1> val) {
  float1_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<float, 2> val) {
  float2_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<float, 3> val) {
  float3_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, std::array<float, 4> val) {
  float4_[uniform] = val;
  return *this;
}

Material &Material::set(const String &uniform, const HMatrix &val) {
  matrix_.emplace(std::make_pair(uniform, val));
  return *this;
}

Vector<SceneMaterialSpec> Material::sceneMaterialSpecs() const {
  Vector<SceneMaterialSpec> specs;
  auto colTexture = colorTexture();
  if (colTexture != nullptr) {
    auto colorMatSpec = colTexture->sceneMaterialSpec();
    if (colorMatSpec != nullptr) {
      specs.push_back(*colorMatSpec);
    }
  }
  auto depTexture = depthTexture();
  if (depTexture != nullptr) {
    auto depMatSpec = depTexture->sceneMaterialSpec();
    if (depMatSpec != nullptr) {
      specs.push_back(*depMatSpec);
    }
  }
  for (const auto &tex : textures_) {
    auto texMatSpec = tex.second->sceneMaterialSpec();
    if (texMatSpec != nullptr) {
      specs.push_back(*texMatSpec);
    }
  }
  return specs;
}

String Material::toString(const String &indent) const {
  return strJoin<String>(
    {
      format("shader:       %s", shader_.c_str()),
      format("colorTexture: %s", colorTexture_ ? colorTexture_->toString().c_str() : "null"),
      format("depthTexture: %s", depthTexture_ ? depthTexture_->toString().c_str() : "null"),
      format("transparent:  %s", transparent_ ? "true" : "false"),
      format("opacity:      %f", opacity_),
      format("renderSide:   %s", renderSideString(renderSide_).c_str()),
      format("float1:       [%d]", float1_.size()),
      format("float2:       [%d]", float2_.size()),
      format("float3:       [%d]", float3_.size()),
      format("float4:       [%d]", float4_.size()),
      format("matrix:       [%d]", matrix_.size()),
      format("blendFunc:    %s", blendFunctionString(blendFunction_).c_str()),
    },
    format("\n%s", indent.c_str()));
}

namespace MatGen {

std::unique_ptr<Material> empty() { return std::make_unique<Material>(); }

std::unique_ptr<Material> colorOnly() {
  auto g = MatGen::empty();
  g->setShader(Shaders::COLOR_ONLY);
  return g;
}

std::unique_ptr<Material> physical() {
  auto g = MatGen::empty();
  g->setShader(Shaders::PHYSICAL);
  return g;
}

std::unique_ptr<Material> image() {
  auto g = MatGen::empty();
  g->setShader(Shaders::IMAGE).setColorTexture(TexGen::empty());
  return g;
}

std::unique_ptr<Material> pointCloudColorOnly() {
  auto g = MatGen::empty();
  g->setShader(Shaders::POINT_CLOUD_COLOR_ONLY);
  g->set(Shaders::POINT_CLOUD_POINT_SIZE, 0.015f);
  return g;
}

std::unique_ptr<Material> pointCloudPhysical() {
  auto g = MatGen::empty();
  g->setShader(Shaders::POINT_CLOUD_PHYSICAL);
  g->set(Shaders::POINT_CLOUD_POINT_SIZE, 0.015f);
  g->set(Shaders::DEPTH_TEXTURE, 1); // Bind depth texture to texture unit 1
  return g;
}

std::unique_ptr<Material> cubemap() {
  auto g = MatGen::empty();
  g->setShader(Shaders::CUBEMAP).setColorTexture(TexGen::empty()).setDepthMask(false);
  return g;
}

std::unique_ptr<Material> splatAttributes() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_ATTRIBUTES)
    .setTransparent(true)
    .setTestDepth(false)
    .setRenderSide(RenderSide::BACK);  // TODO: front
  return g;
}

std::unique_ptr<Material> splatTexture() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_TEXTURE)
    .setTransparent(true)
    .setTestDepth(false)
    .setRenderSide(RenderSide::BACK);  // TODO: front
  return g;
}

std::unique_ptr<Material> splatTextureStacked() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_TEXTURE_STACKED)
    .setTransparent(true)
    .setTestDepth(false)
    .setRenderSide(RenderSide::BACK);  // TODO: front
  return g;
}

std::unique_ptr<Material> splatMultiTexSh() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_MULTITEX_SH)
    .setTransparent(false)  // Overwrite previous colors (including alpha) without blending.
    .setTestDepth(false)
    .setRenderSide(RenderSide::FRONT);
  return g;
}

std::unique_ptr<Material> splatMultiTexFinal() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_MULTITEX_FINAL)
    .setTransparent(true)
    .setTestDepth(false)
    .setRenderSide(RenderSide::BACK);  // TODO: front
  return g;
}

std::unique_ptr<Material> splatSortedTexture() {
  auto g = MatGen::empty();
  g->setShader(Shaders::SPLAT_SORTED_TEXTURE)
    .setTransparent(true)
    .setTestDepth(false)
    .setRenderSide(RenderSide::BACK);  // TODO: front
  return g;
}

// Renders a subscene's renderSpec onto the mesh (typically a quad).
std::unique_ptr<Material> subsceneMaterial(
  const String &subsceneName, const String &renderSpecName) {
  auto g = MatGen::empty();
  auto tex = TexGen::empty();
  tex->setSceneTexture(subsceneName, renderSpecName);
  g->setShader(Shaders::IMAGE).setColorTexture(std::move(tex));
  return g;
}

std::unique_ptr<Material> transparent(std::unique_ptr<Material> &&material) {
  material->setTransparent(true);
  return std::move(material);
}

}  // namespace MatGen

}  // namespace c8
