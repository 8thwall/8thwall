// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Renderer for cubemap with semantics results

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "semantics-cubemap-renderer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:hpoint",
    "//c8:hmatrix",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:intrinsics",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/geometry:raycaster",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels:pixels",
    "//c8/pixels/render:object8",
    "//c8/pixels/render:image-shader",
    "//c8/pixels/render:cubemap-shader",
    "//c8/stats:scope-timer",
    "//reality/engine/semantics:semantics-projection-shader",
    "//reality/engine/semantics:semantics-gaussian-linear-shader",
    "//reality/engine/semantics:semantics-sky-shader",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x8384f5ef);

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/raycaster.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/render/image-shader.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/semantics/semantics-cubemap-renderer.h"

#if C8_OPENGL_VERSION_2
#include "c8/pixels/opengl/glext.h"
#endif

#if JAVASCRIPT
#include <emscripten.h>
#endif

namespace c8 {

namespace {

// use downsized textures to do larger radius blur
constexpr int INTMD_DOWNSIZE_SCALE = 5;

// Gaussian blur parameters
constexpr float BLUR_RADIUS = 11;
constexpr float BLUR_SIGMA = 10.0;

void initCubeQuadGeometry(const MeshGeometry &quadGeometry, GlVertexArray *vao) {
  if (!vao) {
    return;
  }

  Vector<float> verts(3 * quadGeometry.points.size(), 0.0f);
  Vector<float> uvs(2 * quadGeometry.points.size(), 0.0f);
  for (size_t i = 0; i < quadGeometry.points.size(); ++i) {
    verts[3 * i + 0] = quadGeometry.points[i].x();
    verts[3 * i + 1] = quadGeometry.points[i].y();
    verts[3 * i + 2] = quadGeometry.points[i].z();
    uvs[2 * i + 0] = quadGeometry.uvs[i].x();
    uvs[2 * i + 1] = quadGeometry.uvs[i].y();
  }

  vao->addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    verts.size() * sizeof(float),
    verts.data(),
    GL_STATIC_DRAW);

  vao->addVertexBuffer(
    GlVertexAttrib::SLOT_2,
    2,
    GL_FLOAT,
    GL_FALSE,
    0,
    uvs.size() * sizeof(float),
    uvs.data(),
    GL_STATIC_DRAW);
}

HMatrix modelViewMatrix(SemanticsCubemapRenderer::CubemapFaces face) {
  HMatrix flipY = HMatrixGen::rotationR(0.0f, 0.0f, M_PI);
  HMatrix planeMat = HMatrixGen::i();
  switch (face) {
    case SemanticsCubemapRenderer::CubemapFaces::POSITIVE_X:
      planeMat = HMatrixGen::rotationR(0.0f, 0.5f * M_PI, 0.0f);
      break;

    case SemanticsCubemapRenderer::CubemapFaces::NEGATIVE_X:
      planeMat = HMatrixGen::rotationR(0.0f, -0.5f * M_PI, 0.0f);
      break;

    case SemanticsCubemapRenderer::CubemapFaces::POSITIVE_Y:
      flipY = HMatrixGen::i();
      planeMat = HMatrixGen::rotationR(-0.5f * M_PI, 0.0f, 0.0f);
      break;

    case SemanticsCubemapRenderer::CubemapFaces::NEGATIVE_Y:
      flipY = HMatrixGen::i();
      planeMat = HMatrixGen::rotationR(0.5f * M_PI, 0.0f, 0.0f);
      break;

    case SemanticsCubemapRenderer::CubemapFaces::POSITIVE_Z:
      planeMat = HMatrixGen::rotationR(0.0f, M_PI, 0.0f);
      break;

    case SemanticsCubemapRenderer::CubemapFaces::NEGATIVE_Z:
      break;

    default:
      break;
  }
  return flipY * planeMat;
}

}  // namespace

SemanticsCubemapRenderer::SemanticsCubemapRenderer(int texSize) : texSize_(texSize) {
  constexpr int outputW = 192;
  constexpr int outputH = 256;

  // define cube planes
  cubePlanes_ = {
    {1.0f, 0.0f, 0.0f, -1.0f},  // positive-x
    {1.0f, 0.0f, 0.00f, 1.0f},  // negative-x
    {0.0f, 1.0f, 0.0f, -1.0f},  // positive-y
    {0.0f, 1.0f, 0.00f, 1.0f},  // negative-y
    {0.0f, 0.0f, 1.0f, -1.0f},  // positive-z
    {0.0f, 0.0f, 1.00f, 1.0f},  // negative-z
  };

  cubePlaneTargets_ = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  };

  Vector<HPoint3> cubeVerts = {
    {-1.0f, 1.0f, 1.0f},    // 0
    {-1.0f, -1.0f, 1.0f},   // 1
    {1.0f, 1.0f, 1.0f},     // 2
    {1.0f, -1.0f, 1.0f},    // 3
    {-1.0f, 1.0f, -1.0f},   // 4
    {-1.0f, -1.0f, -1.0f},  // 5
    {1.0f, 1.0f, -1.0f},    // 6
    {1.0f, -1.0f, -1.0f},   // 7
  };

  MeshGeometry cubeFaceGeometries[6];

  // positive-x
  cubeFaceGeometries[0].points = {cubeVerts[3], cubeVerts[7], cubeVerts[2], cubeVerts[6]};
  cubeFaceGeometries[0].uvs = {{0.f, 1.f}, {1.f, 1.f}, {0.f, 0.f}, {1.f, 0.f}};

  // negative-x
  cubeFaceGeometries[1].points = {cubeVerts[1], cubeVerts[5], cubeVerts[0], cubeVerts[4]};
  cubeFaceGeometries[1].uvs = {{1.f, 1.f}, {0.f, 1.f}, {1.f, 0.f}, {0.f, 0.f}};

  // positive-y
  cubeFaceGeometries[2].points = {cubeVerts[0], cubeVerts[4], cubeVerts[2], cubeVerts[6]};
  cubeFaceGeometries[2].uvs = {{0.f, 1.f}, {0.f, 0.f}, {1.f, 1.f}, {1.f, 0.f}};

  // negative-y
  cubeFaceGeometries[3].points = {cubeVerts[1], cubeVerts[5], cubeVerts[3], cubeVerts[7]};
  cubeFaceGeometries[3].uvs = {{0.f, 1.f}, {0.f, 0.f}, {1.f, 1.f}, {1.f, 0.f}};

  // positive-z
  cubeFaceGeometries[4].points = {cubeVerts[0], cubeVerts[1], cubeVerts[2], cubeVerts[3]};
  cubeFaceGeometries[4].uvs = {{0.f, 0.f}, {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f}};

  // negative-z
  cubeFaceGeometries[5].points = {cubeVerts[4], cubeVerts[5], cubeVerts[6], cubeVerts[7]};
  cubeFaceGeometries[5].uvs = {{1.f, 0.f}, {1.f, 1.f}, {0.f, 0.f}, {0.f, 1.f}};

  for (int i = 0; i < 6; ++i) {
    cubeFaceGeometries[i].triangles = {{0, 2, 1}, {1, 2, 3}};
    auto &vP = cubeFaceGeometries[i].points;
    cubeFaceTriangles_[i] = {vP[0], vP[2], vP[1], vP[1], vP[2], vP[3]};
    cubeFaceBoundaryLines_[i].clear();
    cubeFaceBoundaryLines_[i].push_back(Line3(vP[0], vP[1]));
    cubeFaceBoundaryLines_[i].push_back(Line3(vP[1], vP[3]));
    cubeFaceBoundaryLines_[i].push_back(Line3(vP[3], vP[2]));
    cubeFaceBoundaryLines_[i].push_back(Line3(vP[2], vP[0]));
  }

  for (int i = 0; i < 6; ++i) {
    faceQuads_[i] = makeVertexArrayRect();
    initCubeQuadGeometry(cubeFaceGeometries[i], &faceQuads_[i]);
  }

  unitQuad_ = makeVertexArrayRect();

  flipYQuad_ = makeVertexArrayRect();
  flipYCWQuad_ = makeVertexArrayRect();
  flipYCCWQuad_ = makeVertexArrayRect();

  // flip the uv texture coordinates for final output rendering
  float flipUvs[][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  flipYQuad_.addVertexBuffer(
    GlVertexAttrib::SLOT_2, 2, GL_FLOAT, GL_FALSE, 0, sizeof(flipUvs), flipUvs, GL_STATIC_DRAW);

  float flipCwUvs[][2] = {{1, 0}, {0, 0}, {1, 1}, {0, 1}};
  flipYCWQuad_.addVertexBuffer(
    GlVertexAttrib::SLOT_2, 2, GL_FLOAT, GL_FALSE, 0, sizeof(flipCwUvs), flipCwUvs, GL_STATIC_DRAW);

  float flipCcwUvs[][2] = {{0, 1}, {1, 1}, {0, 0}, {1, 0}};
  flipYCCWQuad_.addVertexBuffer(
    GlVertexAttrib::SLOT_2,
    2,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(flipCcwUvs),
    flipCcwUvs,
    GL_STATIC_DRAW);

  frustumWorldRayDir_.reserve(4);

  // 90degree FOV camera for all 6 virtual cameras
  perspectiveProjection90DegreeMat_ = Intrinsics::perspectiveProjectionRightHanded(
    M_PI * 0.5f, 1.0f, CAM_PROJECTION_NEAR_CLIP, CAM_PROJECTION_FAR_CLIP);

  // GL resource initialization
  // the depth render buffer will be attached to all Framebuffers as it will not be written to, and
  // it's not going to affect the renderings.
  depthRenderBuffer_ = makeDepthRenderbuffer(texSize_, texSize_);
  checkGLError("[Semantics-cubemap] allocate depth render buffer");

  // THE cubemap texture for rendering the semantics mask every frame
  cubeTex_.initialize(GL_RGBA, texSize_, texSize_, GL_RGBA, GL_UNSIGNED_BYTE);

  // attach each face of the cubemap texture per attach-only framebuffer
  // all attach-only framebuffers share the same depth render buffer to make the attachment complete
  // depth mask is disabled therefore nothing will be written to the depth render buffer.
  for (int i = 0; i < 6; ++i) {
    cubeFaceFbs[i].attachColorTexture2D(GL_COLOR_ATTACHMENT0, cubePlaneTargets_[i], cubeTex_.id());
    cubeFaceFbs[i].attachRenderBuffer(
      GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer_.id());
  }

  // initialize 2 set of separate textures representing cubemap texture
  // attach the textures to framebuffers
  cubeTexSamplerIndex_ = 0;
  for (int k = 0; k < 2; ++k) {
    for (int i = 0; i < 6; ++i) {
      cubeFaces_[k][i] = makeLinearRGBA8888Texture2D(texSize_, texSize_);
      faceTex2dFbs_[k][i].attachColorTexture2D(
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cubeFaces_[k][i].id());
      faceTex2dFbs_[k][i].attachRenderBuffer(
        GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer_.id());
    }
  }

  // initialize shaders
  semProjectShader_.reset(new SemanticsProjectionShader());
  semProjectShader_->initialize();

  imageShader_.reset(new ImageShader());
  imageShader_->initialize();

  // initialize texture data, etc.
  RGBA8888PlanePixelBuffer buffer(texSize_, texSize_);
  std::memset(buffer.pixels().pixels(), 0, buffer.pixels().rows() * buffer.pixels().rowBytes());

  // solid black pixels for negative-Y ground face
  uint8_t *ptr = buffer.pixels().pixels();
  for (int r = 0; r < texSize_; ++r) {
    for (int c = 0; c < texSize_; ++c) {
      ptr[3] = 255;
      ptr += 4;
    }
  }
  cubeTex_.bind();
  cubeTex_.updateImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, buffer.pixels().pixels());
  cubeTex_.unbind();

  // solid yellow pixels for positive-Y top face
  ptr = buffer.pixels().pixels();
  for (int r = 0; r < texSize_; ++r) {
    for (int c = 0; c < texSize_; ++c) {
      ptr[0] = 255;
      ptr[1] = 255;
      ptr[2] = 0;
      ptr[3] = 255;
      ptr += 4;
    }
  }
  cubeTex_.bind();
  cubeTex_.updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, buffer.pixels().pixels());
  cubeTex_.unbind();

  for (int i = 0; i < 2; ++i) {
    cubeFaces_[i][2].bind();
    cubeFaces_[i][2].updateImage(buffer.pixels().pixels());
    cubeFaces_[i][2].unbind();
  }

  // make pixels above horizon line transparent white for all 4 directions
  std::memset(buffer.pixels().pixels(), 0, buffer.pixels().rows() * buffer.pixels().rowBytes());

  int halfRows = texSize_ / 2;
  ptr = buffer.pixels().pixels();
  for (int r = 0; r < halfRows; ++r) {
    for (int c = 0; c < texSize_; ++c) {
      ptr[0] = 255;
      ptr[1] = 255;
      ptr[2] = 0;
      ptr[3] = 255;
      ptr += 4;
    }
  }

  Vector<int> indices = {0, 1, 4, 5};
  for (int i = 0; i < 2; ++i) {
    for (auto k : indices) {
      cubeFaces_[i][k].bind();
      cubeFaces_[i][k].updateImage(buffer.pixels().pixels());
      cubeFaces_[i][k].unbind();
    }
  }

  // raw cube rendering
  constexpr float vertexData[][3] = {
    {10.0f, 10.0f, 10.0f},
    {-10.0f, 10.0f, 10.0f},
    {10.0f, -10.0f, 10.0f},
    {-10.0f, -10.0f, 10.0f},
    {10.0f, 10.0f, -10.0f},
    {-10.0f, 10.0f, -10.0f},
    {10.0f, -10.0f, -10.0f},
    {-10.0f, -10.0f, -10.0f},
  };

  constexpr uint16_t indexData[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 6, 7, 5,  // z
    0, 2, 4, 2, 6, 4, 1, 5, 3, 5, 7, 3,  // x
    0, 4, 5, 0, 5, 1, 2, 3, 6, 3, 7, 6,  // y
  };

  // create the cube vertex array
  outCube_.reset(new GlVertexArray());

  // Create the vertex array.
  outCube_->setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexData), indexData, GL_STATIC_DRAW);
  outCube_->addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexData),
    vertexData,
    GL_STATIC_DRAW);

  cubemapShader_.reset(new CubemapShader());
  cubemapShader_->initialize();

  gaussianLinearShader_.reset(new SemanticsGaussianLinearShader());
  gaussianLinearShader_->initialize();
  gaussianLinearShader_->setGaussianLinearKernels(BLUR_RADIUS, BLUR_SIGMA);

  semSkyShader_.reset(new SemanticsSkyShader());
  semSkyShader_->initialize();

  semSkyRenderBuffer_ = makeDepthRenderbuffer(outputW, outputH);

  semSkyFb_.reset(new GlAttachOnlyFramebuffer());
  semSkyFb_->attachRenderBuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, semSkyRenderBuffer_.id());
}

void SemanticsCubemapRenderer::setProjectionIntrinsics(c8_PixelPinholeCameraModel k) {
  // compute local frustum corners
  frustumLocalCorners_.clear();

  // Find the corners of the camera frustum at distance 1 from the camera.
  float w = static_cast<float>(k.pixelsWidth - 1);
  float h = static_cast<float>(k.pixelsHeight - 1);
  Vector<HPoint3> pixCorners = {
    {0.0f, 0.0f, 1.0f},
    {0.0f, h, 1.0f},
    {w, 0.0f, 1.0f},
    {w, h, 1.0f},
  };
  frustumLocalCorners_ = HMatrixGen::intrinsic(k).inv() * pixCorners;

  projectCamera_ = ObGen::perspectiveCamera(k, k.pixelsWidth, k.pixelsHeight);
}

void SemanticsCubemapRenderer::updateSrcTexture(ConstRGBA8888PlanePixels srcPixels) {
  // check if need to make a new texture
  if (srcTex_.width() != srcPixels.cols() || srcTex_.height() != srcPixels.rows()) {
    // re-allocate srcTex_ under current GL context
    srcTex_ = makeLinearRGBA8888Texture2D(srcPixels.cols(), srcPixels.rows());
    srcTexAllocated_ = true;
  }

  // Upload the CPU image to the GPU texture.
  srcTex_.bind();
  srcTex_.updateImage(srcPixels.pixels());
  srcTex_.unbind();
}

void SemanticsCubemapRenderer::projectImageToFace(
  CubemapFaces face,
  const GlVertexArray &vao,
  const GlTexture2D &prevFaceTex,
  GlAttachOnlyFramebuffer *targetFb) {
  if (!targetFb) {
    return;
  }

  // have to cancel out the camera movement to get pure rotation matrix
  HMatrix camWorld =
    HMatrixGen::translation(-cameraWorldPos_.x(), -cameraWorldPos_.y(), -cameraWorldPos_.z())
    * projectCamera_->world();
  HMatrix camMvp = projectCamera_->projection() * camWorld.inv();

  HMatrix mv = modelViewMatrix(face);
  HMatrix mvp = perspectiveProjection90DegreeMat_ * mv;

  // bind inference result
  srcTex_.bind();

  // bind the shader
  semProjectShader_->bind();

  targetFb->bind();
  checkGLError("[semantics-cubemap-renderer] bind framebuffer");

  glViewport(0, 0, texSize_, texSize_);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load vertex data
  vao.bind();
  checkGLError("[semantics-cubemap-renderer] bind quad");
  if (face == CubemapFaces::POSITIVE_Z || face == CubemapFaces::POSITIVE_X) {
    glFrontFace(GL_CW);
  } else {
    glFrontFace(GL_CCW);
  }

  GLint mvpLoc = semProjectShader_->shader()->location("mvp");
  GLint srcMvpLoc = semProjectShader_->shader()->location("sourceCameraMvp");

  GLint locSrcTex = semProjectShader_->shader()->location("colorSampler");
  GLint locSemTex = semProjectShader_->shader()->location("semSampler");

  // Always draw the main image.
  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());
  glUniformMatrix4fv(srcMvpLoc, 1, GL_FALSE, camMvp.data().data());

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, prevFaceTex.id());
  glUniform1i(locSemTex, 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, srcTex_.id());

  // Draw the filtered input.
  vao.drawElements();
  checkGLError("[semantics-cubemap-renderer] draw elements (input filter)");

  vao.unbind();
  srcTex_.unbind();
  targetFb->unbind();
  checkGLError("[semantics-cubemap-renderer] face quad unbind");
}

void SemanticsCubemapRenderer::renderTextureDirect(
  const GlTexture &srcTex, int dstWidth, int dstHeight, const GlVertexArray &quad) {
  // bind source result
  srcTex.bind();

  // bind the shader
  imageShader_->bind();

  glViewport(0, 0, dstWidth, dstHeight);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  quad.bind();
  glFrontFace(GL_CCW);
  checkGLError("[semantics-cubemap-renderer] bind quad");

  GLint mvpLoc = imageShader_->shader()->location("mvp");
  GLint opacityLoc = imageShader_->shader()->location("opacity");

  GLint locSrcTex = imageShader_->shader()->location("colorSampler");

  auto mvp = HMatrixGen::i();
  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());
  glUniform1f(opacityLoc, 1.0f);

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcTex.id());

  quad.drawElements();

  srcTex.unbind();
  quad.unbind();
}

void SemanticsCubemapRenderer::copyTexture(
  const GlTexture &srcTex,
  int dstWidth,
  int dstHeight,
  const GlVertexArray &quad,
  GlAttachOnlyFramebuffer *cubeFaceFb) {
  if (!cubeFaceFb) {
    return;
  }

  cubeFaceFb->bind();
  checkGLError("[semantics-cubemap-renderer] bind framebuffer");

  renderTextureDirect(srcTex, dstWidth, dstHeight, quad);

  cubeFaceFb->unbind();
  checkGLError("[semantics-cubemap-renderer] face framebuffer unbind");
}

void SemanticsCubemapRenderer::renderToCubemap(const HMatrix &cameraExtrinsics) {
  if (!projectCamera_) {
    return;
  }

  projectCamera_->setLocal(cameraExtrinsics);

  // corner rays in world space
  cameraWorldPos_ = cameraExtrinsics * HPoint3{0.0f, 0.0f, 0.0f};
  frustumWorldCorners_ = cameraExtrinsics * frustumLocalCorners_;

  for (size_t i = 0; i < 4; ++i) {
    const auto &corner = frustumWorldCorners_[i];
    frustumWorldRayDir_[i] = {
      corner.x() - cameraWorldPos_.x(),
      corner.y() - cameraWorldPos_.y(),
      corner.z() - cameraWorldPos_.z()};
  }

  // for each cubemap face, compute the frustum intersection quad then render the quad
  const int targetCubeTexIndex = (cubeTexSamplerIndex_ + 1) % 2;
  Vector<HPoint3> intersectionPoints;
  for (int f = 0; f < 6; ++f) {
    if (f == 3) {
      // skip rendering to negative-Y
      continue;
    }

    if (intersectFrustumWithPlane(
          cubePlanes_[f],
          frustumWorldRayDir_,
          cubeFaceTriangles_[f],
          cubeFaceBoundaryLines_[f],
          &intersectionPoints)) {
      projectImageToFace(
        static_cast<CubemapFaces>(f),
        faceQuads_[f],
        cubeFaces_[cubeTexSamplerIndex_][f],
        &faceTex2dFbs_[targetCubeTexIndex][f]);
      copyTexture(
        cubeFaces_[targetCubeTexIndex][f].tex(), texSize_, texSize_, unitQuad_, &cubeFaceFbs[f]);
    }
  }

  cubeTexSamplerIndex_ = targetCubeTexIndex;
}

void SemanticsCubemapRenderer::initOutputSemanticsScene(
  c8_PixelPinholeCameraModel k, int outputWidth, int outputHeight) {
  if (outputWidth <= 0 || outputHeight <= 0) {
    return;
  }

  outputCamera_ = ObGen::perspectiveCamera(k, k.pixelsWidth, k.pixelsHeight);

  // render cubemap into outputSemPortraitFbo_ in portrait mode,
  // then run post-processing into outputProcessedSceneFbo_
  // readback pixels from outputProcessedSceneFbo_ into outputSemSceneImage_

  int portraitWidth = (outputWidth <= outputHeight) ? outputWidth : outputHeight;
  int portraitHeight = (outputWidth >= outputHeight) ? outputWidth : outputHeight;
  // only allocate resources if output dimensions changed
  if (outputSemPortraitWidth_ != portraitWidth || outputSemPortraitHeight_ != portraitHeight) {
    outputSemPortraitFbo_ = makeLinearRGBA8888Framebuffer(portraitWidth, portraitHeight);
  }

  if (outputSemSceneWidth_ != outputWidth || outputSemSceneHeight_ != outputHeight) {
    outputProcessedSceneFbo_ = makeLinearRGBA8888Framebuffer(outputWidth, outputHeight);
    outputSemSceneImage_ = RGBA8888PlanePixelBuffer(outputHeight, outputWidth);
  }

  if (semSkyRenderBuffer_.width() != outputWidth || semSkyRenderBuffer_.height() != outputHeight) {
    semSkyRenderBuffer_ = makeDepthRenderbuffer(outputWidth, outputHeight);
    semSkyFb_->attachRenderBuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, semSkyRenderBuffer_.id());
  }

  int dsWidth = portraitWidth / INTMD_DOWNSIZE_SCALE;
  int dsHeight = portraitHeight / INTMD_DOWNSIZE_SCALE;

  int intmdW = intmdSmallFbo1_.tex().width();
  int intmdH = intmdSmallFbo1_.tex().height();
  if (intmdW != dsWidth || intmdH != dsHeight) {
    intmdSmallFbo1_ = makeLinearRGBA8888Framebuffer(dsWidth, dsHeight);
    intmdSmallFbo2_ = makeLinearRGBA8888Framebuffer(dsWidth, dsHeight);
  }

  outputSemPortraitWidth_ = portraitWidth;
  outputSemPortraitHeight_ = portraitHeight;

  outputSemSceneWidth_ = outputWidth;
  outputSemSceneHeight_ = outputHeight;
}

ConstRGBA8888PlanePixels SemanticsCubemapRenderer::result() const {
  return outputSemSceneImage_.pixels();
}

void SemanticsCubemapRenderer::drawOutputSemantics(const HMatrix &cameraExtrinsics) {
  if (outputSemSceneWidth_ <= 0 || outputSemSceneHeight_ <= 0) {
    return;
  }

  // deferred reading
  outputProcessedSceneFbo_.bind();
  glReadPixels(
    0,
    0,
    outputSemSceneWidth_,
    outputSemSceneHeight_,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    outputSemSceneImage_.pixels().pixels());
  outputProcessedSceneFbo_.unbind();

  // draw next frame
  outputSemPortraitFbo_.bind();
  renderCubemap(cameraExtrinsics, outputSemPortraitWidth_, outputSemPortraitHeight_);
  outputSemPortraitFbo_.unbind();

  // post-processing
  GlVertexArray *quad = &flipYQuad_;
  if (deviceOrientation_ == 90) {
    quad = &flipYCCWQuad_;
  } else if (deviceOrientation_ == -90) {
    quad = &flipYCWQuad_;
  }
  outputProcessedSceneFbo_.bind();
  renderSkyTexture(
    outputSemPortraitFbo_.tex(),
    outputSemPortraitFbo_.tex(),
    outputSemSceneWidth_,
    outputSemSceneHeight_,
    *quad);
  outputProcessedSceneFbo_.unbind();
}

// render the cubemap directly
// if want to render to a texture, the texture needs to bind with a framebuffer before this call
void SemanticsCubemapRenderer::renderCubemap(
  const HMatrix &cameraExtrinsics, int texWidth, int texHeight) {
  outputCamera_->setLocal(cameraExtrinsics);

  auto camWorldPos = cameraExtrinsics * HPoint3{0.0f, 0.0f, 0.0f};

  // have to cancel out the camera movement to get pure rotation matrix
  HMatrix camWorld = HMatrixGen::translation(-camWorldPos.x(), -camWorldPos.y(), -camWorldPos.z())
    * outputCamera_->world();
  HMatrix mvp = outputCamera_->projection() * camWorld.inv();

  // bind the shader
  cubemapShader_->bind();

  glViewport(0, 0, texWidth, texHeight);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  outCube_->bind();
  glFrontFace(GL_CCW);
  checkGLError("[semantics-cubemap-renderer] bind cube");

  GLboolean restoreDepthMask;
  glGetBooleanv(GL_DEPTH_WRITEMASK, &restoreDepthMask);
  glDepthMask(GL_FALSE);

  GLint mvpLoc = cubemapShader_->shader()->location("mvp");
  GLint opacityLoc = cubemapShader_->shader()->location("opacity");
  GLint locSrcTex = cubemapShader_->shader()->location("colorSampler");

  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());
  glUniform1f(opacityLoc, 1.0f);

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex_.id());

  outCube_->drawElements();

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glDepthMask(restoreDepthMask);
  outCube_->unbind();
}

void SemanticsCubemapRenderer::gaussianBlurLinear(
  const GlTexture2D &srcTex,
  int srcWidth,
  int srcHeight,
  int dstWidth,
  int dstHeight,
  float dirU,
  float dirV,
  const GlVertexArray &quad) {
  // bind source result
  srcTex.bind();

  float uStep = 1.0f / static_cast<float>(srcWidth);
  float vStep = 1.0f / static_cast<float>(srcHeight);

  const float dirNorm = std::sqrt(dirU * dirU + dirV * dirV);
  float dirUNorm = dirU;
  float dirVNorm = dirV;
  if (dirNorm > 0) {
    dirUNorm = dirU / dirNorm;
    dirVNorm = dirV / dirNorm;
  }

  int kernelSize = gaussianLinearShader_->getKernelSize();
  const float *kernelWeights = gaussianLinearShader_->getKernelWeights();

  // bind the shader
  gaussianLinearShader_->bind();

  glViewport(0, 0, dstWidth, dstHeight);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  quad.bind();
  glFrontFace(GL_CCW);
  checkGLError("[semantics-cubemap-renderer] gaussian linear bind quad");

  GLint locMvp = gaussianLinearShader_->shader()->location("mvp");
  GLint locSrcTex = gaussianLinearShader_->shader()->location("colorSampler");
  GLint locUvStep = gaussianLinearShader_->shader()->location("uvStep");
  GLint locKernelSize = gaussianLinearShader_->shader()->location("kernelSize");
  GLint locKernelWeights = gaussianLinearShader_->shader()->location("kWeight");

  auto mvp = HMatrixGen::i();
  glUniformMatrix4fv(locMvp, 1, GL_FALSE, mvp.data().data());

  glUniform2f(locUvStep, dirUNorm * uStep, dirVNorm * vStep);
  checkGLError("[semantics-cubemap-renderer] gaussian linear uvStep");

  glUniform1i(locKernelSize, kernelSize);
  glUniform1fv(locKernelWeights, kernelSize, kernelWeights);

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcTex.id());

  quad.drawElements();

  srcTex.unbind();
  quad.unbind();
}

void SemanticsCubemapRenderer::setSkyPostProcessParameters(
  float edgeSmoothness, bool haveSeenSky, bool flipAlphaMask) {
  haveSeenSky_ = haveSeenSky ? 1 : 0;
  toFlipAlphaMask_ = flipAlphaMask ? 1 : 0;

  edgeSmoothness_ = edgeSmoothness;
}

void SemanticsCubemapRenderer::renderSkyTexture(
  const GlTexture2D &srcTex,
  const GlTexture2D &alphaTex,
  int dstWidth,
  int dstHeight,
  const GlVertexArray &quad) {
  // bind source result
  srcTex.bind();

  // bind the sky post-processing shader
  semSkyShader_->bind();

  glViewport(0, 0, dstWidth, dstHeight);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  quad.bind();
  glFrontFace(GL_CCW);
  checkGLError("[semantics-cubemap-renderer] bind quad");

  GLint mvpLoc = semSkyShader_->shader()->location("mvp");
  GLint locSrcTex = semSkyShader_->shader()->location("colorSampler");
  GLint locAlphaTex = semSkyShader_->shader()->location("alphaSampler");
  GLint locHaveSeenSky = semSkyShader_->shader()->location("haveSeenSky");
  GLint locFlipAlpha = semSkyShader_->shader()->location("flipAlphaMask");

  GLint locSmoothness = semSkyShader_->shader()->location("smoothness");

  auto mvp = HMatrixGen::i();
  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());

  glUniform1i(locHaveSeenSky, haveSeenSky_);
  glUniform1i(locFlipAlpha, toFlipAlphaMask_);

  // in the semantics-sky.frag shader, we will do linear interpolation
  // between raw output and blurred output -
  // float sky = rawSemColor.r * (1.0f - smoothness) + blurEdge * smoothness;
  // the computation below will have slower drop off for 'blurEdge' weight
  // e.g. when edgeSmoothness_ = 0.5, the weight for 'blurEdge' is 0.75
  float smooth = 1.0 - (1.0 - edgeSmoothness_) * (1.0 - edgeSmoothness_);
  glUniform1f(locSmoothness, smooth);

  glUniform1i(locAlphaTex, 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, alphaTex.id());

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcTex.id());

  quad.drawElements();
  checkGLError("[semantics-cubemap-renderer] draw quad");

  srcTex.unbind();
  quad.unbind();
}

void SemanticsCubemapRenderer::drawOutputSemantics(
  const HMatrix &cameraExtrinsics, int texId, int texWidth, int texHeight) {
  if (outputSemSceneWidth_ <= 0 || outputSemSceneHeight_ <= 0 || texWidth <= 0 || texHeight <= 0) {
    return;
  }

  // render initial result to outputSemPortraitFbo_
  outputSemPortraitFbo_.bind();
  if (useDirectSemanticsTex_ && srcTexAllocated_) {
    // use semantics results directly
    renderTextureDirect(
      srcTex_.tex(), outputSemPortraitWidth_, outputSemPortraitHeight_, flipYQuad_);
  } else {
    // render accumulated semantics results from the cached cubemap
    renderCubemap(cameraExtrinsics, outputSemPortraitWidth_, outputSemPortraitHeight_);
  }
  outputSemPortraitFbo_.unbind();

  // downscale to smaller size for post processing
  const int intmdW = intmdSmallFbo1_.tex().width();
  const int intmdH = intmdSmallFbo1_.tex().height();
  intmdSmallFbo1_.bind();
  renderTextureDirect(outputSemPortraitFbo_.tex().tex(), intmdW, intmdH, unitQuad_);
  intmdSmallFbo1_.unbind();

  // blur downsized textures
  intmdSmallFbo2_.bind();
  gaussianBlurLinear(intmdSmallFbo1_.tex(), intmdW, intmdH, intmdW, intmdH, 0.0f, 1.0f, unitQuad_);
  intmdSmallFbo2_.unbind();

  intmdSmallFbo1_.bind();
  gaussianBlurLinear(intmdSmallFbo2_.tex(), intmdW, intmdH, intmdW, intmdH, 1.0f, 0.0f, unitQuad_);
  intmdSmallFbo1_.unbind();

  // post-processing
  GlVertexArray *quad = &flipYQuad_;
  if (deviceOrientation_ == 90) {
    quad = &flipYCCWQuad_;
  } else if (deviceOrientation_ == -90) {
    quad = &flipYCWQuad_;
  }

  semSkyFb_->attachColorTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId);
  semSkyFb_->bind();
  renderSkyTexture(intmdSmallFbo1_.tex(), outputSemPortraitFbo_.tex(), texWidth, texHeight, *quad);
  semSkyFb_->unbind();
  semSkyFb_->attachColorTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0);
}

}  // namespace c8
