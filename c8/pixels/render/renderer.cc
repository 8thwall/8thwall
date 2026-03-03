// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
#include "c8/pixels/render/material.h"

cc_library {
  hdrs = {"renderer.h"};
  deps = {
    ":geometry",
    ":object8",
    "//c8:c8-log",
    "//c8:map",
    "//c8:set",
    "//c8/geometry:egomotion",
    "//c8/geometry:mesh",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels/opengl:client-gl",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:gl-vertex-array",
    "//c8/pixels/opengl:texture-transforms",
    "//c8/pixels/render/shaders:render-shaders",
    "//c8/pixels/render:uniform",
    "//c8/string:containers",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe012fa2f);

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/mesh.h"
#include "c8/map.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-program.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl-vertex-array.h"
#include "c8/pixels/opengl/texture-transforms.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/render/geometry.h"
#include "c8/pixels/render/renderer.h"
#include "c8/pixels/render/shaders/render-shaders.h"
#include "c8/set.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/containers.h"

#if C8_OPENGL_VERSION_2
#include "c8/pixels/opengl/glext.h"
#endif

namespace c8 {

namespace {

#if C8_OPENGL_VERSION_2
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
static PFNGLBLITFRAMEBUFFERANGLEPROC glBlitFramebuffer = nullptr;
#endif

void initExtensions() {
#if C8_OPENGL_VERSION_2
  // Magic static to prevent initialization order issues with extension functions.
  static bool isInit = false;
  if (!isInit) {
    glBlitFramebuffer =
      (PFNGLBLITFRAMEBUFFERANGLEPROC)clientGlGetProcAddress("glBlitFramebufferANGLE");
  }
  isInit = true;
#endif
}

}  // namespace

// NOTE: Keep this in sync with physical-lighting-frag.glsl.
const int NUM_POINT_LIGHTS = 5;
const int NUM_DIR_LIGHTS = 5;

namespace {

// Computes the number of ray units that correspond to one pixel for a clip space projection.
float raysToPixForProjection(const HMatrix &projection, int pixelsWidth) {
  auto raysLeft = (projection.inv() * HPoint3(-1.0f, 0.0f, 0.0f)).flatten().x();
  auto raysRight = (projection.inv() * HPoint3(1.0f, 0.0f, 0.0f)).flatten().x();
  return static_cast<float>(pixelsWidth) / (raysRight - raysLeft);
}

}  // namespace

namespace {
constexpr int INTERLEAVED_VERTEX_BUFFER_ID = 0;
constexpr int INTERLEAVED_INSTANCE_BUFFER_ID = 1;
}  // namespace

#ifdef __EMSCRIPTEN__
bool isWebGL2() {
  String version = c8_glVersion();
  String shadingVersion = c8_glShadingVersion();
  if (shadingVersion.find("GLSL ES 3.00") != std::string::npos) {
    return true;
  }
  if (version.find("WebGL 2") != std::string::npos) {
    return true;
  }
  return false;
}
#endif

// OpenGL implementation of GeometryRenderer.
class GlGeometryRenderer : public GeometryRenderer {
public:
  void initializeVertexArray(Vector<String> attributes) {
    if (dirtyVertexArray_) {
      vertexArray_.initialize(attributes);
      dirtyVertexArray_ = false;
    }
  }

  void render() override {
    vertexArray_.bind();
    checkGLError("[Geometry] render: bind");
    if (expectedInstances_ > 0) {
      vertexArray_.drawElementsInstanced(expectedInstances_);
      checkGLError("[Geometry] render: drawElementsInstanced");
    } else {
      vertexArray_.drawElements();
      checkGLError("[Geometry] render: drawElements");
    }
  }

  unsigned long getGpuDataType(GpuDataType type) override {
    switch (type) {
      case GpuDataType::FLOAT:
        return GL_FLOAT;
      case GpuDataType::INT:
        return GL_INT;
      case GpuDataType::UINT:
        return GL_UNSIGNED_INT;
      case GpuDataType::BYTE:
        return GL_UNSIGNED_BYTE;
    }
  }

  void update(Geometry &geometry) {
    if (geometry.verticesDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::POSITION,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.vertices().size() * sizeof(HPoint3),
        geometry.vertices().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: add vertices");
    }
    if (geometry.colorsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::COLOR,
        4,
        GL_UNSIGNED_BYTE,
        GL_FALSE,
        0,
        geometry.colors().size() * sizeof(Color),
        geometry.colors().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: add color");
    }
    if (geometry.normalsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::NORMAL,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.normals().size() * sizeof(HVector3),
        geometry.normals().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: add normals");
    }
    if (geometry.uvsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::UV,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.uvs().size() * sizeof(HVector2),
        geometry.uvs().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: add uvs");
    }
    if (geometry.instancePositionsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::INSTANCE_POSITION,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.instancePositions().size() * sizeof(HPoint3),
        geometry.instancePositions().data(),
        GL_STATIC_DRAW);
      vertexArray_.setDivisor(Shaders::INSTANCE_POSITION, 1);
      expectedInstances_ = geometry.instancePositions().size();
      checkGLError("[Geometry] update: add instance ids");
    }
    if (geometry.instanceRotationsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::INSTANCE_ROTATION,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.instanceRotations().size() * sizeof(Quaternion),
        geometry.instanceRotations().data(),
        GL_STATIC_DRAW);
      vertexArray_.setDivisor(Shaders::INSTANCE_ROTATION, 1);
      expectedInstances_ = geometry.instanceRotations().size();
      checkGLError("[Geometry] update: add instance rotations");
    }
    if (geometry.instanceScalesDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::INSTANCE_SCALE,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        geometry.instanceScales().size() * sizeof(HVector3),
        geometry.instanceScales().data(),
        GL_STATIC_DRAW);
      vertexArray_.setDivisor(Shaders::INSTANCE_SCALE, 1);
      expectedInstances_ = geometry.instanceScales().size();
      checkGLError("[Geometry] update: add instance scales");
    }
    if (geometry.instanceColorsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::INSTANCE_COLOR,
        4,
        GL_UNSIGNED_BYTE,
        GL_FALSE,
        0,
        geometry.instanceColors().size() * sizeof(Color),
        geometry.instanceColors().data(),
        GL_STATIC_DRAW);
      vertexArray_.setDivisor(Shaders::INSTANCE_COLOR, 1);
      expectedInstances_ = geometry.instanceColors().size();
      checkGLError("[Geometry] update: add instance colors");
    }
    if (geometry.instanceIdsDirty()) {
      vertexArray_.addVertexBuffer(
        Shaders::INSTANCE_ID,
        1,
        GL_UNSIGNED_INT,
        GL_FALSE,
        0,
        geometry.instanceIds().size() * sizeof(uint32_t),
        geometry.instanceIds().data(),
        GL_DYNAMIC_DRAW);
      vertexArray_.setDivisor(Shaders::INSTANCE_ID, 1);
      expectedInstances_ = geometry.instanceIds().size();
      checkGLError("[Geometry] update: add instance colors");
    }
    if (geometry.instanceCount() > 0) {
      expectedInstances_ = geometry.instanceCount();
    }
    if (geometry.trianglesDirty()) {
      vertexArray_.setIndexBuffer(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        geometry.triangles().size() * sizeof(MeshIndices),
        geometry.triangles().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: set triangles index");
    }
    if (geometry.pointsDirty()) {
      vertexArray_.setIndexBuffer(
        GL_POINTS,
        GL_UNSIGNED_INT,
        geometry.points().size() * sizeof(PointIndices),
        geometry.points().data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: set points index");
    }
    if (geometry.interleavedVertexDataDirty()) {
      auto &interleavedVertex = geometry.interleavedVertexData();
      vertexArray_.setInterleavedBuffer(
        INTERLEAVED_VERTEX_BUFFER_ID,
        interleavedVertex.data.size(),
        interleavedVertex.data.data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: set interleaved buffer");

      for (const auto &a : interleavedVertex.attributes) {
        vertexArray_.setInterleavedAttribute(
          INTERLEAVED_VERTEX_BUFFER_ID,
          a.name,
          a.numChannels,
          getGpuDataType(a.type),
          static_cast<GLboolean>(a.normalized),
          a.stride,
          a.offset);
        checkGLError("[Geometry] update: set interleaved attribute");
      }
    }
    if (geometry.interleavedInstanceDataDirty()) {
      auto &interleavedInstance = geometry.interleavedInstanceData();
      vertexArray_.setInterleavedBuffer(
        INTERLEAVED_INSTANCE_BUFFER_ID,
        interleavedInstance.data.size(),
        interleavedInstance.data.data(),
        GL_STATIC_DRAW);
      checkGLError("[Geometry] update: set interleaved instance buffer");

      for (const auto &a : interleavedInstance.attributes) {
        vertexArray_.setInterleavedAttribute(
          INTERLEAVED_INSTANCE_BUFFER_ID,
          a.name,
          a.numChannels,
          getGpuDataType(a.type),
          static_cast<GLboolean>(a.normalized),
          a.stride,
          a.offset);
        vertexArray_.setDivisor(a.name, 1);
        checkGLError("[Geometry] update: set interleaved instance attribute");
      }
      expectedInstances_ =
        interleavedInstance.data.size() / interleavedInstance.attributes[0].stride;
    }
    geometry.setClean();
  }

private:
  GlVertexArray vertexArray_;
  bool dirtyVertexArray_ = true;
  int expectedInstances_ = 0;
};

class GlTextureBinder : public TextureBinder {
public:
  void bind(int slot) override {
    if (nativeId_ >= 0) {
      glBindTexture(nativeTarget_, nativeId_);
      checkGLError("[GlTextureBinder] bindTexture: bindTexture(nativeId)");
    } else if (pixels_.width() > 0) {
      pixels_.bind();
      checkGLError("[GlTextureBinder] bindTexture: bindTexture(pixels)");
    } else {
      // If there is no nativeId or rgba pixels to render, then we set the GL texture to 0.
      // Without this, it'll render the previous GL texture.
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
      checkGLError("[GlTextureBinder] bindTexture: 0");
    }
  }

  void update(Scene *rootScene, Texture *texture) {
    // If the texture has scene material spec, then we need to retrieve the color texture attached
    // to the subscene's fbo to be used as this object's texture.
    if (texture->sceneMaterialSpec() != nullptr) {
      auto spec = texture->sceneMaterialSpec();
      auto &subscene = rootScene->find<Scene>(spec->subsceneName);
      texture->setNativeId(subscene.renderer()->nativeTextureId(spec->renderSpecName));
    }
    static int maxTextureSize = getMaxTextureSize();
    if (texture->hasNativeId()) {
      nativeId_ = texture->nativeId();
      switch (texture->nativeTarget()) {
        case TextureTarget::TEXTURE_CUBE_MAP:
          nativeTarget_ = GL_TEXTURE_CUBE_MAP;
          break;

        case TextureTarget::TEXTURE_2D:
        default:
          nativeTarget_ = GL_TEXTURE_2D;
          break;
      }
    } else {
      nativeId_ = -1;
      nativeTarget_ = GL_TEXTURE_2D;

      if (texture->hasRgbaPixels()) {
        if (
          texture->rgbaPixels().cols() > maxTextureSize
          || texture->rgbaPixels().rows() > maxTextureSize) {
          C8Log(
            "[GlTextureBinder] WARNING: RGBA texture size %dx%d exceeds max texture size %dx%d",
            texture->rgbaPixels().cols(),
            texture->rgbaPixels().rows(),
            maxTextureSize,
            maxTextureSize);
          return;
        }
        if (
          pixels_.width() != texture->rgbaPixels().cols()
          || pixels_.height() != texture->rgbaPixels().rows()) {
          pixels_ =
            makeLinearRGBA8888Texture2D(texture->rgbaPixels().cols(), texture->rgbaPixels().rows());
        }
        if (texture->rgbaPixelsDirty()) {
          pixels_.bind();
          pixels_.updateImage(texture->rgbaPixels().pixels());
        }
      } else if (texture->hasR32Pixels()) {
        if (
          texture->r32Pixels().cols() > maxTextureSize
          || texture->r32Pixels().rows() > maxTextureSize) {
          C8Log(
            "[GlTextureBinder] WARNING: R32 texture size %dx%d exceeds max texture size %dx%d",
            texture->r32Pixels().cols(),
            texture->r32Pixels().rows(),
            maxTextureSize,
            maxTextureSize);
          return;
        }
        if (
          pixels_.width() != texture->r32Pixels().cols()
          || pixels_.height() != texture->r32Pixels().rows()) {
          pixels_ =
            makeNearestR32Texture2D(texture->r32Pixels().cols(), texture->r32Pixels().rows());
        }
        if (texture->r32PixelsDirty()) {
          pixels_.bind();
          pixels_.updateImage(texture->r32Pixels().pixels());
        }
      } else if (texture->hasRgba32Pixels()) {
        if (
          texture->rgba32Pixels().cols() > maxTextureSize
          || texture->rgba32Pixels().rows() > maxTextureSize) {
          C8Log(
            "[GlTextureBinder] WARNING: RGBA32 texture size %dx%d exceeds max texture size %dx%d",
            texture->rgba32Pixels().cols(),
            texture->rgba32Pixels().rows(),
            maxTextureSize,
            maxTextureSize);
          return;
        }
        if (
          pixels_.width() != texture->rgba32Pixels().cols()
          || pixels_.height() != texture->rgba32Pixels().rows()) {
          pixels_ = makeNearestRGBA32Texture2D(
            texture->rgba32Pixels().cols(), texture->rgba32Pixels().rows());
        }
        if (texture->rgba32PixelsDirty()) {
          pixels_.bind();
          pixels_.updateImage(texture->rgba32Pixels().pixels());
        }
      } else if (texture->hasDepthPixels()) {
        if (
          texture->depthPixels().cols() > maxTextureSize
          || texture->depthPixels().rows() > maxTextureSize) {
          C8Log(
            "[GlTextureBinder] WARNING: Depth texture size %dx%d exceeds max texture size %dx%d",
            texture->depthPixels().cols(),
            texture->depthPixels().rows(),
            maxTextureSize,
            maxTextureSize);
          return;
        }
        if (
          pixels_.width() != texture->depthPixels().cols()
          || pixels_.height() != texture->depthPixels().rows()) {
#if C8_OPENGL_VERSION_2
          C8Log("WARNING: GL_R32F and GL_RED texture formats not supported on OpenGL2");
#endif  // OpenGL2
          pixels_ =
            makeLinearR32FTexture2D(texture->depthPixels().cols(), texture->depthPixels().rows());
        }
        if (texture->depthPixelsDirty()) {
          pixels_.bind();
          pixels_.updateImage(texture->depthPixels().pixels());
        }
      }
    }

    texture->setClean();
  }

private:
  int getMaxTextureSize() {
    int maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    return maxTextureSize;
  }
  int nativeId_ = -1;
  uint32_t nativeTarget_ = GL_TEXTURE_2D;
  GlTexture2D pixels_;
};

struct PointLightData {
  HVector3 posInView;
  HVector3 color;
  float intensity;
};

struct DirectionalLightData {
  HVector3 dirInView;
  HVector3 color;
  float intensity;
};

struct MaterialBindingParams {
  Object8 *object = nullptr;
  Scene *sceneRoot = nullptr;
  Material *material = nullptr;
  GlProgramObject *shader = nullptr;
  float raysToPix = 0.0f;
  const HMatrix *mv = nullptr;
  const HMatrix *p = nullptr;
  const HMatrix *mvp = nullptr;
  const Vector<PointLightData> *pointLights = nullptr;
  const Vector<DirectionalLightData> *directionalLights = nullptr;
  const HVector3 *ambientLight = nullptr;
  int numBoundTextureSlots_ = 0;
};

// OpenGL implementation of MaterialBinder.
class GlMaterialBinder : public MaterialBinder {
public:
  void bind() override {
    bindShader();
    bindTextures();
  }

  void update(const MaterialBindingParams &params) { params_ = params; }

private:
  void bindShader() {
    if (params_.shader == nullptr) {
      return;
    }

    switch (params_.material->renderSide()) {
      case RenderSide::FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
      case RenderSide::BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
      case RenderSide::BOTH:
        glDisable(GL_CULL_FACE);
        break;
      default:
        // shouldn't get here.
        break;
    }

    // By default, the engine always writes to the depth mask.
    // For special materials, e.g. cubemaps, we need to turn off writing to depth mask,
    // then turn it back on after the draw calls.
    const bool disableDepthMask = !params_.material->depthMask();
    if (disableDepthMask) {
      glDepthMask(GL_FALSE);
    }

    const bool disableDepthTest = !params_.material->testDepth();
    if (disableDepthTest) {
      glDisable(GL_DEPTH_TEST);
    } else {
      glEnable(GL_DEPTH_TEST);
    }

    glUseProgram(params_.shader->id());
    checkGLError("[GlMaterialBinder] bindShader: use program");

    // Bind the shader's samplers to texture slots. The texture names should match the names in the
    // shader.
    {
      const auto &textureNames = params_.material->textureNames();
      if (!textureNames.empty()) {
        // Bind texture names to slots in the order they were added to the material.
        for (int i = params_.numBoundTextureSlots_; i < textureNames.size(); i++) {
          auto textureLoc = params_.shader->location(textureNames[i]);
          if (textureLoc >= 0) {
            glUniform1i(textureLoc, i);
            checkGLError("[GlMaterialBinder] bindShader: set texture slot");
          } else {
            // Even if something went wrong, we want to keep the slot / name count in sync so that
            // the next texture is bound to the correct slot.
            C8Log(
              "[GlMaterialBinder] bindShader: sampler %s not found in shader",
              textureNames[i].c_str());
          }
          params_.numBoundTextureSlots_++;
        }
      }
    }

    // optionally pass raysToPix to a uniform
    {
      auto loc = params_.shader->location(Shaders::RENDERER_RAYS_TO_PIX);
      if (loc >= 0) {
        glUniform1f(loc, params_.raysToPix);
        checkGLError("[GlMaterialBinder] bindShader: set raysToPix");
      }
    }

    // Check if the shader supports Shaders::RENDERER_HAS_COLOR_TEXTURE as a uniform, and if so,
    // pass the object's float value to the shader.
    {
      auto loc = params_.shader->location(Shaders::RENDERER_HAS_COLOR_TEXTURE);
      if (loc >= 0) {
        auto hasColorTexture = params_.material->colorTexture() != nullptr;

        glUniform1f(loc, hasColorTexture ? 1.0f : 0.0f);
        checkGLError("[GlMaterialBinder] bindShader: set hasColorTexture");
      }
    }

    {
      auto loc = params_.shader->location(Shaders::RENDERER_HAS_DEPTH_TEXTURE);
      if (loc >= 0) {
        auto hasDepthTexture = params_.material->depthTexture() != nullptr;

        glUniform1f(loc, hasDepthTexture ? 1.0f : 0.0f);
        checkGLError("[GlMaterialBinder] bindShader: set hasDepthTexture");
      }
    }

    // Check if the shader supports Shaders::RENDERER_MV as a uniform, and if so, pass the object's
    // mv matrix to the shader.
    {
      auto loc = params_.shader->location(Shaders::RENDERER_MV);
      if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, params_.mv->data().data());
        checkGLError("[GlMaterialBinder] bindShader: set mv");
      }
    }

    // Check if the shader supports Shaders::RENDERER_P as a uniform, and if so, pass the object's
    // p matrix to the shader.
    {
      auto loc = params_.shader->location(Shaders::RENDERER_P);
      if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, params_.p->data().data());
        checkGLError("[GlMaterialBinder] bindShader: set p");
      }
    }

    // Check if the shader supports Shaders::RENDERER_MVP as a uniform, and if so, pass the object's
    // mvp matrix to the shader.
    {
      auto loc = params_.shader->location(Shaders::RENDERER_MVP);
      if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, params_.mvp->data().data());
        checkGLError("[GlMaterialBinder] bindShader: set mvp");
      }
    }

    // Check if the shader supports Shaders::RENDERER_OPACITY as a uniform, and if so, pass the
    // material's opacity to the shader.
    {
      auto loc = params_.shader->location(Shaders::RENDERER_OPACITY);
      if (loc >= 0) {
        glUniform1f(loc, params_.material->opacity());
        checkGLError("[GlMaterialBinder] bindShader: set opacity");
      }
    }

    // Check if the shader supports Shaders::NORMAL_MATRIX as a uniform, and if so, pass the
    // object's normal matrix to the shader.
    {
      auto loc = params_.shader->location(Shaders::NORMAL_MATRIX);
      if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, normalMatrix(*params_.mv).data().data());
        checkGLError("[GlMaterialBinder] bindShader: set normalMatrix");
      }
    }

    // Check if the shader supports Shaders::AMBIENT_LIGHT as a uniform, and if so, pass the
    // object's ambient light to the shader.
    {
      auto loc = params_.shader->location(Shaders::AMBIENT_LIGHT);
      if (loc >= 0) {
        glUniform3fv(loc, 1, params_.ambientLight->data().data());
        checkGLError("[GlMaterialBinder] bindShader: set ambientLight");
      }
    }

    // Check if the shader supports Shaders::POINT_LIGHTS as a uniform, and if so, pass the object's
    // point lights to the shader.
    {
      // POINT_LIGHTS is an array of structs - have to check for a specific member else will get -1.
      auto loc = params_.shader->location(Shaders::POINT_LIGHTS, 0, "posInView");
      if (loc >= 0) {
        constexpr float zero[3] = {0.f, 0.f, 0.f};
        constexpr float white[3] = {1.0f, 1.f, 1.f};
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
          bool used = i < params_.pointLights->size();
          {
            auto loc = params_.shader->location(Shaders::POINT_LIGHTS, i, "posInView");
            glUniform3fv(loc, 1, used ? params_.pointLights->at(i).posInView.data().data() : zero);
          }
          {
            auto loc = params_.shader->location(Shaders::POINT_LIGHTS, i, "color");
            glUniform3fv(loc, 1, used ? params_.pointLights->at(i).color.data().data() : white);
          }
          {
            auto loc = params_.shader->location(Shaders::POINT_LIGHTS, i, "intensity");
            glUniform1f(loc, used ? params_.pointLights->at(i).intensity : 0.f);
          }
        }
        checkGLError("[GlMaterialBinder] bindShader: set pointLights");
      }
    }

    // Check if the shader supports Shaders::DIRECTIONAL_LIGHTS as a uniform, and if so, pass the
    // object's directional lights to the shader.
    {
      // DIRECTIONAL_LIGHTS is an array of structs - have to check for a specific member else will
      // get -1.
      auto loc = params_.shader->location(Shaders::DIRECTIONAL_LIGHTS, 0, "dirInView");
      if (loc >= 0) {
        constexpr float zero[3] = {0.f, 0.f, 0.f};
        constexpr float white[3] = {1.0f, 1.f, 1.f};
        for (int i = 0; i < NUM_DIR_LIGHTS; ++i) {
          bool used = i < params_.directionalLights->size();
          {
            auto loc = params_.shader->location(Shaders::DIRECTIONAL_LIGHTS, i, "dirInView");
            glUniform3fv(
              loc, 1, used ? params_.directionalLights->at(i).dirInView.data().data() : zero);
          }
          {
            auto loc = params_.shader->location(Shaders::DIRECTIONAL_LIGHTS, i, "color");
            glUniform3fv(
              loc, 1, used ? params_.directionalLights->at(i).color.data().data() : white);
          }
          {
            auto loc = params_.shader->location(Shaders::DIRECTIONAL_LIGHTS, i, "intensity");
            glUniform1f(loc, used ? params_.directionalLights->at(i).intensity : 0.f);
          }
        }
        checkGLError("[GlMaterialBinder] bindShader: set directionalLights");
      }
    }

    // Check if the shader supports Shaders::NEAR and FAR as a uniform, and if so, pass the value
    // used for perspective camera
    {
      auto nearLoc = params_.shader->location(Shaders::CAMERA_NEAR);
      if (nearLoc >= 0) {
        glUniform1f(nearLoc, CAM_PROJECTION_NEAR_CLIP);
        checkGLError("[GlMaterialBinder] bindShader: set camera near");
      }
      auto farLoc = params_.shader->location(Shaders::CAMERA_FAR);
      if (farLoc >= 0) {
        glUniform1f(farLoc, CAM_PROJECTION_FAR_CLIP);
        checkGLError("[GlMaterialBinder] bindShader: set camera far");
      }
    }

    for (const auto &e : params_.material->int1()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform1iv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 1iv");
      }
    }

    for (const auto &e : params_.material->int2()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform2iv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 2iv");
      }
    }

    for (const auto &e : params_.material->int3()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform3iv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 3iv");
      }
    }

    for (const auto &e : params_.material->int4()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform4iv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 4iv");
      }
    }

    for (const auto &e : params_.material->float1()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform1fv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 1fv");
      }
    }

    for (const auto &e : params_.material->float2()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform2fv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 2fv");
      }
    }

    for (const auto &e : params_.material->float3()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform3fv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 3fv");
      }
    }

    for (const auto &e : params_.material->float4()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniform4fv(loc, 1, e.second.data());
        checkGLError("[GlMaterialBinder] bindShader: set 4fv");
      }
    }

    for (const auto &e : params_.material->matrix()) {
      auto loc = params_.shader->location(e.first);
      if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, e.second.data().data());
        checkGLError("[GlMaterialBinder] bindShader: set Matrix4fv");
      }
    }

    if (disableDepthMask) {
      glDepthMask(GL_TRUE);
    }
  }

  void bindTexture(Texture *t, int slot) {
    if (t->binder() == nullptr) {
      t->setBinder(std::make_unique<GlTextureBinder>());
    }
    // Update texture unit here so binder.update() won't unbind the previous texture unit
    glActiveTexture(GL_TEXTURE0 + slot);
    checkGLError("[GlTextureBinder] bindTexture: activeTexture");
    auto &binder = *dynamic_cast<GlTextureBinder *>(t->binder());
    binder.update(params_.sceneRoot, t);
    binder.bind(slot);
  }

  void bindTextures() {
    const auto &textureNames = params_.material->textureNames();
    if (!textureNames.empty()) {
      // Bind texture names to slots in the order they were added to the material.
      for (int i = 0; i < textureNames.size(); i++) {
        bindTexture(params_.material->texture(textureNames[i]), i);
      }
    } else {
      // Bind available textures in a predictable order. Each present texture on the material will
      // be bound in the next active slot. Shaders that expect and use these textures must declare
      // their samplers in this order, in order to match them to the texture slot.
      //
      // Additionally when using multiple textures, make sure the textures uniforms are correctly
      // assigned to the correct texture unit.
      // To assign a texture to the correct unit, add the texture sampler as a shader uniform, and
      // set its uniform value to the correct unit.
      // (ex. glUniform1i(colorTextureLocation, 0), glUniform1i(depthTextureLocation, 1))
      // Setting the correct texture unit is not necessary if a shader only uses a colorTexture
      //
      // Currently supported textures, in order, are:
      //  - color
      //  - depth
      if (params_.material->colorTexture() != nullptr) {
        bindTexture(params_.material->colorTexture(), 0);
      }
      if (params_.material->depthTexture() != nullptr) {
        bindTexture(params_.material->depthTexture(), 1);
      }
    }
  }

  MaterialBindingParams params_;
};

// Specifies a shader that can be created on the GPU, without actualy creating it.
struct GlShaderDef {
  String vertexShaderCode;
  String fragmentShaderCode;
  Vector<String> vertexAttribs;
  Vector<Uniform> uniforms;
};

class GlRendererState : public RendererState {
public:
  // A raw pointer to the main framebuffer for the entire scene that is held in the main
  // GlSceneRenderer structure.  This allows result() to read from the correct framebuffer.
  GlFramebufferObject *framebuffer;

  TreeMap<String, GlShaderDef> shaderDefs;   // Shader definitions, not yet initialized in gpu.
  TreeMap<String, GlProgramObject> shaders;  // Shaders on GPU.
  Vector<String> foundShaderVertexAttribs;   // Vertex attributes in foundShader.

  // Gets the shader, initalizing from its shader def if needed. If there is no shader def, throws
  // out_of_bounds.
  GlProgramObject &shader(const String &name) {
    auto foundShader = shaders.find(name);
    if (foundShader == shaders.end()) {
      shaders[name] = {};
      foundShader = shaders.find(name);
      const auto &d = shaderDefs.at(name);
      foundShader->second.initialize(
        d.vertexShaderCode.c_str(), d.fragmentShaderCode.c_str(), d.vertexAttribs, d.uniforms);
    }
    foundShaderVertexAttribs = shaderDefs.at(name).vertexAttribs;
    return foundShader->second;
  }

  void registerShader(
    const String &name,
    const String &vertexShaderCode,
    const String &fragmentShaderCode,
    const Vector<String> &vertexAttribs,
    const Vector<Uniform> &uniforms) {
    GlShaderDef d = {vertexShaderCode, fragmentShaderCode};
    for (const auto &e : vertexAttribs) {
      d.vertexAttribs.push_back(e);
    }
    for (const auto &e : uniforms) {
      d.uniforms.push_back(e);
    }
    shaderDefs[name] = d;
  }

  void registerDefaultShaders() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Color Only
    {
      auto vert = embeddedColorOnlyVertCStr;
      auto frag = embeddedColorOnlyFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedColorOnlyWebgl2VertCStr;
        frag = embeddedColorOnlyWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::COLOR_ONLY,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::COLOR,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MVP}, {Shaders::RENDERER_OPACITY}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Physical
    {
      auto vert = embeddedPhysicalVertCStr;
      auto frag = embeddedPhysicalFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedPhysicalWebgl2VertCStr;
        frag = embeddedPhysicalWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::PHYSICAL,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::COLOR,
         Shaders::NORMAL,
         Shaders::UV,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_HAS_COLOR_TEXTURE},
         {Shaders::RENDERER_MV},
         {Shaders::RENDERER_MVP},
         {Shaders::RENDERER_OPACITY},
         {Shaders::NORMAL_MATRIX},
         {Shaders::AMBIENT_LIGHT},
         {Shaders::POINT_LIGHTS, {"posInView", "color", "intensity"}, NUM_POINT_LIGHTS},
         {Shaders::DIRECTIONAL_LIGHTS, {"dirInView", "color", "intensity"}, NUM_DIR_LIGHTS},
         {Shaders::RENDERER_HAS_DEPTH_TEXTURE},
         {Shaders::DEPTH_TEXTURE},
         {Shaders::OCCLUDE_TOLERANCE}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Image
    {
      auto vert = embeddedImageVertCStr;
      auto frag = embeddedImageFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedImageWebgl2VertCStr;
        frag = embeddedImageWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::IMAGE,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::UV,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MVP}, {Shaders::RENDERER_OPACITY}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Point Cloud Color Only
    {
      auto vert = embeddedPointCloudColorOnlyVertCStr;
      auto frag = embeddedPointCloudColorOnlyFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedPointCloudColorOnlyWebgl2VertCStr;
        frag = embeddedPointCloudColorOnlyWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::POINT_CLOUD_COLOR_ONLY,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::COLOR,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MV},
         {Shaders::RENDERER_MVP},
         {Shaders::RENDERER_OPACITY},
         {Shaders::RENDERER_RAYS_TO_PIX},
         {Shaders::POINT_CLOUD_POINT_SIZE}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Point Cloud Physical
    {
      auto vert = embeddedPointCloudPhysicalVertCStr;
      auto frag = embeddedPointCloudPhysicalFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedPointCloudPhysicalWebgl2VertCStr;
        frag = embeddedPointCloudPhysicalWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::POINT_CLOUD_PHYSICAL,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::COLOR,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MV},
         {Shaders::RENDERER_MVP},
         {Shaders::RENDERER_OPACITY},
         {Shaders::RENDERER_RAYS_TO_PIX},
         {Shaders::POINT_CLOUD_POINT_SIZE},
         {Shaders::AMBIENT_LIGHT},
         {Shaders::POINT_LIGHTS, {"posInView", "color", "intensity"}, NUM_POINT_LIGHTS},
         {Shaders::DIRECTIONAL_LIGHTS, {"dirInView", "color", "intensity"}, NUM_DIR_LIGHTS},
         {Shaders::RENDERER_HAS_DEPTH_TEXTURE},
         {Shaders::DEPTH_TEXTURE},
         {Shaders::OCCLUDE_TOLERANCE}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Depth
    {
      auto vert = embeddedDepthVertCStr;
      auto frag = embeddedDepthFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedDepthWebgl2VertCStr;
        frag = embeddedDepthWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::DEPTH,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MVP}, {Shaders::CAMERA_NEAR}, {Shaders::CAMERA_FAR}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Cubemap
    {
      auto vert = embeddedCubemapVertCStr;
      auto frag = embeddedCubemapFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedCubemapWebgl2VertCStr;
        frag = embeddedCubemapWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::CUBEMAP,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE},
        {{Shaders::RENDERER_MVP}, {Shaders::RENDERER_OPACITY}});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Splat (Attributes variant)
    {
      auto vert = embeddedSplatAttributesVertCStr;
      auto frag = embeddedSplatFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        vert = embeddedSplatAttributesWebgl2VertCStr;
        frag = embeddedSplatWebgl2FragCStr;
      }
#endif

      registerShader(
        Shaders::SPLAT_ATTRIBUTES,
        vert,
        frag,
        {Shaders::POSITION,
         Shaders::INSTANCE_POSITION,
         Shaders::INSTANCE_ROTATION,
         Shaders::INSTANCE_SCALE,
         Shaders::INSTANCE_COLOR},
        {
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_P},
          {Shaders::RENDERER_RAYS_TO_PIX},
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Splat (Texture variant)
    {
      auto vert = embeddedSplatTextureVertCStr;
      auto frag = embeddedSplatFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        frag = embeddedSplatWebgl2FragCStr;
      } else {
        C8Log("WARNING: Texture shader not supported on WebGL1.");
      }
#endif

      registerShader(
        Shaders::SPLAT_TEXTURE,
        vert,
        frag,
        {Shaders::POSITION, Shaders::INSTANCE_ID},
        {
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_P},
          {Shaders::RENDERER_RAYS_TO_PIX},
          {Shaders::RENDERER_ANTIALIASED},
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Splat (Texture stacked variant)
    {
      auto vert = embeddedSplatTextureStackedVertCStr;
      auto frag = embeddedSplatFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        frag = embeddedSplatWebgl2FragCStr;
      } else {
        C8Log("WARNING: Texture shader not supported on WebGL1.");
      }
#endif

      registerShader(
        Shaders::SPLAT_TEXTURE_STACKED,
        vert,
        frag,
        {Shaders::POSITION},
        {
          {Shaders::RENDERER_SPLAT_DATA_TEX},
          {Shaders::RENDERER_SPLAT_IDS_TEX},
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_P},
          {Shaders::RENDERER_RAYS_TO_PIX},
          {Shaders::RENDERER_ANTIALIASED},
          {Shaders::RENDERER_STACKED_INSTANCE_COUNT},
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Splat (Multitexture SH stage)
    {
      auto vert = embeddedSplatMultitexNoopVertCStr;
      auto frag = embeddedSplatMultitexShFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        frag = embeddedSplatMultitexShFragCStr;
      } else {
        C8Log("WARNING: Texture shader not supported on WebGL1.");
      }
#endif

      registerShader(
        Shaders::SPLAT_MULTITEX_SH,
        vert,
        frag,
        {Shaders::POSITION, Shaders::UV},
        {
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_SPLAT_POSITIONCOLOR_TEX},
          {Shaders::RENDERER_SPLAT_SHRG_TEX},
          {Shaders::RENDERER_SPLAT_SHB_TEX},
        });
    }

    // Splat (Multitex final stage)
    {
      auto vert = embeddedSplatMultitexFinalVertCStr;
      auto frag = embeddedSplatFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        frag = embeddedSplatWebgl2FragCStr;
      } else {
        C8Log("WARNING: Texture shader not supported on WebGL1.");
      }
#endif

      registerShader(
        Shaders::SPLAT_MULTITEX_FINAL,
        vert,
        frag,
        {Shaders::POSITION},
        {
          {Shaders::RENDERER_SPLAT_IDS_TEX},
          {Shaders::RENDERER_SPLAT_POSITIONCOLOR_TEX},
          {Shaders::RENDERER_SPLAT_ROTATIONSCALE_TEX},
          {Shaders::RENDERER_SPLAT_SHCOLOR_TEX},
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_P},
          {Shaders::RENDERER_RAYS_TO_PIX},
          {Shaders::RENDERER_ANTIALIASED},
          {Shaders::RENDERER_STACKED_INSTANCE_COUNT},
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Splat (Sorted Texture variant)
    {
      auto vert = embeddedSplatSortedTextureVertCStr;
      auto frag = embeddedSplatFragCStr;
#ifdef __EMSCRIPTEN__
      if (isWebGL2()) {
        frag = embeddedSplatWebgl2FragCStr;
      } else {
        C8Log("WARNING: Texture shader not supported on WebGL1.");
      }
#endif

      registerShader(
        Shaders::SPLAT_SORTED_TEXTURE,
        vert,
        frag,
        {Shaders::POSITION, "positionColor", "scaleRotation", "shRG", "shB"},
        {
          {Shaders::RENDERER_MV},
          {Shaders::RENDERER_P},
          {Shaders::RENDERER_RAYS_TO_PIX},
          {Shaders::RENDERER_ANTIALIASED},
        });
    }
  }
};  // namespace c8

Renderer::Renderer() {
  initExtensions();
  state_.reset(new GlRendererState());
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  state.registerDefaultShaders();
  checkGLError("[Renderer] initialize: material");

#if !C8_OPENGL_ES
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  checkGLError("[Renderer] initialize: blend func");
}

void Renderer::registerShader(
  const String &name,
  const String &vertexShaderCode,
  const String &fragmentShaderCode,
  const Vector<String> &vertexAttribs,
  const Vector<Uniform> &uniforms) {
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  state.registerShader(name, vertexShaderCode, fragmentShaderCode, vertexAttribs, uniforms);
}

RGBA8888PlanePixelBuffer Renderer::result() const {
  ScopeTimer t("renderer-result");
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  auto pixbuf = readFramebufferRGBA8888PixelBuffer(*state.framebuffer);
  RGBA8888PlanePixelBuffer flipbuf(pixbuf.pixels().rows(), pixbuf.pixels().cols());
  auto flippix = flipbuf.pixels();
  flipVertical(pixbuf.pixels(), &flippix);
  return flipbuf;
}

std::pair<RGBA8888PlanePixelBuffer, RGBA8888PlanePixelBuffer> Renderer::allocateForResult() const {
  const auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  auto t = state.framebuffer->tex().tex();
  return std::make_pair<RGBA8888PlanePixelBuffer, RGBA8888PlanePixelBuffer>(
    {t.height(), t.width()}, {t.height(), t.width()});
}

void Renderer::result(RGBA8888PlanePixels dest) const {
  ScopeTimer t("renderer-result");
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  readFramebufferRGBA8888Pixels(*state.framebuffer, dest);
}

void Renderer::result(RGBA8888PlanePixels scratchFlipBuf, RGBA8888PlanePixels dest) const {
  ScopeTimer t("renderer-result");
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());
  readFramebufferRGBA8888Pixels(*state.framebuffer, scratchFlipBuf);
  flipVertical(scratchFlipBuf, &dest);
}

void getObjects(
  Object8 &obj,
  Vector<Renderable *> *opaque,
  Vector<Renderable *> *transparent,
  Vector<Light *> *lights) {
  if (!obj.enabled()) {
    return;
  }

  if (obj.is<Renderable>()) {
    if (obj.as<Renderable>().material().transparent()) {
      transparent->push_back(&obj.as<Renderable>());
    } else {
      opaque->push_back(&obj.as<Renderable>());
    }
  } else if (obj.is<Light>()) {
    lights->push_back(&obj.as<Light>());
  }

  for (auto *child : obj.mutableChildren()) {
    // We don't want to get objects from within sub-scenes.
    if (child->is<Scene>()) {
      continue;
    }
    getObjects(*child, opaque, transparent, lights);
  }
}

// Returns a list of subscenes given the root Scene node
void getSubscenes(Object8 &root, Vector<Scene *> *subscenes) {
  for (auto *child : root.mutableChildren()) {
    if (child->is<Scene>()) {
      subscenes->push_back(&child->as<Scene>());
    }
    getSubscenes(*child, subscenes);
  }
}

// Painter Sort
Vector<Renderable *> sortRenderables(
  const Vector<Renderable *> &objs,
  const HMatrix &view,
  const HMatrix &projection,
  bool opaqueOrder) {
  Vector<std::pair<float, Renderable *>> renderableDistances;
  renderableDistances.reserve(objs.size());
  for (auto *o : objs) {
    // Get the bounding box in view space, and then sort by bounding box center. Converting
    // perspective points to clip space can lead to artifacts for points outside the frustum. For
    // example, points at negative depth in perspective space project to positive z-values (>1) in
    // clip space. This means a plane that has its corners both in front of and behind the camera
    // could have its center behind the far corners in clip space.
    auto toViewSpace = o->ignoreProjection() ? projection.inv() * o->world() : view * o->world();
    auto bb = o->geometry().boundingBox().transform(toViewSpace);
    // TODO(nb): frustum culling in view space: Compute the corners in clip space and multiply by
    // view.inv(). Then compute the 6 planes of the frustum from triples of these 8 points, and
    // check at all 8 points of the bounding box are on the outside of each plane.
    renderableDistances.push_back({bb.center().z(), o});
  }

  // opaque order sort.
  auto less = [](const std::pair<float, Renderable *> &a, const std::pair<float, Renderable *> &b) {
    if (a.first == b.first) {
      return a.second->id() < b.second->id();
    }
    return a.first < b.first;
  };

  std::sort(
    renderableDistances.begin(),
    renderableDistances.end(),
    [opaqueOrder, less](const auto &a, const auto &b) {
      return opaqueOrder ? less(a, b) : less(b, a);
    });

  return map<std::pair<float, Renderable *>, Renderable *>(
    renderableDistances, [](const auto &v) { return v.second; });
}

void renderObject(
  Renderable *object,
  Scene *sceneRoot,
  GlRendererState &state,
  float raysToPix,
  const HMatrix &mv,
  const HMatrix &p,
  const HMatrix &mvp,
  const Vector<PointLightData> &pointLights,
  const Vector<DirectionalLightData> &directionalLights,
  const HVector3 &ambientLight) {
  if (object->material().binder() == nullptr) {
    object->material().setBinder(std::make_unique<GlMaterialBinder>());
  }

  if (object->geometry().renderer() == nullptr) {
    object->geometry().setRenderer(std::make_unique<GlGeometryRenderer>());
  }

  switch (object->material().blendFunction()) {
    case BlendFunction::NORMAL:
      glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case BlendFunction::ADDITIVE:
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      break;
    default:
      glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  }

  auto &binder = *dynamic_cast<GlMaterialBinder *>(object->material().binder());
  binder.update(
    {object,
     sceneRoot,
     &object->material(),
     &state.shader(object->material().shader()),
     raysToPix,
     &mv,
     &p,
     &mvp,
     &pointLights,
     &directionalLights,
     &ambientLight});
  binder.bind();

  auto &renderer = *dynamic_cast<GlGeometryRenderer *>(object->geometry().renderer());
  renderer.initializeVertexArray(state.foundShaderVertexAttribs);
  renderer.update(object->geometry());
  renderer.render();
}

// GlSceneRenderer holds the OpenGL gpu resources of the scene.  The renderer sets this on the scene
// so that it shares lifetime with the scene, gets destroyed with the scene, and are generally
// managed together.
class GlSceneRenderer : public SceneRenderer {
public:
  void render(const RenderSpec &renderSpec) override {
    // Bind the framebuffer so that drawing commands go to it.
    auto &framebuffer = renderSpecFBO_[renderSpec.name];
    if (renderSpec.outputFramebuffer != 0) {
      glBindFramebuffer(GL_FRAMEBUFFER, renderSpec.outputFramebuffer);
    } else {
      framebuffer.bind();
    }
    checkGLError("[GlSceneRenderer] render: framebuffer bind");

    auto rv = renderSpec.viewport;
    if (!rv.w || !rv.h) {
      rv.w = framebuffer.tex().width();
      rv.h = framebuffer.tex().height();
    }

    glViewport(rv.x, rv.y, rv.w, rv.h);
    checkGLError("[GlSceneRenderer] render: viewport");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CW);

    // World -> clip space transform.
    auto &camera = !renderSpec.cameraName.empty() ? scene_->find<Camera>(renderSpec.cameraName)
                                                  : scene_->find<Camera>();
    auto view = camera.world().inv();
    auto viewProjection = camera.projection() * view;
    auto raysToPix = raysToPixForProjection(camera.projection(), rv.w);

    // Used in pointcloud vertex shaders to determine whether to ignore projection
    // Set to 1.0f if camera is an orthographic, equivalent to ignoring the projection
    if (camera.projection()(3, 2) == 0.f) {
      raysToPix = 1.0f;
    }

    Vector<Renderable *> opaqueRenderList;
    Vector<Renderable *> transparentRenderList;
    Vector<Light *> lights;
    getObjects(*scene_, &opaqueRenderList, &transparentRenderList, &lights);

    // Lights.
    float r = 0.f, g = 0.f, b = 0.f;
    Vector<PointLightData> pointLights;
    Vector<DirectionalLightData> directionalLights;
    for (auto *light : lights) {
      switch (light->kind()) {
        case Light::LightKind::AMBIENT:
          r += light->color().r() * light->intensity();
          g += light->color().g() * light->intensity();
          b += light->color().b() * light->intensity();
          break;

        case Light::LightKind::POINT:
          pointLights.push_back(
            {translation(view * light->world()),
             HVector3(
               light->color().r() / 255.f, light->color().g() / 255.f, light->color().b() / 255.f),
             light->intensity()});
          break;

        case Light::LightKind::DIRECTIONAL: {
          directionalLights.push_back(
            {view * (light->world() * HVector3(0.f, 0.f, 1.f)),
             HVector3(
               light->color().r() / 255.f, light->color().g() / 255.f, light->color().b() / 255.f),
             light->intensity()});
          break;
        }
        default:
          break;
      }
    }
    HVector3 ambientLight =
      lights.size() != 0 ? HVector3(r, g, b) * (1.f / 256.f) : HVector3(1.f, 1.f, 1.f);

    // TODO:
    // Render shadow map

    // Render background
    if (
      renderSpec.clearBuffers == BufferClear::UNSPECIFIED
      || renderSpec.clearBuffers == BufferClear::ALL) {
      glClearColor(
        renderSpec.clearColor.r() / 255.f,
        renderSpec.clearColor.g() / 255.f,
        renderSpec.clearColor.b() / 255.f,
        renderSpec.clearColor.a() / 255.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      checkGLError("[GlSceneRenderer] render: clear");
    }

    // Render opaque objects
    if (!opaqueRenderList.empty()) {
      glDisable(GL_BLEND);
    }

    // Render opaque objects from front to back. This minimizes the number of fragments that need
    // to be redrawn because fragments drawn behind other fragments are discarded by the gpu.
    for (auto *object : sortRenderables(opaqueRenderList, view, camera.projection(), true)) {
      renderObject(
        object,
        rootScene_,
        *state_,
        object->ignoreProjection() ? 1.0f : raysToPix,
        object->ignoreProjection() ? object->world() : view * object->world(),
        object->ignoreProjection() ? HMatrixGen::i() : camera.projection(),
        object->ignoreProjection() ? object->world() : viewProjection * object->world(),
        pointLights,
        directionalLights,
        ambientLight);
    }
    if (!transparentRenderList.empty()) {
      glEnable(GL_BLEND);
    }
    // Render transparent objects from back to front. A transparent object behind a transparent
    // object needs to be drawn so its color can be added to the front transparent object. If we
    // drew the front object first, the back object's fragment would be discarded by depth test,
    // and so the rear object would disappear behind the front object even though the front object
    // is transparent.
    for (auto *object : sortRenderables(transparentRenderList, view, camera.projection(), false)) {
      renderObject(
        object,
        rootScene_,
        *state_,
        object->ignoreProjection() ? 1.0f : raysToPix,
        object->ignoreProjection() ? object->world() : view * object->world(),
        object->ignoreProjection() ? HMatrixGen::i() : camera.projection(),
        object->ignoreProjection() ? object->world() : viewProjection * object->world(),
        pointLights,
        directionalLights,
        ambientLight);
    }

    if (renderSpec.blitToScreen) {
#ifdef JAVASCRIPT
      if (glBlitFramebuffer == nullptr) {
        // TODO: Implement this with quad drawing draw elements implementation
        C8Log(
          "[renderer] Screen output is not yet implemented with "
          "WEBGL2_BACKWARDS_COMPATIBILITY_EMULATION, recompile with USE_WEBGL2 for support.");
        return;
      }
#endif

      Viewport v =
        renderSpec.screenViewport.w && renderSpec.screenViewport.h ? renderSpec.screenViewport : rv;

      // glBlitFramebuffer implementation.
      int interpolation = rv.w == v.w && rv.h == v.h ? GL_NEAREST : GL_LINEAR;
      glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        renderSpec.outputFramebuffer == 0 ? framebuffer.id() : renderSpec.outputFramebuffer);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(
        rv.x, rv.y, rv.w, rv.h, v.x, v.y, v.w, v.h, GL_COLOR_BUFFER_BIT, interpolation);

      // Unbind read buffer; write buffer (screen, id 0) is effectively already unbound.
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }
  }

  // Sets the framebuffer for each renderSpec
  void update(GlRendererState &state, Scene &rootScene, Scene &scene) {
    state_ = &state;
    rootScene_ = &rootScene;
    scene_ = &scene;

    for (auto &renderSpec : scene.renderSpecs()) {
      auto fbo = renderSpecFBO_.find(renderSpec.name);
      if (fbo != renderSpecFBO_.end()) {
        if (
          fbo->second.tex().width() == renderSpec.resolutionWidth
          && fbo->second.tex().height() == renderSpec.resolutionHeight) {
          // We already have a corresponding fbo of the correct size for this renderSpec, so we will
          // re-use it.
          continue;
        }
      }

      // Create a new framebuffer
      renderSpecFBO_[renderSpec.name] =
        makeLinearRGBA8888Framebuffer(renderSpec.resolutionWidth, renderSpec.resolutionHeight);
      renderSpecFBO_[renderSpec.name].attachRenderbuffer(
        makeDepthRenderbuffer(renderSpec.resolutionWidth, renderSpec.resolutionHeight),
        GL_DEPTH_ATTACHMENT);
      renderSpecFBO_[renderSpec.name].bind();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      renderSpecFBO_[renderSpec.name].unbind();
      checkGLError("[GlSceneRenderer] update");
    }
  }

  // Returns the Gluint for the color texture attached to the framebuffer for this renderSpec
  int32_t nativeTextureId(const String &renderSpecName) override {
    return specFramebuffer(renderSpecName).tex().id();
  }

  // Returns the framebuffer for the corresponding renderspec.  If the renderspec doesn't exist
  // it will throw an OutOfRange error.
  GlFramebufferObject &specFramebuffer(const String &renderSpecName) {
    // Confirm the renderSpecName exists before accidentally creating a new one
    if (renderSpecFBO_.find(renderSpecName) == renderSpecFBO_.end()) {
      const String errorMsg = format(
        "No render spec with name '%s' in scene '%s'",
        renderSpecName.c_str(),
        scene_->name().c_str());
      C8_THROW_OUT_OF_RANGE(errorMsg);
    }
    return renderSpecFBO_[renderSpecName];
  }

private:
  // each render spec will have its own offscreen framebuffer
  TreeMap<String, GlFramebufferObject> renderSpecFBO_;
  GlRendererState *state_;

  // Store a reference to the root scene
  Scene *rootScene_;
  // This is the corresponding scene/subscene.  It's not necessarily the root scene.
  Scene *scene_;
};

// types required for dfs traversal of renderspecs
struct TreeTraversalStatus {
  bool visiting = false;  // true when a scene node has a dfs started on it.
  bool visited = false;   // true when a scene node has finished a dfs through it.
};

SceneRenderSpecPair getSceneRenderSpecPair(
  const SceneMaterialSpec &sceneMatSpec,
  const TreeMap<String, Scene *> &nameToScene,
  const TreeMap<SceneRenderSpecNamePair, const RenderSpec *> &nameToRenderSpec) {
  auto relatedSceneFound = nameToScene.find(sceneMatSpec.subsceneName);
  if (relatedSceneFound == nameToScene.end()) {
    // We cannot find this subscene via name. Also, subscene names have to be unique.
    C8Log("[renderer] Subscene name %s cannot be found", sceneMatSpec.subsceneName.c_str());
    return {nullptr, nullptr};
  }
  auto relatedScene = relatedSceneFound->second;
  auto renderSpec = nameToRenderSpec.find({relatedScene, sceneMatSpec.renderSpecName});
  if (renderSpec == nameToRenderSpec.end()) {
    // We cannot find this subscene's renderspec via name.
    C8Log(
      "[renderer] Subscene %s renderSpec %s cannot be found",
      sceneMatSpec.subsceneName.c_str(),
      sceneMatSpec.renderSpecName.c_str());
    return {relatedScene, nullptr};
  }
  return {relatedScene, renderSpec->second};
}

// Get all the SceneRenderSpec pair that this node depends on via materials
// Will skip over Scene nodes
TreeSet<SceneRenderSpecPair> getDependentRenderSpec(
  const Object8 &node,
  const TreeMap<String, Scene *> &nameToScene,
  const TreeMap<SceneRenderSpecNamePair, const RenderSpec *> &nameToRenderSpec) {
  TreeSet<SceneRenderSpecPair> sceneRenderSpecPairs;
  for (const auto &child : node.children()) {
    if (child->is<Scene>()) {
      continue;
    }
    auto spec = getDependentRenderSpec(*child, nameToScene, nameToRenderSpec);
    sceneRenderSpecPairs.insert(spec.begin(), spec.end());
  }

  if (node.is<Renderable>()) {
    for (SceneMaterialSpec &sceneMatSpec : node.as<Renderable>().material().sceneMaterialSpecs()) {
      auto sceneRenderSpecPair =
        getSceneRenderSpecPair(sceneMatSpec, nameToScene, nameToRenderSpec);
      if (sceneRenderSpecPair.second == nullptr) {
        // couldn't find this render spec
        C8Log(
          "[renderer] cannot find renderspec scene %s renderspec %s",
          sceneMatSpec.subsceneName.c_str(),
          sceneMatSpec.renderSpecName.c_str());
        return sceneRenderSpecPairs;
      }
      sceneRenderSpecPairs.insert(sceneRenderSpecPair);
    }
  }
  return sceneRenderSpecPairs;
}

// Topologically traverse the SceneRenderSpec dependency DAG
// return true if successful. Will append to renderingOrder the topological order.
// return false if there is there is a cycle. Will leave renderingOrder in a bad state.
bool dfsSceneRenderSpecVisit(
  const SceneRenderSpecPair &sceneRenderSpecPair,
  const TreeMap<SceneRenderSpecPair, TreeSet<SceneRenderSpecPair>> &deps,
  Vector<SceneRenderSpecPair> *renderingOrder,
  TreeMap<SceneRenderSpecPair, TreeTraversalStatus> *sceneRenderSpecTraversal) {
  auto &traversalStatus = (*sceneRenderSpecTraversal)[sceneRenderSpecPair];
  if (traversalStatus.visited) {
    // I have previously visited this node and all its children
    return true;
  }
  if (traversalStatus.visiting) {
    // I have already tried visiting this node from another part of the dfs
    // this indicates a loop in our dep
    return false;
  }
  traversalStatus.visiting = true;

  auto depRenderSpecPairs = deps.at(sceneRenderSpecPair);
  for (auto depRenderSpecPair : depRenderSpecPairs) {
    if (!dfsSceneRenderSpecVisit(
          depRenderSpecPair, deps, renderingOrder, sceneRenderSpecTraversal)) {
      return false;
    }
  }

  traversalStatus.visited = true;
  renderingOrder->push_back(sceneRenderSpecPair);
  return true;
}

// Recursively traverse all scenes from the rootScene to populate the two output maps
// @param nameToScene given a scene name, what is the pointer to the scene
// @param nameToRenderSpec given a scene pointer and a render spec name, what is the pointer to the
// render spec
void getSceneNamesAndRenderSpecs(
  Scene &rootScene,
  TreeMap<String, Scene *> *nameToScene,
  TreeMap<SceneRenderSpecNamePair, const RenderSpec *> *nameToRenderSpec) {
  std::function<void(Object8 &)> dfs = [&](Object8 &root) {
    for (auto *child : root.mutableChildren()) {
      if (child->is<Scene>()) {
        auto &childScene = child->as<Scene>();
        (*nameToScene)[child->name()] = &childScene;
        for (auto &renderSpec : childScene.renderSpecs()) {
          (*nameToRenderSpec)[{&childScene, renderSpec.name}] = &renderSpec;
        }
      }
      dfs(*child);
    }
  };
  dfs(rootScene);
}

// Start with a scene, find all scenes referenced in this scene's child objects' materials' render
// specs Recursively populate a dependency list of (scene, renderspec) -> set((scene, renderspec))
void getSceneRenderSpecPairDependencies(
  Scene &scene,
  const TreeMap<String, Scene *> &nameToScene,
  const TreeMap<SceneRenderSpecNamePair, const RenderSpec *> &nameToRenderSpec,
  HashSet<Scene *> *visitedScenes,
  TreeMap<SceneRenderSpecPair, TreeSet<SceneRenderSpecPair>> *dep) {
  if (visitedScenes->find(&scene) != visitedScenes->end()) {
    // cycle dependencies escape hatch
    return;
  }
  auto depRenderSpecs = getDependentRenderSpec(scene, nameToScene, nameToRenderSpec);

  // register all render specs of this scene to the dependent ones
  for (auto &renderSpec : scene.renderSpecs()) {
    SceneRenderSpecPair sceneRenderSpecPair{&scene, &renderSpec};
    (*dep)[sceneRenderSpecPair] = depRenderSpecs;
  }

  // Find all the scenes
  TreeSet<Scene *> depScenes;
  for (auto &depRenderSpec : depRenderSpecs) {
    depScenes.insert(depRenderSpec.first);
  }

  visitedScenes->insert(&scene);
  // Dfs into these scenes to find more deps
  for (auto *depScene : depScenes) {
    getSceneRenderSpecPairDependencies(
      *depScene, nameToScene, nameToRenderSpec, visitedScenes, dep);
  }
}

// Find the order of rendering (scene, renderspec) pair to render the rootScene
bool getSceneRenderingOrder(Scene &rootScene, Vector<SceneRenderSpecPair> *renderingOrder) {
  TreeMap<String, Scene *> nameToScene;
  TreeMap<SceneRenderSpecNamePair, const RenderSpec *> nameToRenderSpec;
  TreeMap<SceneRenderSpecPair, TreeSet<SceneRenderSpecPair>> dep;
  getSceneNamesAndRenderSpecs(rootScene, &nameToScene, &nameToRenderSpec);
  HashSet<Scene *> visitedScenes;
  getSceneRenderSpecPairDependencies(
    rootScene, nameToScene, nameToRenderSpec, &visitedScenes, &dep);

  renderingOrder->clear();
  TreeMap<SceneRenderSpecPair, TreeTraversalStatus> sceneRenderSpecTraversal;
  // We assume that the root scene is depended on and thus render all its render specs
  for (auto &rootRenderSpec : rootScene.renderSpecs()) {
    SceneRenderSpecPair sceneRenderSpec{&rootScene, &rootRenderSpec};
    bool okVisit =
      dfsSceneRenderSpecVisit(sceneRenderSpec, dep, renderingOrder, &sceneRenderSpecTraversal);
    if (!okVisit) {
      return false;
    }
  }
  return true;
}

// Render the scene (and all possible sub-scenes based on render specs)
RenderResult Renderer::render(Scene &rootScene) {
  auto &state = *dynamic_cast<GlRendererState *>(state_.get());

  Vector<SceneRenderSpecPair> renderingOrder;
  auto okRenderingOrder = getSceneRenderingOrder(rootScene, &renderingOrder);
  if (!okRenderingOrder) {
    C8Log("[renderer] Cannot find a rendering order for this scene %s", rootScene.name().c_str());
    return {};
  }

  for (auto rendering : renderingOrder) {
    // Get list of items that need to be rendered. Partition items by opaque and transparent.
    auto scene = rendering.first;
    if (scene->renderer() == nullptr) {
      scene->setRenderer(std::make_unique<GlSceneRenderer>());
    }
    auto &sceneRenderer = *dynamic_cast<GlSceneRenderer *>(scene->renderer());
    // scenes will have .update() calls once per renderspec that is rendered but .update() has a
    // dirty check so we are ok
    sceneRenderer.update(state, rootScene, *scene);
    sceneRenderer.render(*rendering.second);
  }

  glFlush();

  // Store in GlState a pointer to the main framebuffer to be used in result()
  auto &rootSceneRenderer = *dynamic_cast<GlSceneRenderer *>(rootScene.renderer());
  auto &sceneFBO = rootSceneRenderer.specFramebuffer("");
  state.framebuffer = &sceneFBO;

  return {
    .texId = static_cast<int>(sceneFBO.tex().id()),
    .bufferId = static_cast<int>(sceneFBO.id()),
    .width = sceneFBO.tex().width(),
    .height = sceneFBO.tex().height(),
  };
}

}  // namespace c8
