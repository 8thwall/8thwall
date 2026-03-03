// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Renderer for cubemap with semantics results

#pragma once

#include "c8/geometry/line3.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/pixels/render/cubemap-shader.h"
#include "c8/pixels/render/image-shader.h"
#include "c8/pixels/render/object8.h"
#include "c8/quaternion.h"
#include "c8/vector.h"
#include "reality/engine/semantics/semantics-gaussian-linear-shader.h"
#include "reality/engine/semantics/semantics-projection-shader.h"
#include "reality/engine/semantics/semantics-sky-shader.h"

namespace c8 {

class SemanticsCubemapRenderer {
public:
  enum class CubemapFaces {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,
  };

  // Set the intrinsics for projecting textures onto the cubemap
  // the intrinsics can be pre-calculated based on a different aspect ratio, etc.
  void setProjectionIntrinsics(c8_PixelPinholeCameraModel k);

  // if true, we use semantics texture directly.
  // if false, we render the semantics results from the cached cubemap.
  void setToUseSrcTextureDirectly(bool toUseDirectly) { useDirectSemanticsTex_ = toUseDirectly; }

  // upload semantics result from CPU to GPU texture
  // @param srcPixels semantics pixels to be uploaded to texture
  void updateSrcTexture(ConstRGBA8888PlanePixels srcPixels);

  // render srcTex into cubemap
  void renderToCubemap(const HMatrix &cameraExtrinsics);

  // The mask texture.
  GlTexture maskTex() const { return srcTex_.tex(); }

  // cubemap texture
  GlTexture cubemapTex() const { return cubeTex_.tex(); }

  // init output semantics scene when camera intrinsics or output image dimensions change
  void initOutputSemanticsScene(c8_PixelPinholeCameraModel k, int outputWidth, int outputHeight);

  ConstRGBA8888PlanePixels result() const;

  // render the output semantics scene and get the semantics pixels
  void drawOutputSemantics(const HMatrix &cameraExtrinsics);

  // render semantics from cubemap
  void drawOutputSemantics(const HMatrix &cameraExtrinsics, int texId, int texWidth, int texHeight);

  // set parameters for sky result post-processing
  void setDeviceOrientation(int orientation) { deviceOrientation_ = orientation; };
  void setSkyPostProcessParameters(float edgeSmoothness, bool haveSeenSky, bool flipAlphaMask);

  // Default constructor.
  explicit SemanticsCubemapRenderer(int texSize = 256);

  // Default move constructors.
  SemanticsCubemapRenderer(SemanticsCubemapRenderer &&o) = default;
  SemanticsCubemapRenderer &operator=(SemanticsCubemapRenderer &&o) = default;

  // Disallow copying.
  SemanticsCubemapRenderer(const SemanticsCubemapRenderer &) = delete;
  SemanticsCubemapRenderer &operator=(const SemanticsCubemapRenderer &) = delete;

private:
  // A plane is defined as A*x + B*y + C*z + D = 0
  // Each subvector is a vector of 4 floats defining a plane as {{A, B, C, D}, ...}
  Vector<Vector<float>> cubePlanes_;

  // cubemap triangles for intersection tests
  Vector<HPoint3> cubeFaceTriangles_[6];

  // cubemap boundary lines for intersection tests
  Vector<Line3> cubeFaceBoundaryLines_[6];

  // rendering targets for each plane
  Vector<uint32_t> cubePlaneTargets_;

  // quads for each face when drawing to the cubemap
  GlVertexArray faceQuads_[6];

  int texSize_ = 0;  // texture size for each face of the cubemap.

  bool srcTexAllocated_ = false;  // true if the srcTex_ is allocated under the current GL context
  GlTexture2D srcTex_;            // source texture from semantics inference results

  int cubeTexSamplerIndex_ = 0;
  GlTextureCubemap cubeTex_;          // cubemap texture
  GlRenderbuffer depthRenderBuffer_;  // depth render buffer shared to all framebuffers

  // Framebuffer with all 6 faces of the cubeTex_ attached
  GlAttachOnlyFramebuffer cubeFaceFbs[6];

  // cubemap faces for 2 set of render targets
  // we first render to the faces cubeFaces_[target][face]
  // then copy the results into the cubemap texture
  GlTexture2D cubeFaces_[2][6];

  GlAttachOnlyFramebuffer faceTex2dFbs_[2][6];

  // camera frustum
  HPoint3 cameraWorldPos_;
  Vector<HPoint3> frustumLocalCorners_;
  Vector<HPoint3> frustumWorldCorners_;
  Vector<HVector3> frustumWorldRayDir_;

  // intersection points between the frustum rays and cubemap faces
  Vector<HPoint3> intersectionPts_;

  // using this camera for rendering the semantics results onto the cubemap texture
  std::unique_ptr<Camera> projectCamera_;

  // Projection matrix for cubemap virtual cameras that has 90degree FOV.
  // Same projection for all 6 virtual cameras when rendering into 6 faces.
  HMatrix perspectiveProjection90DegreeMat_ = HMatrixGen::i();

  // project srcTex onto a face of the cubemap if the face intersects with camera frustum
  std::unique_ptr<SemanticsProjectionShader> semProjectShader_;
  void projectImageToFace(
    CubemapFaces face,
    const GlVertexArray &vao,
    const GlTexture2D &prevFaceTex,
    GlAttachOnlyFramebuffer *targetFb);

  void renderTextureDirect(
    const GlTexture &srcTex, int dstWidth, int dstHeight, const GlVertexArray &quad);

  // simply passthrough from the rendered 2D texture
  std::unique_ptr<ImageShader> imageShader_;
  GlVertexArray unitQuad_;
  void copyTexture(
    const GlTexture &srcTex,
    int dstWidth,
    int dstHeight,
    const GlVertexArray &quad,
    GlAttachOnlyFramebuffer *cubeFaceFb);

  // output semantics scene & renderer
  int outputSemSceneWidth_ = 0;
  int outputSemSceneHeight_ = 0;
  RGBA8888PlanePixelBuffer outputSemSceneImage_;

  // always render the cubemap in portrait mode
  int outputSemPortraitWidth_ = 0;
  int outputSemPortraitHeight_ = 0;
  GlFramebufferObject outputSemPortraitFbo_;

  // intermediate FBO to blur the cubemap rendering result
  std::unique_ptr<SemanticsGaussianLinearShader> gaussianLinearShader_;
  void gaussianBlurLinear(
    const GlTexture2D &srcTex,
    int srcWidth,
    int srcHeight,
    int dstWidth,
    int dstHeight,
    float dirU,
    float dirV,
    const GlVertexArray &quad);

  GlFramebufferObject intmdSmallFbo1_;
  GlFramebufferObject intmdSmallFbo2_;

  // flag to indicate whether we want to render from the cubemap or use semantics texture directly
  bool useDirectSemanticsTex_ = false;

  // resources for rendering cubemap directly to screen or framebuffer
  std::unique_ptr<GlVertexArray> outCube_;
  std::unique_ptr<Camera> outputCamera_;
  std::unique_ptr<CubemapShader> cubemapShader_;
  void renderCubemap(const HMatrix &cameraExtrinsics, int texWidth, int texHeight);

  // parameters for post-processing
  int deviceOrientation_ = 0;  // 0: portrait; 90: landscape left; -90: landscape right

  // edge feathering parameters
  int haveSeenSky_ = 0;
  int toFlipAlphaMask_ = 0;
  float edgeSmoothness_ = 0.5;

  // resources for post-processing after rendering the cubemap
  std::unique_ptr<GlAttachOnlyFramebuffer> semSkyFb_;
  GlRenderbuffer semSkyRenderBuffer_;

  std::unique_ptr<SemanticsSkyShader> semSkyShader_;
  GlFramebufferObject outputProcessedSceneFbo_;
  GlVertexArray flipYQuad_;     // unit quad with Y flipped
  GlVertexArray flipYCWQuad_;   // unit quad with Y flipped, then rotate clockwise 90degree
  GlVertexArray flipYCCWQuad_;  // unit quad with Y flipped, then rotate counter-clockwise 90degree
  void renderSkyTexture(
    const GlTexture2D &srcTex,
    const GlTexture2D &alphaTex,
    int dstWidth,
    int dstHeight,
    const GlVertexArray &quad);
};

}  // namespace c8
