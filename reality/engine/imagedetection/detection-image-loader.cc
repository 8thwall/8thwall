// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "detection-image-loader.h",
  };
  deps = {
    ":detection-image",
    ":target-point",
    "//c8:parameter-data",
    "//c8/geometry:intrinsics",
    "//c8/geometry:parameterized-geometry",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/features:gl-reality-frame",
    "//reality/engine/features:gr8gl",
    "//reality/engine/features:gr8-feature-shader",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x265b319d);

#include "c8/geometry/intrinsics.h"
#include "c8/parameter-data.h"
#include "reality/engine/imagedetection/detection-image-loader.h"

namespace c8 {

namespace {

constexpr float epsilon = 1e-9f;

struct Settings {
  float minRollAngleFromXZPlaneForGorb;  // Minimum roll angle from the XZ plane for tracking image
                                         // targets using GORB
};

const Settings &settings() {
  static Settings settings_ = {
    globalParams().getOrSet("DetectionImageLoader.minRollAngleFromXZPlaneForGorb", 2.0f),
  };
  return settings_;
}

}  // namespace

void DetectionImageLoader::initialize(
  Gr8FeatureShader *glShader,
  ImageTargetMetadata::Reader imageTargetMetadataReader,
  c8_PixelPinholeCameraModel k) {
  imTexture_ = makeLinearRGBA8888Texture2D(
    imageTargetMetadataReader.getImageWidth(), imageTargetMetadataReader.getImageHeight());
  name_ = imageTargetMetadataReader.getName();
  // Indicates this is a upright 3:4 image in which the pixels have been rotated 90 degrees
  // clockwise (xrhome does this)
  isRotated_ = imageTargetMetadataReader.getIsRotated();

  rotateFeatures_ = isRotated_;
  // TODO(Yuling): need further support for GORB matching in curvy images.
  if (
    imageTargetMetadataReader.getType() == ImageTargetTypeMsg::PLANAR
    && imageTargetMetadataReader.getIsStaticTarget()) {
    float rollAngle = imageTargetMetadataReader.getRollAngle();
    float pitchAngle = imageTargetMetadataReader.getPitchAngle();
    // We can only use GORB if the roll angle is not too close to the XZ plane, and the pitch
    // angle is close to zero. Other wise, we use ORB.
    if (
      !(std::abs(rollAngle - 90.0f) < settings().minRollAngleFromXZPlaneForGorb
        || std::abs(rollAngle - 270.0f) < settings().minRollAngleFromXZPlaneForGorb)
      && (std::abs(pitchAngle) < epsilon)) {
      featureType_ = DescriptorType::GORB;
      // If an image's width is larger than its height, it is loaded rotated 90° in GL. In such
      // cases, we need to rotate features back for correct GORB matching and visualization.
      // TODO(Yuling): handle the case where both isRotated_ and rotateFeatures_ are true later.
      rotateFeatures_ |=
        (imageTargetMetadataReader.getImageWidth() > imageTargetMetadataReader.getImageHeight());

    } else {
      featureType_ = DescriptorType::ORB;
    }
  }

  rotation_ =
    imageTargetMetadataReader.getImageWidth() > imageTargetMetadataReader.getImageHeight() ? 90 : 0;
  gl_.initialize(
    glShader,
    imageTargetMetadataReader.getImageWidth(),
    imageTargetMetadataReader.getImageHeight(),
    rotation_);

  auto l0 = gl_.pyramid().levels[0];
  k_ = rotateFeatures_ ? Intrinsics::rotateCropAndScaleIntrinsics(k, l0.h, l0.w)
                       : Intrinsics::rotateCropAndScaleIntrinsics(k, l0.w, l0.h);

  switch (imageTargetMetadataReader.getType()) {
    case ImageTargetTypeMsg::PLANAR:
      planarGeom_ = PlanarImageGeometry{rotateFeatures_};
      detectionImageType_ = DetectionImageType::PLANAR;
      break;

    case ImageTargetTypeMsg::CURVY: {
      auto curvySpec = buildCurvySpec(imageTargetMetadataReader);
      auto cropWidth = imageTargetMetadataReader.getCropOriginalImageWidth();
      auto cropHeight = imageTargetMetadataReader.getCropOriginalImageHeight();
      setGeometry(cropWidth, cropHeight, curvySpec);
      break;
    }

    default:
      planarGeom_ = PlanarImageGeometry{rotateFeatures_};
      detectionImageType_ = DetectionImageType::UNSPECIFIED;
  }
}

/** Compute the matrix that transforms the points in the model space of a full curvy to points in
 * the model space of the curvy defined by an activation region, using the activation region as a
 * reference for a shared scale. For example, if we project the four corners of the activation
 * region into the full curvy space (by inverting this matrix), they would still be separated by
 * y-distance of 1.
 *
 * Note that the height of the curvy being tracked is always 1. The curvy is at (0, 0, 0), its
 * pose is the -z axis. Since we only track a cropped curvy, the tracker will output the origin
 * of this cropped curvy. Applying the invert of this matrix to the origin of the cropped
 * curvy will give you the origin of the full curvy.
 */
HMatrix DetectionImageLoader::buildFullLabelToCroppedLabelPose(CurvySpec s) {
  if (s.isRotated) {
    std::swap(s.crop.outerWidth, s.crop.outerHeight);
    std::swap(s.crop.cropStartX, s.crop.cropStartY);
    s.crop.cropStartY = 1.f - s.crop.cropStartY - (1.f / s.crop.outerHeight);
  }
  CurvyOuterCrop crop = s.crop;

  // trying to find the yaw rotation between a cropped label and a full label
  float innerLeftShiftToTargetCenter = crop.cropStartX + 0.5f / crop.outerWidth;
  float shiftInLabelSpace = innerLeftShiftToTargetCenter - 0.5;
  float shiftInRadian = 2 * M_PI * shiftInLabelSpace * s.arc;
  auto rot = HMatrixGen::yRadians(shiftInRadian);

  // pixel space has y pointing down but model space has y pointing up
  float translateY = -1.0f * (crop.outerHeight * crop.cropStartY + 0.5f - crop.outerHeight / 2);
  return HMatrixGen::translation(0, translateY, 0) * rot;
}

CurvySpec DetectionImageLoader::buildCurvySpec(
  ImageTargetMetadata::Reader imageTargetMetadataReader) {
  auto curvyGeometry = imageTargetMetadataReader.getCurvyGeometry();
  bool isRotated = imageTargetMetadataReader.getIsRotated();
  int originalWidth = imageTargetMetadataReader.getOriginalImageWidth();
  int originalHeight = imageTargetMetadataReader.getOriginalImageHeight();
  int x = imageTargetMetadataReader.getCropOriginalImageX();
  int y = imageTargetMetadataReader.getCropOriginalImageY();
  int cropWidth = imageTargetMetadataReader.getCropOriginalImageWidth();
  int cropHeight = imageTargetMetadataReader.getCropOriginalImageHeight();
  float circumferenceTop = curvyGeometry.getCurvyCircumferenceTop();
  float circumferenceBottom = curvyGeometry.getCurvyCircumferenceBottom();
  float targetCircumferenceTop = curvyGeometry.getTargetCircumferenceTop();

  float arc = targetCircumferenceTop / circumferenceTop;
  CurvyOuterCrop crop{
    1.0f * originalWidth / cropWidth,
    1.0f * originalHeight / cropHeight,
    1.0f * x / originalWidth,
    1.0f * y / originalHeight,
  };
  float base = circumferenceTop / circumferenceBottom;
  // TODO(Yuling): We might need to replace isRotated with rotateFeatures in the future to support
  // GORB matching in curvy images.
  return {arc, isRotated, crop, base};
}

void DetectionImageLoader::setGeometry(int cropWidth, int cropHeight, CurvySpec spec) {
  detectionImageType_ = DetectionImageType::CURVY;
  fullLabelToCroppedLabelPose_ = DetectionImageLoader::buildFullLabelToCroppedLabelPose(spec);
  curvyForTarget(cropWidth, cropHeight, spec, &curvyGeom_, &curvyFullGeom_);
  curvyFullGeom_.srcRows = cropHeight * spec.crop.outerHeight;
  curvyFullGeom_.srcCols = cropWidth * spec.crop.outerWidth;

  // In this case we do not rotate the image we pass into the GlRealityFrame (instead we rotate
  // extracted feature points), so must flip w and h here
  if (isRotated_) {
    curvyGeom_.srcRows = gl_.pyramid().levels[0].w;
    curvyGeom_.srcCols = gl_.pyramid().levels[0].h;
  } else {
    curvyGeom_.srcRows = gl_.pyramid().levels[0].h;
    curvyGeom_.srcCols = gl_.pyramid().levels[0].w;
  }
}

void DetectionImageLoader::processGpu() {
  gl_.draw(imTexture_.id(), GlRealityFrame::Options::DEFER_READ);
}

void DetectionImageLoader::readDataToCpu() {
  // Block until the image pyramid has computed processing and read its result back.
  gl_.readPixels();
}

DetectionImage DetectionImageLoader::extractFeatures() {
  FrameWithPoints fakeFrame({});
  DetectionConfig imageDetectionConfig;
  imageDetectionConfig.allOrb = true;
  imageDetectionConfig.allGorb = true;
  imageDetectionConfig.nKeypoints = 2500;
  // Compute the feature point locations.
  float gravityAngleOffset = rotateFeatures_ ? 90.0f : 0.0f;
  auto imagePoints = featureDetector_.detectAndCompute(
    gl_.pyramid(), fakeFrame, imageDetectionConfig, gravityAngleOffset);

  TargetWithPoints framePoints{k_};
  {
    // copy output.
    size_t s = imagePoints.size();
    framePoints.reserve(s);

    int width = gl_.pyramid().levels[0].w;
    for (const auto &f : imagePoints) {
      auto l = f.location();
      if (rotateFeatures_) {
        float newX = l.pt.y;
        float newY = width - 1 - l.pt.x;
        float newAngle = l.angle - 90;
        float newGravityAngle = l.gravityAngle - 90;
        framePoints.addImagePixelPoint(
          HPoint2(newX, newY), l.scale, newAngle, newGravityAngle, f.features().clone());
      } else {
        framePoints.addImagePixelPoint(
          HPoint2(l.pt.x, l.pt.y), l.scale, l.angle, l.gravityAngle, f.features().clone());
      }
    }
    framePoints.setExcludedEdgePixels(featureDetector_.edgeThreshold());
  }

  framePoints.setFeatureType(featureType_);

  if (detectionImageType_ == DetectionImageType::CURVY) {
    return DetectionImage(
      name_,
      std::move(framePoints),
      rotation_,
      curvyGeom_,
      curvyFullGeom_,
      fullLabelToCroppedLabelPose_);
  } else {
    return DetectionImage(name_, std::move(framePoints), rotation_, planarGeom_);
  }
}

}  // namespace c8
