// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "intrinsics.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8/camera:device-infos",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//reality/engine/api:reality.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x9d46171c);

#include <cctype>

#include "c8/geometry/intrinsics.h"

namespace c8 {

HMatrix Intrinsics::unitTestCamera() {
  HMatrix matrix{
    {5.0f, 0.0f, 3.0f, 0.0f},
    {0.0f, -5.0f, 2.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  return matrix;
}

HMatrix Intrinsics::logitechQuickCamPro9000() {
  HMatrix matrix{
    {543.0f, 0.0f, 319.5f, 0.0f},
    {0.0f, -543.0f, 239.5f, 0.0f},
    {0.0f, 0.0000f, 1.000f, 0.0f},
    {0.0f, 0.0000f, 0.000f, 1.0f}};
  return matrix;
}

HMatrix Intrinsics::iPhone7RearPortrait() {
  HMatrix matrix{
    {526.365f, 0.000000f, 238.5960f, 0.0f},
    {0.000000f, -526.365f, 319.1485f, 0.0f},
    {0.000000f, 0.000000f, 1.000000f, 0.0f},
    {0.000000f, 0.000000f, 0.000000f, 1.0f}};
  return matrix;
}

HMatrix Intrinsics::galaxyS6RearLandscape() {
  HMatrix matrix{
    {649.0f, 0.0f, 319.5f, 0.0f},
    {0.0f, -649.0f, 239.5f, 0.0f},
    {0.0f, 0.0000f, 1.000f, 0.0f},
    {0.0f, 0.0000f, 0.000f, 1.0f}};
  return matrix;
}

HMatrix Intrinsics::galaxyS6RearPortrait() {
  HMatrix matrix{
    {649.0f, 0.0f, 239.5f, 0.0f},
    {0.0f, -649.0f, 319.5f, 0.0f},
    {0.0f, 0.0000f, 1.000f, 0.0f},
    {0.0f, 0.0000f, 0.000f, 1.0f}};
  return matrix;
}

HMatrix Intrinsics::galaxyS7RearLandscape() {
  HMatrix matrix{
    {511.0f, 0.0f, 319.5f, 0.0f},
    {0.0f, -511.0f, 239.5f, 0.0f},
    {0.0f, 0.0000f, 1.000f, 0.0f},
    {0.0f, 0.0000f, 0.000f, 1.0f}};
  return matrix;
}

void Intrinsics::toClipSpace(
  const c8_PixelPinholeCameraModel intrinsics,
  float kn,
  float kf,
  float *colMajorProjMat,
  float *invColMajorProjMat) {

  float c00 = 2 * intrinsics.focalLengthHorizontal / intrinsics.pixelsWidth;
  float c11 = 2 * intrinsics.focalLengthVertical / intrinsics.pixelsHeight;

  float c02 = 1.0f - (2.0f * intrinsics.centerPointX + 1.0f) / intrinsics.pixelsWidth;
  float c12 = -1.0f + (2.0f * intrinsics.centerPointY + 1.0f) / intrinsics.pixelsHeight;

  float c22 = (kf + kn) / (kn - kf);
  float c23 = 2 * kf * kn / (kn - kf);
  float c32 = -1;

  std::fill(colMajorProjMat, colMajorProjMat + 16, 0.0f);

  colMajorProjMat[0] = c00;
  colMajorProjMat[5] = c11;
  colMajorProjMat[8] = c02;
  colMajorProjMat[9] = c12;
  colMajorProjMat[10] = c22;
  colMajorProjMat[11] = c32;
  colMajorProjMat[14] = c23;

  if (invColMajorProjMat != nullptr) {
    std::fill(invColMajorProjMat, invColMajorProjMat + 16, 0.0f);
    invColMajorProjMat[0] = 1.0f / c00;
    invColMajorProjMat[5] = 1.0f / c11;
    invColMajorProjMat[11] = 1.0f / c23;
    invColMajorProjMat[12] = c02 / c00;
    invColMajorProjMat[13] = c12 / c11;
    invColMajorProjMat[14] = -1.0f;
    invColMajorProjMat[15] = c22 / c23;
  }
}

HMatrix Intrinsics::toClipSpaceMat(
  const c8_PixelPinholeCameraModel intrinsics, float nearClip, float farClip) {
  std::array<float, 16> mat;
  std::array<float, 16> matInv;
  Intrinsics::toClipSpace(intrinsics, nearClip, farClip, mat.data(), matInv.data());

  return {mat.data(), matInv.data()};
}

void Intrinsics::toClipSpaceLeftHanded(
  const c8_PixelPinholeCameraModel intrinsics,
  float kn,
  float kf,
  float *colMajorProjMat,
  float *invColMajorProjMat) {

  float c00 = 2 * intrinsics.focalLengthHorizontal / intrinsics.pixelsWidth;
  float c11 = 2 * intrinsics.focalLengthVertical / intrinsics.pixelsHeight;

  float c02 = 1.0f - (2.0f * intrinsics.centerPointX + 1.0f) / intrinsics.pixelsWidth;
  float c12 = -1.0f + (2.0f * intrinsics.centerPointY + 1.0f) / intrinsics.pixelsHeight;

  float c22 = (kf + kn) / (kf - kn);
  float c23 = -(2 * kf * kn) / (kf - kn);
  float c32 = 1.0f;

  std::fill(colMajorProjMat, colMajorProjMat + 16, 0.0f);

  colMajorProjMat[0] = c00;
  colMajorProjMat[5] = c11;
  colMajorProjMat[8] = c02;
  colMajorProjMat[9] = c12;
  colMajorProjMat[10] = c22;
  colMajorProjMat[11] = c32;
  colMajorProjMat[14] = c23;

  if (invColMajorProjMat != nullptr) {
    std::fill(invColMajorProjMat, invColMajorProjMat + 16, 0.0f);
    invColMajorProjMat[0] = 1.0f / c00;
    invColMajorProjMat[5] = 1.0f / c11;
    invColMajorProjMat[11] = 1.0f / c23;
    invColMajorProjMat[12] = -c02 / c00;
    invColMajorProjMat[13] = -c12 / c11;
    invColMajorProjMat[14] = 1.0f;
    invColMajorProjMat[15] = -c22 / c23;
  }
}

c8_PixelPinholeCameraModel Intrinsics::rotateCropAndScaleIntrinsics(
  const c8_PixelPinholeCameraModel intrinsics, int width, int height) {
  auto k = intrinsics;
  // If the camera feed is rotated with respect to our model, swap x/y.
  if ((width > height) != (k.pixelsWidth > k.pixelsHeight)) {
    k = {
      k.pixelsHeight,           // width
      k.pixelsWidth,            // height
      k.centerPointY,           // centerPointX
      k.centerPointX,           // centerPointY
      k.focalLengthVertical,    // focalLengthHorizontal
      k.focalLengthHorizontal,  // focalLengthVertical
    };
  }

  return Intrinsics::cropAndScaleIntrinsics(k, width, height);
}

HMatrix Intrinsics::toClipSpaceMatLeftHanded(
  const c8_PixelPinholeCameraModel intrinsics, float nearClip, float farClip) {
  std::array<float, 16> mat;
  std::array<float, 16> matInv;
  Intrinsics::toClipSpaceLeftHanded(intrinsics, nearClip, farClip, mat.data(), matInv.data());

  return {mat.data(), matInv.data()};
}

HMatrix Intrinsics::orthographicProjectionRightHanded(
  float xLeft, float xRight, float yUp, float yDown, float zNear, float zFar) noexcept {
  float xScale = 2 / (xRight - xLeft);
  float yScale = 2 / (yUp - yDown);
  float zScale = - 2 / (zFar - zNear);
  float tx = -(xRight + xLeft) / (xRight - xLeft);
  float ty = -(yUp + yDown) / (yUp - yDown);
  float tz = -(zFar + zNear) / (zFar - zNear);

  float xScInv = 1 / xScale;
  float yScInv = 1 / yScale;
  float zScInv = 1 / zScale;
  float ix = -xScInv * tx;
  float iy = -yScInv * ty;
  float iz = -zScInv * tz;

  return HMatrix{
    {xScale, 0.0f, 0.0f, tx},
    {0.0f, yScale, 0.0f, ty},
    {0.0f, 0.0f, zScale, tz},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {xScInv, 0.0f, 0.0f, ix},
    {0.0f, yScInv, 0.0f, iy},
    {0.0f, 0.0f, zScInv, iz},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };
}

HMatrix Intrinsics::perspectiveProjectionRightHanded(
  float fovRadius, float aspectRatio, float nearClip, float farClip) noexcept {
  float f = 1.0f / std::tan(0.5f * fovRadius);
  auto fova = f / aspectRatio;
  auto zonf = -(farClip + nearClip) / (farClip - nearClip);
  auto znf2 = -2.0f * farClip * nearClip / (farClip - nearClip);
  auto finv = 1.0f / f;
  auto infa = 1.0f / fova;
  auto izf2 = 1.0f / znf2;
  auto zoz2 = zonf / znf2;
  return HMatrix{
    {fova, 0.00f, 0.0f, 0.0f},
    {0.00000f, f, 0.0f, 0.0f},
    {0.0f, 0.00f, zonf, znf2},
    {0.0f, 0.0f, -1.0f, 0.0f},
    {infa, 0.00f, 0.0f, 0.0f},
    {0.00f, finv, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, -1.0f},
    {0.00f, 0.0f, izf2, zoz2},
  };
}

c8_PixelPinholeCameraModel Intrinsics::cropAndScaleIntrinsics(
  const c8_PixelPinholeCameraModel intrinsics, const int width, const int height) {
  float scale = std::max(
    static_cast<float>(width) / intrinsics.pixelsWidth,
    static_cast<float>(height) / intrinsics.pixelsHeight);

  float centerX =
    (width - 1) / 2.0f + (intrinsics.centerPointX - (intrinsics.pixelsWidth - 1) / 2.0f) * scale;
  float centerY =
    (height - 1) / 2.0f + (intrinsics.centerPointY - (intrinsics.pixelsHeight - 1) / 2.0f) * scale;

  return {
    width,
    height,
    centerX,
    centerY,
    intrinsics.focalLengthHorizontal * scale,
    intrinsics.focalLengthVertical * scale};
}

c8_PixelPinholeCameraModel Intrinsics::getProcessingIntrinsics(
  DeviceInfos::DeviceModel model, int width, int height) {
  return cropAndScaleIntrinsics(getCameraIntrinsics(model), width, height);
}

c8_PixelPinholeCameraModel Intrinsics::getCaptureIntrinsics(
  c8_PixelPinholeCameraModel fullFrameIntrinsics, CameraCaptureGeometry::Reader captureGeometry) {
  return cropAndScaleIntrinsics(
    fullFrameIntrinsics, captureGeometry.getWidth(), captureGeometry.getHeight());
}
c8_PixelPinholeCameraModel Intrinsics::getProcessingIntrinsics(
  c8_PixelPinholeCameraModel captureIntrinsics, CameraFrame::Reader processingFrame) {
  if (processingFrame.hasPyramid() && processingFrame.getPyramid().getLevels().size() > 0) {
    auto levelInfo = processingFrame.getPyramid().getLevels()[0];
    return cropAndScaleIntrinsics(captureIntrinsics, levelInfo.getW(), levelInfo.getH());
  }
  auto grayImagePointer = processingFrame.getImage().getOneOf().getGrayImagePointer();
  return cropAndScaleIntrinsics(
    captureIntrinsics, grayImagePointer.getCols(), grayImagePointer.getRows());
}
c8_PixelPinholeCameraModel Intrinsics::getDisplayIntrinsics(
  c8_PixelPinholeCameraModel captureIntrinsics, GraphicsPinholeCameraModel::Reader displayModel) {
  return cropAndScaleIntrinsics(
    captureIntrinsics, displayModel.getTextureWidth(), displayModel.getTextureHeight());
}

c8_PixelPinholeCameraModel Intrinsics::getFullFrameIntrinsics(
  const RequestCamera::Reader cameraReader, const DeviceInfo::Reader &model) {
  auto ri = cameraReader.getPixelIntrinsics();
  if (ri.getFocalLengthHorizontal() != 0) {
    return c8_PixelPinholeCameraModel{
      ri.getPixelsWidth(),
      ri.getPixelsHeight(),
      ri.getCenterPointX(),
      ri.getCenterPointY(),
      ri.getFocalLengthHorizontal(),
      ri.getFocalLengthVertical()};
  }
  auto infosModel = DeviceInfos::getDeviceModel(model);
  if (infosModel != DeviceInfos::NOT_SPECIFIED) {
    return getCameraIntrinsics(infosModel);
  } else {
    return getFallbackCameraIntrinsics(model);
  }
}

c8_PixelPinholeCameraModel Intrinsics::getCaptureIntrinsics(
  const RequestCamera::Reader cameraReader,
  const DeviceInfo::Reader &model,
  CameraCaptureGeometry::Reader captureGeometry) {
  return getCaptureIntrinsics(getFullFrameIntrinsics(cameraReader, model), captureGeometry);
}

c8_PixelPinholeCameraModel Intrinsics::getProcessingIntrinsics(
  const RequestCamera::Reader cameraReader,
  const DeviceInfo::Reader &model,
  CameraCaptureGeometry::Reader captureGeometry,
  CameraFrame::Reader processingFrame) {
  return getProcessingIntrinsics(
    getCaptureIntrinsics(cameraReader, model, captureGeometry), processingFrame);
}

c8_PixelPinholeCameraModel Intrinsics::getDisplayIntrinsics(
  const RequestCamera::Reader cameraReader,
  const DeviceInfo::Reader &model,
  CameraCaptureGeometry::Reader captureGeometry,
  GraphicsPinholeCameraModel::Reader displayModel) {
  return getDisplayIntrinsics(
    getCaptureIntrinsics(cameraReader, model, captureGeometry), displayModel);
}

c8_PixelPinholeCameraModel Intrinsics::getFullFrameIntrinsics(
  const RealityRequest::Reader &request) {
  return getFullFrameIntrinsics(request.getSensors().getCamera(), request.getDeviceInfo());
}

c8_PixelPinholeCameraModel Intrinsics::getCaptureIntrinsics(const RealityRequest::Reader &request) {
  return getCaptureIntrinsics(
    getFullFrameIntrinsics(request),
    request.getXRConfiguration().getCameraConfiguration().getCaptureGeometry());
}

c8_PixelPinholeCameraModel Intrinsics::getProcessingIntrinsics(
  const RealityRequest::Reader &request) {
  return getProcessingIntrinsics(
    getCaptureIntrinsics(request), request.getSensors().getCamera().getCurrentFrame());
}

c8_PixelPinholeCameraModel Intrinsics::getDisplayIntrinsics(const RealityRequest::Reader &request) {
  return getDisplayIntrinsics(
    getCaptureIntrinsics(request), request.getXRConfiguration().getGraphicsIntrinsics());
}

c8_PixelPinholeCameraModel makeIntrinsics(float focalLength) {
  return c8_PixelPinholeCameraModel{480, 640, 239.5f, 319.5f, focalLength, focalLength};
}

#define MODEL_TO_INTRINSIC(deviceInfoEnum, focalLength) \
  case DeviceInfos::deviceInfoEnum:                     \
    return makeIntrinsics(focalLength);

c8_PixelPinholeCameraModel Intrinsics::getCameraIntrinsics(const DeviceInfos::DeviceModel &model) {
  c8_PixelPinholeCameraModel intrinsic;
  switch (model) {
    MODEL_TO_INTRINSIC(APPLE_IPAD_G3, 603.0738f)
    MODEL_TO_INTRINSIC(APPLE_IPAD_G5, 577.8471f)
    MODEL_TO_INTRINSIC(APPLE_IPAD_G6, 575.7200f)
    MODEL_TO_INTRINSIC(APPLE_IPAD_PRO12_G2, 522.5444f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_5C, 584.3393f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_5S, 574.4124f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_6, 562.1421f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_6PLUS, 536.9322f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_6S, 530.9312f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_SE, 546.5731f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_7, 538.0105f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_7PLUS, 525.9533f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_8, 510.5460f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_8PLUS, 510.7869f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_X, 506.4104f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_XS, 485.9624f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_XS_MAX, 489.6052f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_XR, 488.1437f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_11, 481.7698f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_11_PRO, 498.2050f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_12_MINI, 484.6808f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_12, 474.6663f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_12_PRO, 489.1995f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_12_PRO_MAX, 496.f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_13, 477.786f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_13_PRO, 486.193f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_13_PRO_MAX, 483.227f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_13_MINI, 485.243f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_14, 489.5614f)
    // uncalibrated, use APPLE_IPHONE_14_PLUS as a fallback.
    MODEL_TO_INTRINSIC(APPLE_IPHONE_14_PLUS, 489.5614f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_14_PRO, 449.4042f)
    MODEL_TO_INTRINSIC(APPLE_IPHONE_14_PRO_MAX, 459.4768f)
    MODEL_TO_INTRINSIC(ASUS_ZENPHONE2, 546.6852f)
    MODEL_TO_INTRINSIC(EPSON_MOVERIO_BT300, 486.7557f)
    MODEL_TO_INTRINSIC(ESSENTIAL_PH_1, 927.7996f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL, 491.7793f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL_XL, 499.2098f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL2, 499.2760f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL2_XL, 490.7689f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL3, 519.9731f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL3_XL, 514.4164f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL4, 511.2248f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL5, 672.6488f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL5A, 677.5731f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL5_XL, 672.6488f)
    // uncalibrated, use GOOGLE_PIXEL5 as a fallback.
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL6, 695.8949f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL6_PRO, 701.1237f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL7, 521.8225f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL7_PRO, 521.6449f)
    MODEL_TO_INTRINSIC(GOOGLE_PIXEL4_XL, 514.8140f)
    MODEL_TO_INTRINSIC(HIP_H450R, 608.5653f)
    MODEL_TO_INTRINSIC(HTC_ONE_M8, 611.1558f)
    MODEL_TO_INTRINSIC(HUAWEI_HONOR_8, 498.8813f)
    MODEL_TO_INTRINSIC(HUAWEI_HONOR_9, 512.9015f)
    MODEL_TO_INTRINSIC(HUAWEI_MATE_9, 521.2284f)
    MODEL_TO_INTRINSIC(HUAWEI_MATE_10, 531.8810f)
    MODEL_TO_INTRINSIC(HUAWEI_NEXUS_6P, 502.3116f)
    MODEL_TO_INTRINSIC(HUAWEI_P20, 488.2982f)
    MODEL_TO_INTRINSIC(HUAWEI_P20_LITE, 488.6114f)
    MODEL_TO_INTRINSIC(HUAWEI_P30, 514.1215f)
    MODEL_TO_INTRINSIC(LENOVO_PB2_690Y, 619.0314f)
    MODEL_TO_INTRINSIC(LENOVO_K6_NOTE, 506.3837f)
    MODEL_TO_INTRINSIC(LGE_NEXUS_4, 581.1556f)
    MODEL_TO_INTRINSIC(LGE_NEXUS_5, 559.9671f)
    MODEL_TO_INTRINSIC(LGE_NEXUS_5X, 479.3516f)
    MODEL_TO_INTRINSIC(MOTOROLA_NEXUS_6, 538.1735f)
    MODEL_TO_INTRINSIC(MOTOROLA_MOTOG3, 492.4324f)
    MODEL_TO_INTRINSIC(MOTOROLA_EDGE_PLUS, 643.2200f)
    MODEL_TO_INTRINSIC(MOTOROLA_RAZR_PLUS, 481.1333f)
    MODEL_TO_INTRINSIC(ODG_R7, 567.6768f)
    MODEL_TO_INTRINSIC(ONEPLUS_9_5G, 456.0290f)
    MODEL_TO_INTRINSIC(ONEPLUS_9_PRO, 462.9853f)
    MODEL_TO_INTRINSIC(ONEPLUS_NORD10_5G, 443.7300f)
    MODEL_TO_INTRINSIC(ONEPLUS_8T, 387.9355f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_CORE_PRIME, 575.0482f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_J7, 690.9029f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S5, 723.0940f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S6, 625.4912f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S6_EDGE, 649.8869f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S7, 491.4958f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S7_EDGE, 511.5746f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S8, 508.2721f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S8_PLUS, 508.1794f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S9, 499.9801f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S9_PLUS, 499.2632f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S10, 492.1022f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S10e, 337.2781f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S10_PLUS, 494.8447f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_NOTE8, 645.3651f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_NOTE9, 672.9767f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_NOTE10_PLUS, 485.4488f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S21_5G, 488.1016f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S21_ULTRA_5G, 465.0067f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_S22_PLUS, 439.8777f)
    MODEL_TO_INTRINSIC(SAMSUNG_ZFOLD_3, 412.0340f)
    MODEL_TO_INTRINSIC(SAMSUNG_ZFLIP_3, 498.8212f)
    MODEL_TO_INTRINSIC(SAMSUNG_GALAXY_TAB_A, 512.0151f)
    MODEL_TO_INTRINSIC(XIAOMI_MIX_2, 493.5102f)
    MODEL_TO_INTRINSIC(XIAOMI_REDMI_4X, 561.6292f)
    // DeviceInfos::APPLE_IPHONE_X
    default:
      return makeIntrinsics(506.4104f);
  }
  return intrinsic;
}

c8_PixelPinholeCameraModel Intrinsics::getFallbackCameraIntrinsics(
  const DeviceInfo::Reader &deviceInfoReader) {
  String model = toUpperCase(deviceInfoReader.getModel());
  if (model == "IPHONE 12/12PRO") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_12);
  } else if (model == "IPHONE XSMAX/11PROMAX") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS_MAX);
  } else if (model == "IPHONE X/XS/11PRO") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_X);
  } else if (model == "IPHONE XR/11") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XR);
  } else if (model == "IPHONE 6+/6S+/7+/8+") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6PLUS);
  } else if (model == "IPHONE 6/6S/7/8") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6);
  } else if (model == "IPHONE 5/5S/5C/SE") {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_5);
  } else {
    return getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_X);
  }
}

c8_PixelPinholeCameraModel Intrinsics::fullFrameFocalLengthMm(float focalLengthMm) {
  float focalLength = focalLengthMm * 8192.0f / 36.0f;
  return c8_PixelPinholeCameraModel{8192, 5461, 4095.5f, 2730.0f, focalLength, focalLength};
}

}  // namespace c8
