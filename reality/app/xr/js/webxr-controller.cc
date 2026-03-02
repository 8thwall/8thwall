// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Akul Santhosh (akulsanthosh@nianticlabs.com)

// WebXR Controller manages the data received from WebXR -
// Receives the capnp messages for the intrinsic and extrinsic data.
// Uses the id mapped to the camera texture for CV.

// This can act as the entry point for different AR experiences, currently
// image targets is implemented. The corresponding js file is webxr-controller.js

#ifdef JAVASCRIPT

#include <emscripten.h>

#include "c8/c8-log.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/capnp-messages.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/protolog/xr-requests.h"
#include "c8/string/format.h"
#include "c8/symbol-visibility.h"
#include "reality/engine/features/feature-detector.h"
#include "reality/engine/features/gl-reality-frame.h"
#include "reality/engine/features/gr8-feature-shader.h"
#include "reality/engine/geometry/image-warp.h"
#include "reality/engine/geometry/pose-pnp.h"
#include "reality/engine/imagedetection/detection-image-loader.h"
#include "reality/engine/imagedetection/detection-image-tracker.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/imagedetection/location.h"
#include "reality/engine/imagedetection/tracked-image.h"

using namespace c8;

namespace {
struct EngineData {
  std::unique_ptr<GlRealityFrame> gl;
  std::unique_ptr<Gr8FeatureShader> glShader;
  HMatrix extrinsic = HMatrixGen::i();
  c8_PixelPinholeCameraModel intrinsic;
  DetectionImageLoader targetLoader;
  DetectionImage targetFeatures;
  c8_PixelPinholeCameraModel searchK;
  TrackerInput trackerInput{c8_PixelPinholeCameraModel{}, Quaternion{1, 0, 0, 0}};
  FeatureDetector detector;
  Vector<PointMatch> globalMatches;
  HMatrix globalPose = HMatrixGen::i();
};

std::unique_ptr<EngineData> &data() {
  static std::unique_ptr<EngineData> d(nullptr);
  if (d == nullptr) {
    d.reset(new EngineData());
  }
  return d;
}

const FrameWithPoints &getFirstImageTargetRoiFeatures(const Vector<FrameWithPoints> &roiPoints) {
  for (const auto &pts : roiPoints) {
    if (pts.roi().source == ImageRoi::Source::IMAGE_TARGET) {
      return pts;
    }
  }
  return roiPoints[0];
}

void writeDetectedImageLocation(const LocatedImage &found) {
  MutableRootMessage<WebXRController> webxrMsg;
  auto req = webxrMsg.builder();
  req.setWidthInMeters(found.width);
  req.setHeightInMeters(found.height);
  setPosition(translation(found.pose.inv()), req.getPlace().getPosition());
  setQuaternion(rotation(found.pose.inv()), req.getPlace().getRotation());

  ConstRootMessage<WebXRController> msg = ConstRootMessage<WebXRController>(webxrMsg);
  EM_ASM_(
    {
      window._c8.webxrOverlayCanvasPtr = $0;
      window._c8.webxrOverlayCanvasSize = $1;
    },
    msg.bytes().begin(),
    msg.bytes().size());
}

LocatedImage locate(
  const String &targetFile,
  const HMatrix &poseFromTarget,
  const DetectionImage &t,
  const HMatrix &camPos,
  float scale,
  c8_PixelPinholeCameraModel searchK) {
  ImageRoi roi = {
    ImageRoi::Source::IMAGE_TARGET,
    0,
    targetFile,
    glImageTargetWarp(t.framePoints().intrinsic(), searchK, poseFromTarget)};
  auto pose = updateWorldPosition(camPos, scaleTranslation(scale, poseFromTarget.inv()));

  HPoint3 origin{0.0f, 0.0f, scale};
  HPoint3 impos = pose * origin;
  auto imrot = Quaternion::fromHMatrix(pose);

  HPoint2 llb;
  HPoint2 urb;
  t.framePoints().frameBounds(&llb, &urb);
  return {
    cameraMotion(impos, imrot), (urb.x() - llb.x()) * scale, (urb.y() - llb.y()) * scale, roi};
}
}  // namespace

extern "C" {
// Initialises the target image loader and returns the texture id to copy the target image.
C8_PUBLIC
GLuint c8EmAsm_initImageTargetLoader(int height, int width, String targetName) {
  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  imageTargetMetadata.builder().setType(ImageTargetTypeMsg::PLANAR);
  imageTargetMetadata.builder().setName(targetName);
  imageTargetMetadata.builder().setImageWidth(width);
  imageTargetMetadata.builder().setImageHeight(height);

  if (data()->glShader == nullptr) {
    data()->glShader.reset(new Gr8FeatureShader());
    data()->glShader->initialize();
  }
  data()->targetLoader.initialize(
    data()->glShader.get(), imageTargetMetadata.reader(), data()->intrinsic);

  // Returns the texture ID created internally for binding the image target.
  return data()->targetLoader.imTexture().id();
}

// Draws the texture, reads pixels and extracts target features.
C8_PUBLIC
void c8EmAsm_processNewImageTarget() {
  data()->targetLoader.processGpu();
  data()->targetLoader.readDataToCpu();
  data()->targetFeatures = data()->targetLoader.extractFeatures();
}

// Initialize gpu processing for the search texture.
C8_PUBLIC
void c8EmAsm_initDetection(int captureWidth, int captureHeight, int rotation) {
  if (data()->glShader == nullptr) {
    data()->glShader.reset(new Gr8FeatureShader());
    data()->glShader->initialize();
    data()->gl.reset(new GlRealityFrame());
  }
  data()->gl->initialize(data()->glShader.get(), captureWidth, captureHeight, rotation);
}

// Reads intrinsic data from WebXR.
C8_PUBLIC
void c8EmAsm_initIntrinsic(uint8_t *bytes, int numBytes) {
  ConstRootMessage<WebXRController> requestMsg(bytes, numBytes);
  auto request = requestMsg.reader().getIntrinsic();
  auto width = request.getPixelsWidth();
  auto height = request.getPixelsHeight();
  auto cx = request.getCenterPointX();
  auto cy = request.getCenterPointY();
  auto fh = request.getFocalLengthHorizontal();
  auto fv = request.getFocalLengthVertical();

  data()->intrinsic = c8_PixelPinholeCameraModel{width, height, cx, cy, fh, fv};
}

// Reads extrinsic data from WebXR.
C8_PUBLIC
void c8EmAsm_initExtrinsic(uint8_t *bytes, int numBytes) {
  ConstRootMessage<WebXRController> requestMsg(bytes, numBytes);
  auto msg = requestMsg.reader().getExtrinsic();
  data()->extrinsic = HMatrix{
    {msg[0], msg[1], msg[2], msg[3]},
    {msg[4], msg[5], msg[6], msg[7]},
    {msg[8], msg[9], msg[10], msg[11]},
    {msg[12], msg[13], msg[14], msg[15]}};
}

C8_PUBLIC
bool c8EmAsm_detectGlobalLocation(GLuint id, int rows, int cols) {
  // Since we're not yet tracking an image, configure the renderer to scan the central portion at
  // higher resolution.
  auto l0 = data()->gl->pyramid().levels[0];

  data()->searchK = Intrinsics::rotateCropAndScaleIntrinsics(data()->intrinsic, l0.w, l0.h);
  data()->gl->addNextDrawHiResScans(data()->searchK, {0.0f, 0.0f});

  // Render the pyramid and read back the result.
  data()->gl->draw(id, GlRealityFrame::Options::DEFER_READ);
  data()->gl->readPixels();

  // Extract features from the search image. TrackerInput holds image points (in ray space) for
  // both the pyramid and extracted ROIs, and also the gravity-oriented pose reported by the phone.
  // Here we set the pose to identity.
  data()->trackerInput = TrackerInput(data()->searchK, {1.0f, 0.0f, 0.0f, 0.0f});
  data()->detector.detectFeatures(
    data()->gl->pyramid(), &data()->trackerInput.framePoints, &data()->trackerInput.roiPoints);

  // Concatenate features and hi-res features.
  auto combinedPoints = data()->trackerInput.framePoints.clone();
  for (const auto &f : data()->trackerInput.roiPoints) {
    if (f.roi().source == ImageRoi::Source::HIRES_SCAN) {
      combinedPoints.addAll(f);
    }
  }

  // Compute global matches again with the default score limit.
  data()->targetFeatures.globalMatcher().match(combinedPoints, &data()->globalMatches);

  // Construct a 3d model of the image target (globalWorldPts) from the matched points, and extract
  // the ray locations of the matched points from the camera.
  Vector<HPoint3> globalWorldPts;
  Vector<HPoint2> globalCamRays;
  getPointsAndRays(
    data()->globalMatches, data()->targetFeatures, combinedPoints, &globalWorldPts, &globalCamRays);

  // Run robustPnP to find the location of the camera with respect to the image target 3d model.
  Vector<uint8_t> globalInliers;
  RobustPoseScratchSpace scratch;
  RandomNumbers random;
  auto found = robustPnP(
    globalWorldPts,
    globalCamRays,
    HMatrixGen::i(),
    {},
    &data()->globalPose,
    &globalInliers,
    &random,
    &scratch);

  // Target image was not found. Run detectGlobalLocation() again for next frame.
  if (!found) {
    return false;
  }

  // Target image was found. Run DetectLocalLocation() for this and consecutive frame till
  // local location not found.
  return true;
}

C8_PUBLIC
bool c8EmAsm_detectLocalLocation(GLuint id, int rows, int cols) {
  // Known locaction of the camera in world space.
  auto camPos = data()->extrinsic;
  auto scale = 1.0f;  // guessed scale of the image target.

  // Now we know the location of the camera with respect to the target image, but what we want is to
  // locate the target image with respect to the camera.
  auto globalLocatedImage = locate(
    "imagetarget", data()->globalPose, data()->targetFeatures, camPos, scale, data()->searchK);

  // Now that we have found the target, rerun feature exatraction focusing in on the image target.
  data()->gl->addNextDrawRoi(globalLocatedImage.roi);
  data()->gl->addNextDrawHiResScans(data()->searchK, {0.0f, 0.0f});

  // Render the pyramid and read back the result.
  data()->gl->draw(id, GlRealityFrame::Options::DEFER_READ);
  data()->gl->readPixels();

  // Extract features from the search image. TrackerInput holds image points (in ray space) for
  // both the pyramid and extracted ROIs, and also the gravity-oriented pose reported by the phone.
  // Here we set the pose to identity.
  data()->trackerInput.framePoints.clear();
  data()->trackerInput.roiPoints.clear();
  data()->detector.detectFeatures(
    data()->gl->pyramid(), &data()->trackerInput.framePoints, &data()->trackerInput.roiPoints);

  // Based on the estimated pose, compute the location where we think the points probably are in
  // the field of view of the camera that took the photo of the image target.
  auto &localFeatures = getFirstImageTargetRoiFeatures(data()->trackerInput.roiPoints);
  auto featsRayInTargetView =
    projectToTargetSpace(data()->targetFeatures, localFeatures, data()->globalPose);

  // Compute local matches in the space of the image target.
  Vector<PointMatch> localMatches;
  data()->targetFeatures.localMatcher().useScaleFilter(true);
  data()->targetFeatures.localMatcher().setDescriptorThreshold(64);
  data()->targetFeatures.localMatcher().setRoiScale(true);
  data()->targetFeatures.localMatcher().setRadius(0.1f);
  data()->targetFeatures.localMatcher().findMatches(featsRayInTargetView, &localMatches);

  Vector<HPoint2> imTargetRays;
  Vector<HPoint2> camRays;
  Vector<float> weights;
  getMatchedRays(
    localMatches, data()->targetFeatures, localFeatures, &imTargetRays, &camRays, &weights);
  Vector<uint8_t> localInliers;
  RobustPoseScratchSpace scratch2;
  HMatrix localPose = data()->globalPose;
  bool found = solveImageTargetHomography(
    imTargetRays,
    camRays,
    weights,
    {10, 20, 500e-6f, 1e-2f, 10.0f},
    &localPose,
    &localInliers,
    &scratch2);

  // Target image was not found in local search. Run detectGlobalLocation() for next frame.
  if (!found) {
    return false;
  }

  Vector<PointMatch> localMatchesInliers;
  for (int i = 0; i < localInliers.size(); ++i) {
    if (localInliers[i]) {
      localMatchesInliers.push_back(localMatches[i]);
    }
  }

  auto localLocatedImage =
    locate("imagetarget", localPose, data()->targetFeatures, camPos, scale, data()->searchK);

  // Write the pose of the detected image in a capnp message.
  writeDetectedImageLocation(localLocatedImage);
  // Target image was found. Run DetectLocalLocation() for consecutive frame till
  // local location not found.
  return true;
}
}  // EXTERN "C"

#else
#warning "xr-js-cc requires --cpu=js"
#endif
