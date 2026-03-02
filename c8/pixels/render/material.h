// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <array>
#include <memory>

#include "c8/hmatrix.h"
#include "c8/map.h"
#include "c8/pixels/render/texture.h"
#include "c8/string.h"

namespace c8 {

// Shaders that are built into the renderer. Materials can use these directly, or callers can
// register their own shaders.
namespace Shaders {

// Shaders
constexpr const char *COLOR_ONLY = "coloronly";
constexpr const char *PHYSICAL = "physical";
constexpr const char *IMAGE = "image";
constexpr const char *POINT_CLOUD_COLOR_ONLY = "pointcloudcoloronly";
constexpr const char *POINT_CLOUD_PHYSICAL = "pointcloudphysical";
constexpr const char *DEPTH = "depth";
constexpr const char *CUBEMAP = "cubemap";
constexpr const char *SPLAT_ATTRIBUTES = "splatattributes";
constexpr const char *SPLAT_MULTITEX_SH = "splatmultitexsh";
constexpr const char *SPLAT_MULTITEX_FINAL = "splatmultitexfinal";
constexpr const char *SPLAT_TEXTURE = "splattexture";
constexpr const char *SPLAT_TEXTURE_STACKED = "splattexturestacked";
constexpr const char *SPLAT_SORTED_TEXTURE = "splatsortedtexture";

// Material Uniforms
constexpr const char *POINT_CLOUD_POINT_SIZE = "pointSize";

// Renderer Vertex Attributes
constexpr const char *POSITION = "position";
constexpr const char *NORMAL = "normal";
constexpr const char *UV = "uv";
constexpr const char *COLOR = "color";
constexpr const char *TANGENT = "tangent";
constexpr const char *INSTANCE_POSITION = "instancePosition";
constexpr const char *INSTANCE_ROTATION = "instanceRotation";
constexpr const char *INSTANCE_SCALE = "instanceScale";
constexpr const char *INSTANCE_COLOR = "instanceColor";
constexpr const char *INSTANCE_ID = "instanceId";

// Renderer Uniforms
constexpr const char *RENDERER_HAS_COLOR_TEXTURE = "hasColorTexture";
constexpr const char *RENDERER_HAS_DEPTH_TEXTURE = "hasDepthTexture";
constexpr const char *RENDERER_MV = "mv";
constexpr const char *RENDERER_P = "p";
constexpr const char *RENDERER_MVP = "mvp";
constexpr const char *RENDERER_OPACITY = "opacity";
constexpr const char *RENDERER_RAYS_TO_PIX = "raysToPix";
constexpr const char *RENDERER_ANTIALIASED = "antialiased";
constexpr const char *RENDERER_STACKED_INSTANCE_COUNT = "stackedInstanceCount";
constexpr const char *RENDERER_SPLAT_DATA_TEX = "splatDataTexture";
constexpr const char *RENDERER_SPLAT_IDS_TEX = "splatIdsTexture";
constexpr const char *RENDERER_SPLAT_POSITIONCOLOR_TEX = "positionColorTexture";
constexpr const char *RENDERER_SPLAT_ROTATIONSCALE_TEX = "rotationScaleTexture";
constexpr const char *RENDERER_SPLAT_SHCOLOR_TEX = "shColorTexture";
constexpr const char *RENDERER_SPLAT_SHRG_TEX = "shRGTexture";
constexpr const char *RENDERER_SPLAT_SHB_TEX = "shBTexture";
constexpr const char *NORMAL_MATRIX = "normalMatrix";
constexpr const char *POINT_LIGHTS = "pointLights";
constexpr const char *DIRECTIONAL_LIGHTS = "directionalLights";
constexpr const char *AMBIENT_LIGHT = "ambientLight";
constexpr const char *CAMERA_NEAR = "near";
constexpr const char *CAMERA_FAR = "far";
constexpr const char *COLOR_TEXTURE = "colorSampler";
constexpr const char *DEPTH_TEXTURE = "depthSampler";
constexpr const char *OCCLUDE_TOLERANCE = "occludeTol";

}  // namespace Shaders

// Abstract base class for binding this material. The implementation of this is specific to a
// gpu framework (e.g. vulkan vs. metal vs. opengl) and this will be set by the renderer.
class MaterialBinder {
public:
  virtual void bind() = 0;
  virtual ~MaterialBinder() = default;
};

// Specifies which side of a geometry to render.
enum class RenderSide {
  FRONT = 0,
  BACK = 1,
  BOTH = 2,
};

enum class BlendFunction {
  NORMAL = 0,
  ADDITIVE = 1,
};

// A material defines the parameters that control how a geometry is rendered. This includes
// parameters to the gpu framework (enable transparency, depth function, face drawing sid, etc.),
// parameters to the shader (uniforms), texture resources, and hints to the rendering engine about
// what stages of rendering to participate in (shadow map generation, etc.).
class Material {
public:
  const String &shader() const { return shader_; }

  // Enable transparency on this material. If a material is not transparent, all opacity information
  // from the material, vertex colors, or texture will be ignored.
  bool transparent() const { return transparent_; }
  Material &setTransparent(bool val);

  // For transparent materials, modify their global opacity by this value. Transparent must be set
  // to true for this to have an effect. Note that if a Material is transparent, it can have
  // transparency built into its vertex colors or its texture. This global opacity level will
  // further attenuate those individual values.
  float opacity() const { return opacity_; }
  Material &setOpacity(float opacity);

  // Set the side of the renderable's geometry that is rendered. By default, both sides are
  // rendered.
  RenderSide renderSide() const { return renderSide_; }
  Material &setRenderSide(RenderSide renderSide);

  BlendFunction blendFunction() const { return blendFunction_; }
  Material &setBlendFunction(BlendFunction blendFunction_);

  bool depthMask() const { return depthMask_; }
  Material &setDepthMask(bool depthMask);

  bool testDepth() const { return testDepth_; }
  Material &setTestDepth(bool testDepth_);

  Material &setShader(const String &shader);

  // Standard shader color texture. This is used by several standard shaders for color information,
  // and if present, the `hasColorTexture` uniform is set to true if this is set or false if not.
  Material &setColorTexture(std::unique_ptr<Texture> &&colorTexture);
  const Texture *colorTexture() const { return colorTexture_.get(); }
  Texture *colorTexture() { return colorTexture_.get(); }

  // Depth texture for shadow maps or depth maps. This is used by several standard shaders for
  // depth information, and if present, the `hasDepthTexture` uniform is set to true if this is set
  // or false if not.
  Material &setDepthTexture(std::unique_ptr<Texture> &&depthTexture);
  const Texture *depthTexture() const { return depthTexture_.get(); }
  Texture *depthTexture() { return depthTexture_.get(); }

  // Other textures that are not color or depth textures. Shaders that use any number of custom
  // custom textures should use the `setTexture()` api. This API is mutually exclusive with the
  // setColorTexture / setDepthTexture APIs.
  //
  // When setting textures, the names must match the names of the uniforms in the shader.
  //
  // When using this API, custom shader sampler uniform names should be passed along with the names
  // of other uniforms when the shader is registered with the renderer.
  Material &setTexture(const String &name, std::unique_ptr<Texture> &&texture);
  const Vector<String> &textureNames() const { return textureNames_; }
  const Texture *texture(const String &name) const;
  Texture *texture(const String &name);

  MaterialBinder *binder() { return binder_.get(); }
  Material &setBinder(std::unique_ptr<MaterialBinder> &&binder);

  // Set custom values for this material. These will be mapped to uniforms in the shader.
  Material &set(const String &uniform, int a);
  Material &set(const String &uniform, int a, int b);
  Material &set(const String &uniform, int a, int b, int c);
  Material &set(const String &uniform, int a, int b, int c, int d);
  Material &set(const String &uniform, float a);
  Material &set(const String &uniform, float a, float b);
  Material &set(const String &uniform, float a, float b, float c);
  Material &set(const String &uniform, float a, float b, float c, float d);
  Material &set(const String &uniform, std::array<int, 1> val);
  Material &set(const String &uniform, std::array<int, 2> val);
  Material &set(const String &uniform, std::array<int, 3> val);
  Material &set(const String &uniform, std::array<int, 4> val);
  Material &set(const String &uniform, std::array<float, 1> val);
  Material &set(const String &uniform, std::array<float, 2> val);
  Material &set(const String &uniform, std::array<float, 3> val);
  Material &set(const String &uniform, std::array<float, 4> val);
  Material &set(const String &uniform, const HMatrix &val);

  const TreeMap<String, std::array<int, 1>> &int1() const { return int1_; }
  const TreeMap<String, std::array<int, 2>> &int2() const { return int2_; }
  const TreeMap<String, std::array<int, 3>> &int3() const { return int3_; }
  const TreeMap<String, std::array<int, 4>> &int4() const { return int4_; }
  const TreeMap<String, std::array<float, 1>> &float1() const { return float1_; };
  const TreeMap<String, std::array<float, 2>> &float2() const { return float2_; };
  const TreeMap<String, std::array<float, 3>> &float3() const { return float3_; };
  const TreeMap<String, std::array<float, 4>> &float4() const { return float4_; };
  const TreeMap<String, HMatrix> &matrix() const { return matrix_; };

  // Right now only the color texture can make this material depends on a SceneMaterialSpec
  // In the future, other textures can increase the number of SceneMaterialSpecs this material
  // depends on
  Vector<SceneMaterialSpec> sceneMaterialSpecs() const;

  String toString(const String &indent) const;
  String toString() const { return toString(""); }

private:
  String shader_;
  std::unique_ptr<Texture> colorTexture_;
  std::unique_ptr<Texture> depthTexture_;  // For shadow or depth maps
  Vector<String> textureNames_;
  TreeMap<String, std::unique_ptr<Texture>> textures_;  // For other textures
  bool transparent_ = false;
  float opacity_ = 1.0f;
  RenderSide renderSide_ = RenderSide::BOTH;
  BlendFunction blendFunction_ = BlendFunction::NORMAL;
  bool testDepth_ = true;
  bool depthMask_ = true;

  std::unique_ptr<MaterialBinder> binder_;

  TreeMap<String, std::array<int, 1>> int1_;
  TreeMap<String, std::array<int, 2>> int2_;
  TreeMap<String, std::array<int, 3>> int3_;
  TreeMap<String, std::array<int, 4>> int4_;
  TreeMap<String, std::array<float, 1>> float1_;
  TreeMap<String, std::array<float, 2>> float2_;
  TreeMap<String, std::array<float, 3>> float3_;
  TreeMap<String, std::array<float, 4>> float4_;
  TreeMap<String, HMatrix> matrix_;
};

// Factory methods for creating Material instances.
namespace MatGen {

// Basic factory methods for creating a material with default data.
std::unique_ptr<Material> empty();
std::unique_ptr<Material> colorOnly();
std::unique_ptr<Material> physical();
std::unique_ptr<Material> image();
std::unique_ptr<Material> pointCloudColorOnly();
std::unique_ptr<Material> pointCloudPhysical();
std::unique_ptr<Material> cubemap();
std::unique_ptr<Material> splatAttributes();
std::unique_ptr<Material> splatTexture();
std::unique_ptr<Material> splatTextureStacked();
std::unique_ptr<Material> splatMultiTexSh();
std::unique_ptr<Material> splatMultiTexFinal();
std::unique_ptr<Material> splatSortedTexture();
std::unique_ptr<Material> subsceneMaterial(
  const String &subsceneName, const String &renderSpecName = "");
std::unique_ptr<Material> transparent(std::unique_ptr<Material> &&material);

}  // namespace MatGen

}  // namespace c8
