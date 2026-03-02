// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":detection-image",
    ":detection-image-loader",
    "//c8/io:image-io",
    "//c8/geometry:intrinsics",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//reality/engine/features:gr8-feature-shader",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/imagedetection/testdata:amity-up",
    "//reality/engine/imagedetection/testdata:amity-rotated",
    "//reality/engine/imagedetection/testdata:flower-big",
  };
}
cc_end(0x1b368ae4);

#include "c8/geometry/intrinsics.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/io/image-io.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "reality/engine/features/gr8-feature-shader.h"
#include "reality/engine/imagedetection/detection-image-loader.h"
#include "reality/engine/imagedetection/detection-image.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class DetectionImageLoaderTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

static c8_PixelPinholeCameraModel testK() {
  return Intrinsics::getCameraIntrinsics(DeviceInfos::GOOGLE_PIXEL3);
}

static DetectionImage loadTestImage(
  String imagePath, ImageTargetMetadata::Reader imageTargetMetadataReader) {
  ScopeTimer rt("test");
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  auto img = readJpgToRGBA(imagePath);
  auto p = img.pixels();
  Gr8FeatureShader shader;
  shader.initialize();

  DetectionImageLoader loader;
  loader.initialize(&shader, imageTargetMetadataReader, testK());
  loader.imTexture().bind();
  loader.imTexture().setPixels(p.pixels());
  loader.imTexture().unbind();
  loader.processGpu();
  loader.readDataToCpu();

  return loader.extractFeatures();
}

static DetectionImage loadPlanarTestImage() {
  ScopeTimer rt("test");
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  auto img = readJpgToRGBA("reality/engine/imagedetection/testdata/flower-big.jpg");
  auto p = img.pixels();
  Gr8FeatureShader shader;
  shader.initialize();

  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  imageTargetMetadata.builder().setType(ImageTargetTypeMsg::PLANAR);
  imageTargetMetadata.builder().setImageWidth(p.cols());
  imageTargetMetadata.builder().setImageHeight(p.rows());

  DetectionImageLoader loader;
  loader.initialize(&shader, imageTargetMetadata.reader(), testK());
  loader.imTexture().bind();
  loader.imTexture().setPixels(p.pixels());
  loader.imTexture().unbind();
  loader.processGpu();
  loader.readDataToCpu();
  return loader.extractFeatures();
}

const int FLOWER_IMG_WIDTH = 1440;
const int FLOWER_IMG_HEIGHT = 1920;
const float CURVY_SIDE_LENGTH = 9.3f;
const float TARGET_CIRCUMFERENCE = CURVY_SIDE_LENGTH * FLOWER_IMG_WIDTH / FLOWER_IMG_HEIGHT;
const float CURVY_CIRCUMFERENCE = 21.0f;
static DetectionImage loadCroppedCurvyTestImage(
  int x, int y, int w, int h, bool isRotated = false) {
  ScopeTimer rt("test");
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  auto img = readJpgToRGBA("reality/engine/imagedetection/testdata/flower-big.jpg");
  auto p = img.pixels();
  Gr8FeatureShader shader;
  shader.initialize();

  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  auto itmBuilder = imageTargetMetadata.builder();
  itmBuilder.setType(ImageTargetTypeMsg::CURVY);
  itmBuilder.setImageWidth(p.cols());
  itmBuilder.setImageHeight(p.rows());
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceTop(CURVY_CIRCUMFERENCE);
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceBottom(CURVY_CIRCUMFERENCE);
  itmBuilder.getCurvyGeometry().setCurvySideLength(CURVY_SIDE_LENGTH);
  itmBuilder.getCurvyGeometry().setTargetCircumferenceTop(TARGET_CIRCUMFERENCE);

  itmBuilder.setOriginalImageWidth(isRotated ? FLOWER_IMG_HEIGHT : FLOWER_IMG_WIDTH);
  itmBuilder.setOriginalImageHeight(isRotated ? FLOWER_IMG_WIDTH : FLOWER_IMG_HEIGHT);
  itmBuilder.setCropOriginalImageX(x);
  itmBuilder.setCropOriginalImageY(y);
  itmBuilder.setCropOriginalImageWidth(w);
  itmBuilder.setCropOriginalImageHeight(h);

  itmBuilder.setIsRotated(isRotated);

  DetectionImageLoader loader;
  loader.initialize(&shader, imageTargetMetadata.reader(), testK());
  loader.imTexture().bind();
  loader.imTexture().setPixels(p.pixels());
  loader.imTexture().unbind();
  loader.processGpu();
  loader.readDataToCpu();
  return loader.extractFeatures();
}

static DetectionImage loadFullCurvyGeometry(int w, int h) {
  ScopeTimer rt("test");
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  auto img = readJpgToRGBA("reality/engine/imagedetection/testdata/flower-big.jpg");
  auto p = img.pixels();
  Gr8FeatureShader shader;
  shader.initialize();

  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  auto itmBuilder = imageTargetMetadata.builder();
  itmBuilder.setType(ImageTargetTypeMsg::CURVY);
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceTop(CURVY_CIRCUMFERENCE);
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceBottom(CURVY_CIRCUMFERENCE);
  float sideLength = CURVY_SIDE_LENGTH / FLOWER_IMG_HEIGHT * h;
  itmBuilder.getCurvyGeometry().setCurvySideLength(sideLength);
  itmBuilder.getCurvyGeometry().setTargetCircumferenceTop(sideLength * w / h);

  itmBuilder.setImageWidth(p.cols());
  itmBuilder.setImageHeight(p.rows());
  itmBuilder.setOriginalImageWidth(w);
  itmBuilder.setOriginalImageHeight(h);
  itmBuilder.setCropOriginalImageX(0);
  itmBuilder.setCropOriginalImageY(0);
  itmBuilder.setCropOriginalImageWidth(w);
  itmBuilder.setCropOriginalImageHeight(h);

  DetectionImageLoader loader;
  loader.initialize(&shader, imageTargetMetadata.reader(), testK());
  loader.imTexture().bind();
  loader.imTexture().setPixels(p.pixels());
  loader.imTexture().unbind();
  loader.processGpu();
  loader.readDataToCpu();
  return loader.extractFeatures();
}

// Return the full image target with no crop
static DetectionImage loadCurvyTestImage() {
  return loadCroppedCurvyTestImage(0, 0, FLOWER_IMG_WIDTH, FLOWER_IMG_HEIGHT);
}

TEST_F(DetectionImageLoaderTest, TrackLoadPlanarImage) {
  auto detectionImage = loadPlanarTestImage();
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);
}

TEST_F(DetectionImageLoaderTest, TrackLoadPlanarImageAgain) {
  auto detectionImage = loadPlanarTestImage();
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);
}

TEST_F(DetectionImageLoaderTest, TrackLoadCurvyImage) {
  auto detectionImage = loadCurvyTestImage();
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  auto curvyGeometry = detectionImage.getGeometry();

  EXPECT_FLOAT_EQ(curvyGeometry.height, 1.f);
  EXPECT_NEAR(curvyGeometry.radius, 0.359382f, 1e-6);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.top, 0.f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.bottom, 1.f);
  EXPECT_NEAR(curvyGeometry.activationRegion.left, 0.333928f, 1e-6);
  EXPECT_NEAR(curvyGeometry.activationRegion.right, 0.666071, 1e-6);

  EXPECT_EQ(curvyGeometry.srcCols, 480) << "geometry of source is pyramid in portrait";
  EXPECT_EQ(curvyGeometry.srcRows, 640) << "geometry of source is pyramid in portrait";
}

TEST_F(DetectionImageLoaderTest, TrackLoadCurvyImageCropped) {
  // Got these numbers by opening up GIMP, then select a 3x4 selection
  auto detectionImage = loadCroppedCurvyTestImage(230, 62, 1065, 1420);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  auto curvyGeometry = detectionImage.getGeometry();

  EXPECT_NEAR(curvyGeometry.height, 1.f, 1e-6) << "internal curvy geometry in the engine is 1.0";
  // Since the cropped curvy has to have height of 1.0, its radius grows as a ratio of
  // originalHeight to croppedHeight
  EXPECT_NEAR(curvyGeometry.radius, 0.485925f, 1e-6);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.top, 0.032291669f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.bottom, 0.77187502f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.left, 0.38697919f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.right, 0.63262653f);

  EXPECT_EQ(curvyGeometry.srcCols, 480) << "geometry of source is pyramid in portrait";
  EXPECT_EQ(curvyGeometry.srcRows, 640) << "geometry of source is pyramid in portrait";
}

TEST_F(DetectionImageLoaderTest, TrackLoadCurvyImageCroppedSameAsFull) {
  // Loading an image cropped to a specific dimension should result in the same geometry
  // as loading the cropped image of that dimension if the cropped is centered in X
  int croppedHeight = 1200;
  auto detectionImage = loadCroppedCurvyTestImage(270, 62, 900, croppedHeight);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);
  auto croppedGeometry = detectionImage.getGeometry();
  auto fullGeometry = loadFullCurvyGeometry(900, croppedHeight).getGeometry();

  EXPECT_NEAR(croppedGeometry.height, fullGeometry.height, 1e-6)
    << "internal curvy geometry in the engine is 1.0";
  EXPECT_NEAR(croppedGeometry.radius, fullGeometry.radius, 1e-6);
  EXPECT_FLOAT_EQ(croppedGeometry.activationRegion.top, 0.032291669f);
  EXPECT_FLOAT_EQ(croppedGeometry.activationRegion.bottom, 0.65729171f);
  EXPECT_FLOAT_EQ(croppedGeometry.activationRegion.left, fullGeometry.activationRegion.left);
  EXPECT_FLOAT_EQ(croppedGeometry.activationRegion.right, fullGeometry.activationRegion.right);

  EXPECT_EQ(croppedGeometry.srcCols, 480) << "geometry of source is pyramid in portrait";
  EXPECT_EQ(croppedGeometry.srcRows, 640) << "geometry of source is pyramid in portrait";
}

TEST_F(DetectionImageLoaderTest, CenterCropIsNoTransformation) {
  int croppedWidth = 900;
  int croppedHeight = 1200;
  int shiftX = (FLOWER_IMG_WIDTH - croppedWidth) / 2;
  int shiftY = (FLOWER_IMG_HEIGHT - croppedHeight) / 2;
  auto detectionImage = loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // Since the crop is at the center of the image target, there is no transformation between
  // the coordinate of the cropped curvy and the full curvy
  EXPECT_THAT(
    HMatrixGen::i().data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, CenterCropIsNoTransformationRotated) {
  int croppedWidth = 480;
  int croppedHeight = 640;
  int shiftX = (FLOWER_IMG_HEIGHT - croppedWidth) / 2;
  int shiftY = (FLOWER_IMG_WIDTH - croppedHeight) / 2;
  auto detectionImage =
    loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight, true);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // Since the crop is at the center of the image target, there is no transformation between
  // the coordinate of the cropped curvy and the full curvy
  EXPECT_THAT(
    HMatrixGen::i().data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, MaxRotationNoShift) {
  // we are doing a left crop of this image
  int croppedWidth = FLOWER_IMG_WIDTH / 2;
  int croppedHeight = FLOWER_IMG_HEIGHT / 2;
  int shiftX = 0;
  int shiftY = (FLOWER_IMG_HEIGHT - croppedHeight) / 2;
  auto detectionImage = loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // if the label has been fully wrapped around the curvy, the left of the label
  // would be exactly -90 degree. Instead, it is linearly scaled by the target label size
  HMatrix rotation = HMatrixGen::rotationD(0, -90 * TARGET_CIRCUMFERENCE / CURVY_CIRCUMFERENCE, 0);
  EXPECT_THAT(rotation.data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, MaxRotationNoShiftRotated) {
  // we are doing a top crop of this image. When rotated back counter clock-wise, it is like a left
  // crop of the image.
  int croppedWidth = FLOWER_IMG_HEIGHT / 2;
  int croppedHeight = FLOWER_IMG_WIDTH / 2;
  int shiftX = (FLOWER_IMG_HEIGHT - croppedWidth) / 2;
  int shiftY = 0;
  auto detectionImage =
    loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight, true);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // if the label has been fully wrapped around the curvy, the left of the label
  // would be exactly -90 degree. Instead, it is linearly scaled by the target label size
  HMatrix rotation = HMatrixGen::rotationD(0, -90 * TARGET_CIRCUMFERENCE / CURVY_CIRCUMFERENCE, 0);
  EXPECT_THAT(rotation.data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, RotatedMaxRotationIsMaxShift) {
  // we are doing a left crop of this image. But since it is a rotated image, the pose diff is just
  // a translation
  int croppedWidth = FLOWER_IMG_HEIGHT / 2;
  int croppedHeight = FLOWER_IMG_WIDTH / 2;
  int shiftX = 0;
  int shiftY = (FLOWER_IMG_WIDTH - croppedHeight) / 2;
  auto detectionImage =
    loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight, true);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // Since we crop out the left, the cropped label is shifted downward by half (when rotated
  // counter-clockwise)
  HMatrix translation = HMatrixGen::translation(0, -0.5, 0);
  EXPECT_THAT(translation.data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, RotationAndShift) {
  // we are doing a bottom right crop of this image
  int croppedWidth = FLOWER_IMG_WIDTH / 2;
  int croppedHeight = FLOWER_IMG_HEIGHT / 2;
  int shiftX = FLOWER_IMG_WIDTH / 2;
  int shiftY = FLOWER_IMG_HEIGHT / 2;
  auto detectionImage = loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  // see MaxRotationNoShift unit test for explanation
  HMatrix rotation = HMatrixGen::rotationD(0, 90 * TARGET_CIRCUMFERENCE / CURVY_CIRCUMFERENCE, 0);
  // Since we crop out the bottom right, the cropped label is shifted downward by half
  HMatrix translation = HMatrixGen::translation(0, -0.5, 0);
  EXPECT_THAT(
    (translation * rotation).data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, RotationAndShiftRotated) {
  // we are doing bottom right crop of the rotated image
  int croppedWidth = FLOWER_IMG_HEIGHT / 2;
  int croppedHeight = FLOWER_IMG_WIDTH / 2;
  int shiftX = FLOWER_IMG_HEIGHT / 2;
  int shiftY = FLOWER_IMG_WIDTH / 2;
  auto detectionImage =
    loadCroppedCurvyTestImage(shiftX, shiftY, croppedWidth, croppedHeight, true);
  EXPECT_GT(detectionImage.framePoints().points().size(), 1000);

  HMatrix rotation = HMatrixGen::rotationD(0, 90 * TARGET_CIRCUMFERENCE / CURVY_CIRCUMFERENCE, 0);
  // Since we crop out the bottom right, the cropped label is shifted upward by half
  HMatrix translation = HMatrixGen::translation(0, 0.5, 0);
  EXPECT_THAT(
    (translation * rotation).data(), equalsMatrix(detectionImage.getFullLabelToCroppedLabelPose()));
}

TEST_F(DetectionImageLoaderTest, DetectionImageIntrinsicIsRotatedForRotatedInput) {
  ScopeTimer rt("test");

  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  ImageTargetMetadata::Builder itmBuilder = imageTargetMetadata.builder();
  String imagePath = "reality/engine/imagedetection/testdata/amity_up.jpg";
  auto img = readJpgToRGBA(imagePath);
  auto p = img.pixels();
  itmBuilder.setType(ImageTargetTypeMsg::PLANAR);
  itmBuilder.setImageWidth(p.cols());
  itmBuilder.setImageHeight(p.rows());
  itmBuilder.setOriginalImageWidth(1092);
  itmBuilder.setOriginalImageHeight(1278);
  itmBuilder.setCropOriginalImageX(68);
  itmBuilder.setCropOriginalImageY(0);
  itmBuilder.setCropOriginalImageWidth(959);
  itmBuilder.setCropOriginalImageHeight(1278);

  // Unrotated
  itmBuilder.setIsRotated(false);
  DetectionImage detectionImage = loadTestImage(imagePath, imageTargetMetadata.reader());
  c8_PixelPinholeCameraModel intrinsic = detectionImage.framePoints().intrinsic();

  // Rotated
  itmBuilder.setIsRotated(true);
  DetectionImage detectionImageRotated = loadTestImage(imagePath, imageTargetMetadata.reader());
  c8_PixelPinholeCameraModel intrinsicRotated = detectionImageRotated.framePoints().intrinsic();

  EXPECT_EQ(intrinsic.pixelsWidth, intrinsicRotated.pixelsHeight);
  EXPECT_EQ(intrinsic.pixelsHeight, intrinsicRotated.pixelsWidth);
  EXPECT_EQ(intrinsic.centerPointX, intrinsicRotated.centerPointY);
  EXPECT_EQ(intrinsic.centerPointY, intrinsicRotated.centerPointX);
}

TEST_F(DetectionImageLoaderTest, DetectionImageCurvyGeometryIsCorrectForRotatedInput) {
  ScopeTimer rt("test");
  String imagePath = "reality/engine/imagedetection/testdata/amity_rotated.jpg";
  auto img = readJpgToRGBA(imagePath);
  auto p = img.pixels();
  MutableRootMessage<ImageTargetMetadata> imageTargetMetadata;
  auto itmBuilder = imageTargetMetadata.builder();
  itmBuilder.setType(ImageTargetTypeMsg::CURVY);
  itmBuilder.setImageWidth(p.cols());
  itmBuilder.setImageHeight(p.rows());
  int ORIGINAL_WIDTH = 1278;
  int ORIGINAL_HEIGHT = 1092;
  int CROP_WIDTH = 819;
  int CROP_HEIGHT = ORIGINAL_HEIGHT;
  itmBuilder.setOriginalImageWidth(ORIGINAL_WIDTH);
  itmBuilder.setOriginalImageHeight(ORIGINAL_HEIGHT);
  itmBuilder.setCropOriginalImageX(236);
  itmBuilder.setCropOriginalImageY(0);
  itmBuilder.setCropOriginalImageWidth(CROP_WIDTH);
  itmBuilder.setCropOriginalImageHeight(CROP_HEIGHT);

  float SIDE_LENGTH = 12.9f;
  float TARGET_CIRCUMFERENCE = SIDE_LENGTH * ORIGINAL_HEIGHT / ORIGINAL_WIDTH;
  float CIRCUMFERENCE = 27.3f;
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceTop(CIRCUMFERENCE);
  itmBuilder.getCurvyGeometry().setCurvyCircumferenceBottom(CIRCUMFERENCE);
  itmBuilder.getCurvyGeometry().setCurvySideLength(SIDE_LENGTH);
  // the target circumference is measured, and thus not in width / height aspect ratio
  // but on height / width aspect ratio
  itmBuilder.getCurvyGeometry().setTargetCircumferenceTop(TARGET_CIRCUMFERENCE);
  itmBuilder.setIsRotated(true);

  DetectionImage detectionImageRotated = loadTestImage(imagePath, imageTargetMetadata.reader());
  CurvyImageGeometry curvyGeometry = detectionImageRotated.getGeometry();

  // Have to swap width and height because the image is rotated
  float originalWidth = static_cast<float>(imageTargetMetadata.reader().getOriginalImageHeight());
  float originalHeight = static_cast<float>(imageTargetMetadata.reader().getOriginalImageWidth());
  float cropWidth = static_cast<float>(imageTargetMetadata.reader().getCropOriginalImageHeight());
  float cropHeight = static_cast<float>(imageTargetMetadata.reader().getCropOriginalImageWidth());

  // The radius in the engine is the curvy's radius divided by sideLength.
  // A cropped image target corresponds to a curvy with the engine full radius scaled by the
  // cropped width and height.
  // croppedRadius = engineFullRadius * (cropHeight/originalHeight) / (cropWidth/originalWidth)
  // 0.525581 = ((27.3 / 2 / pi) / 12.9) * (cropHeight/originalHeight) / (cropWidth/originalWidth)
  float fullRadius = itmBuilder.getCurvyGeometry().getCurvyCircumferenceTop() / (2 * M_PI);
  float engineFullRadius = fullRadius / SIDE_LENGTH;
  float fullRadiusToCroppedRadiusRatio =
    (cropHeight / originalHeight) / (cropWidth / originalWidth);
  float croppedRadius = engineFullRadius / fullRadiusToCroppedRadiusRatio;
  EXPECT_NEAR(curvyGeometry.radius, croppedRadius, 1e-6);
  EXPECT_NEAR(curvyGeometry.radius, 0.525581, 1e-6);

  EXPECT_FLOAT_EQ(curvyGeometry.height, 1.f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.top, 0.17449139f);
  EXPECT_FLOAT_EQ(curvyGeometry.activationRegion.bottom, 0.81533647f);
  float left = 0.5f - TARGET_CIRCUMFERENCE / CIRCUMFERENCE / 2;
  float right = 0.5f + TARGET_CIRCUMFERENCE / CIRCUMFERENCE / 2;
  EXPECT_NEAR(curvyGeometry.activationRegion.left, left, 1e-6);
  EXPECT_NEAR(curvyGeometry.activationRegion.right, right, 1e-6);

  EXPECT_EQ(curvyGeometry.srcCols, 640) << "geometry source is set to pyramid level in landscape";
  EXPECT_EQ(curvyGeometry.srcRows, 480) << "geometry source is set to pyramid level in landscape";
}

}  // namespace c8
