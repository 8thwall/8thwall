// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#ifdef JAVASCRIPT

#include <emscripten.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <deque>
#include <memory>

#include "c8/geometry/intrinsics.h"
#include "c8/io/image-io.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/xr-requests.h"
#include "c8/symbol-visibility.h"
#include "reality/engine/executor/xr-engine.h"
#include "reality/engine/features/gl-reality-frame.h"
#include "reality/engine/imagedetection/detection-image-loader.h"
#include "reality/engine/imagedetection/detection-image.h"

using namespace c8;

namespace {

struct Orientation {
  float alpha;
  float beta;
  float gamma;
};

struct PipelineData {
  uint32_t realityTextureId = 0;
  Quaternion devicePose{1, 0, 0, 0};
  Orientation deviceWebOrientation;
  ConstRootMessage<RealityResponse> realityResponse;
  ConstRootMessage<LogRecord> logRecord;
  int captureWidth = 0;
  int captureHeight = 0;
  int captureRotation = 0;
  // The video playback time.
  int64_t videoTimeNanos = 0;
  // The time when the frame was read.
  int64_t frameTimeNanos = 0;
  // The time when the frame was staged.
  int64_t timeNanos = 0;
  ConstRootMessage<RequestPose> eventQueue;

  double latitude = 0.0;
  double longitude = 0.0;
  // The accuracy, with a 95% confidence level, of the latitude and longitude properties expressed
  // in meters: https://developer.mozilla.org/en-US/docs/Web/API/GeolocationCoordinates/accuracy
  double accuracy = 0.0;
};

// If you need to add a field here that needs to eventually be on PipelineData, just add it directly
// to the PipelineData object when you call c8EmAsm_stageFrame().
struct EngineData {
  std::unique_ptr<XREngine> xr;
  std::unique_ptr<GlRealityFrame> gl;
  std::unique_ptr<Gr8FeatureShader> glShader;
  ConstRootMessage<CoordinateSystemConfiguration> configuredCoordinateSystem;
  // Holds IMU data before it is set on PipelineData. Do not access it from here directly except to
  // set on PipelineData in c8EmAsm_stageFrame.
  ConstRootMessage<RequestPose> eventQueue;
  ConstRootMessage<XrQueryResponse> queryResponse;

  float nearClip = 0.01f;
  float farClip = 1000.0f;

  // Global environment variables
  int displayWidth = 0;
  int displayHeight = 0;
  ConstRootMessage<DeviceInfo> deviceInfo;

  ConstRootMessage<XRConfiguration> xrConfig;

  std::deque<PipelineData> pipeline;
  PipelineData active;

  int frameTick = 0;
  int framesUntilRecenter = 5;

  bool disableVio = false;
  int checkAllZeroes = 0;

  YPlanePixelBuffer yBuffer;
  UVPlanePixelBuffer uvBuffer;

  std::deque<DetectionImageLoader> imLoaders_;
  DetectionImageMap detectionImages_;
  ConstRootMessage<CurvyGeometry> lastProcessedCurvyGeometry_;

  // Should we disable pixel buffer?
  bool disablePixelBuffer = false;

  // The JWT session token used to authenticate against VPS endpoints.
  String xrSessionToken;

  String sessionId;

  String mapSrcUrl = "";
  bool readyForFetch = false;

  CompressedImageData::Encoding encodingMode = CompressedImageData::Encoding::JPG_RGBA;

  bool isDataRecording = false;
  bool enableDataRecorder = false;

  std::optional<Quaternion> firstDevicePose_;
  std::optional<HVector3> firstDevicePosition_;
};

std::unique_ptr<EngineData> &data() {
  static std::unique_ptr<EngineData> d(nullptr);
  if (d == nullptr) {
    d.reset(new EngineData());
  }
  return d;
}

void clearImageTargetState() {
  data()->imLoaders_.clear();
  data()->detectionImages_.clear();

  // Reset the tracking state
  data()->xr->tracker().setImageTrackingDisabled(true);
  data()->xr->tracker().setImageTrackingDisabled(false);
}

void rebuildEngine() {
  data()->xr.reset(nullptr);
  data()->xr.reset(new XREngine());
  data()->xr->setDetectionImages(&data()->detectionImages_);
  data()->xr->setDisableSummaryLog(true);
  data()->xr->setResetLoggingTreeRoot(true);

  data()->pipeline.clear();
  data()->frameTick = 0;

  // Set the last externally configured coordinate system (or a default one if none was set).
  MutableRootMessage<XRConfiguration> configMessageBuilder;
  configMessageBuilder.builder().setCoordinateConfiguration(
    data()->configuredCoordinateSystem.reader());
  data()->xr->configure(configMessageBuilder.reader());
  data()->xr->configure(data()->xrConfig.reader());
}

AppContext::DeviceOrientation rotationToDeviceOrientation(int rotation) {
  switch (rotation) {
    case 0:
      return AppContext::DeviceOrientation::PORTRAIT;
    case 90:
    case -270:
      return AppContext::DeviceOrientation::LANDSCAPE_LEFT;
    case 180:
    case -180:
      return AppContext::DeviceOrientation::PORTRAIT_UPSIDE_DOWN;
    case 270:
    case -90:
      return AppContext::DeviceOrientation::LANDSCAPE_RIGHT;
    default:
      return AppContext::DeviceOrientation::UNSPECIFIED;
  }
}

// Builds a RealityRequest for the engine to process the current frame. This includes information
// about the user's config (which fields are requested? which coordinate system options should we
// use?), the user's graphics engine (what should the near clip, far clip, and aspect ratio be to
// set the intrinsics properly), pointers to the feature pyramid and info about its geometry, and
// any engine data that was precomputed in different stages of the pipeline, such as features that
// were pre-extracted from the image, or frame-frame tracker output.
void buildXrRequest(RealityRequest::Builder request, PipelineData &p) {
  // Request mean-pixel-value and pose procesing.
  // TODO(nb): enable external callers to set sensortest.
  // request.getMask().setSensorTest(true);

  // TODO(nb): should xrConfig be on PipelineData?
  if (data()->xrConfig.reader().hasMask()) {
    request.setXRConfiguration(data()->xrConfig.reader());
  } else {
    request.getXRConfiguration().getMask().setCamera(true);

    auto graphicsIntrinsics = request.getXRConfiguration().getGraphicsIntrinsics();
    graphicsIntrinsics.setTextureWidth(data()->displayWidth);
    graphicsIntrinsics.setTextureHeight(data()->displayHeight);
    graphicsIntrinsics.setNearClip(data()->nearClip);
    graphicsIntrinsics.setFarClip(data()->farClip);
  }

  request.setDeviceInfo(data()->deviceInfo.reader());

  auto captureGeometry = request.getXRConfiguration().getCameraConfiguration().getCaptureGeometry();

  // Engine expects a portrait capture size to properly calculate intrinsics from
  // pre-calibrated devices.
  if (p.captureWidth > p.captureHeight) {
    captureGeometry.setWidth(p.captureHeight);
    captureGeometry.setHeight(p.captureWidth);
  } else {
    captureGeometry.setWidth(p.captureWidth);
    captureGeometry.setHeight(p.captureHeight);
  }

  // Set the image pixel pointers.
  buildRealityRequestPyramid(data()->gl->pyramid(), request);
  request.getSensors().getCamera().getCurrentFrame().setVideoTimestampNanos(p.videoTimeNanos);
  request.getSensors().getCamera().getCurrentFrame().setFrameTimestampNanos(p.frameTimeNanos);
  request.getSensors().getCamera().getCurrentFrame().setTimestampNanos(p.timeNanos);

  // Set device pose and IMU data.
  auto devicePose = request.getSensors().getPose();
  auto dr = p.devicePose;
  setDevicePose(0.0f, 0.0f, 0.0f, dr.w(), dr.x(), dr.y(), dr.z(), &devicePose);
  devicePose.setEventQueue(p.eventQueue.reader().getEventQueue());

  auto webOrientation = devicePose.getDeviceWebOrientation();
  webOrientation.setAlpha(p.deviceWebOrientation.alpha);
  webOrientation.setBeta(p.deviceWebOrientation.beta);
  webOrientation.setGamma(p.deviceWebOrientation.gamma);
  // Set GPS data.
  request.getSensors().getGps().setLatitude(p.latitude);
  request.getSensors().getGps().setLongitude(p.longitude);
  request.getSensors().getGps().setHorizontalAccuracy(p.accuracy);

  auto appContext = request.getAppContext();
  appContext.setDeviceOrientation(rotationToDeviceOrientation(p.captureRotation));
}

// Set a single rgba image on the camera frame
void setJpgCompressedImageData(
  CompressedImageData::Builder imageBuilder,
  ConstYPlanePixels yPixels,
  ConstUVPlanePixels uvPixels) {
  // Store these values ready for the decoded value
  imageBuilder.setHeight(yPixels.rows());
  imageBuilder.setWidth(yPixels.cols());
  imageBuilder.setEncoding(CompressedImageData::Encoding::JPG_RGBA);

  // Deinterleave uv image into u and v plane pixels
  UPlanePixelBuffer uBuf{uvPixels.rows(), uvPixels.cols()};
  VPlanePixelBuffer vBuf{uvPixels.rows(), uvPixels.cols()};
  splitPixels(uvPixels, uBuf.pixels(), vBuf.pixels());

  // Encode our the image prior to storing the blob. Since JPG is internally in YUV, this
  // skips extra conversions.
  const auto encodedBuf = writePixelsToJpg(yPixels, uBuf.pixels(), vBuf.pixels());
  imageBuilder.initData(encodedBuf.size());
  std::memcpy(imageBuilder.getData().begin(), encodedBuf.data(), encodedBuf.size());
}

void setYuvOnRealityRequest(
  const YPlanePixels &yPixels,
  const UVPlanePixels &uvPixels,
  RealityRequest::Builder requestBuilder) {
  // copy planar yuv into capnp message
  auto currentFrame = requestBuilder.getSensors().getCamera().getCurrentFrame();

  MutableRootMessage<CameraFrame> imgmsg;
  CameraFrame::Builder frame = imgmsg.builder();
  frame.setVideoTimestampNanos(currentFrame.getVideoTimestampNanos());
  frame.setFrameTimestampNanos(currentFrame.getFrameTimestampNanos());
  frame.setTimestampNanos(currentFrame.getTimestampNanos());

  // TODO(dat): Support PNG / lossless
  // TODO(dat): We previous had to convert RGBA to YUV using yuv-pixels-array module but we can
  // just compress our encoding directly in a lossless format like PNG instead.

  // NOTE(dat): LogRecordCapture will automatically expand either format into Y and UV channel
  //            for later consumption downstream (see *-from-datarecorder.cc).
  if (data()->encodingMode == CompressedImageData::Encoding::JPG_RGBA) {
    auto rgbImage = frame.initRGBAImage();
    setJpgCompressedImageData(rgbImage.getOneOf().initCompressedImageData(), yPixels, uvPixels);
  } else {
    // Default unsupported mode to RAW
    setRawYImageData(yPixels, frame.getImage().getOneOf().initGrayImageData());
    setRawUvImageData(uvPixels, frame.getUvImage().getOneOf().initGrayImageData());
  }

  requestBuilder.getSensors().getCamera().initCurrentFrame();
  requestBuilder.getSensors().getCamera().setCurrentFrame(frame);
}

// Builds an xr request from the users's config, pyramid data, and any engine data precomputed by
// the pipeline, and then calls xr->execute. After execution completes, the pyramid renderer is
// updated to include any requested ROIs on the next call to draw. Finally, the engine response is
// saved on this frame's pipeline data.
//
// When dealing with CPU - GPU interactions, it can be bit complicated to understand GlRealityFrame.
// Here's a legend showing the processing steps. Note this is dependent on the order of calls in
// camera-loop.js. If that changes then the frame numbers here will change. So this diagram is
// really an artifact of the order in which we call processGpu and processCpu more than a property
// of the system. But it still may be useful in understanding this code. Terminology:
// - d0 = draw frame 0, r0 = read frame 0
// - i = inform GPU that we need to draw the next image
// GL
// Frame number 0   1       2       3
// GPU          d0  r0, d1  r1, d2  r2, d3
// CPU          -   0       1       2
//
// Similarly, when for DepthRenderer, this may be useful:
// DepthRenderer cpu updates.
// Frame number 0       1       2
// GPU          -       d0      r0
// CPU          stage0  -       addDepthPoints
void processPipelineData(PipelineData &p) {
  MutableRootMessage<RealityRequest> requestMsg;
  auto request = requestMsg.builder();
  buildXrRequest(request, p);
  auto intrinsics = Intrinsics::getProcessingIntrinsics(request);

  MutableRootMessage<RealityResponse> responseMsg;
  auto response = responseMsg.builder();
  {
    ScopeTimer t("xr-engine");
    data()->xr->execute(requestMsg.reader(), &response);
  }
  for (const auto &roi : getRois(response.getEngineExport().asReader())) {
    // Set ROIs for the next call to draw.
    data()->gl->addNextDrawRoi(roi);
  }
  if (!data()->detectionImages_.empty()) {
    data()->gl->addNextDrawHiResScans(intrinsics, {0.0f, 0.0f});
  }

  p.realityResponse = ConstRootMessage<RealityResponse>(response);
  if (data()->isDataRecording) {
    // Reset extrinsics to start at the origin. We can't just clear the origin and rebuild the
    // engine because we want to preserve the previous map.
    auto extrinsic = response.getXRResponse().getCamera().getExtrinsic();
    Quaternion currRot = toQuaternion(extrinsic.getRotation());
    HVector3 currPos = toHVector(extrinsic.getPosition());
    if (!data()->firstDevicePose_) {
      data()->firstDevicePose_ = std::make_optional<Quaternion>(currRot);
      data()->firstDevicePosition_ = std::make_optional<HVector3>(currPos);
    }

    HVector3 relPos = currPos - *data()->firstDevicePosition_;
    setPosition32f(relPos.x(), relPos.y(), relPos.z(), extrinsic.getPosition());

    request.getSensors().getCamera().getPixelIntrinsics().setPixelsWidth(intrinsics.pixelsWidth);
    request.getSensors().getCamera().getPixelIntrinsics().setPixelsHeight(intrinsics.pixelsHeight);
    request.getSensors().getCamera().getPixelIntrinsics().setCenterPointX(intrinsics.centerPointX);
    request.getSensors().getCamera().getPixelIntrinsics().setCenterPointY(intrinsics.centerPointY);
    request.getSensors().getCamera().getPixelIntrinsics().setFocalLengthHorizontal(
      intrinsics.focalLengthHorizontal);
    request.getSensors().getCamera().getPixelIntrinsics().setFocalLengthVertical(
      intrinsics.focalLengthVertical);

    setYuvOnRealityRequest(data()->yBuffer.pixels(), data()->uvBuffer.pixels(), request);
    MutableRootMessage<LogRecord> logRecordMsg;
    LogRecord::Builder record = logRecordMsg.builder();
    record.getRealityEngine().setRequest(request);
    record.getRealityEngine().setResponse(response);

    p.logRecord = ConstRootMessage<LogRecord>(logRecordMsg);

    EM_ASM_(
      {
        window._c8.logRecord = $0;
        window._c8.logRecordSize = $1;
      },
      p.logRecord.bytes().begin(),
      p.logRecord.bytes().size());
  }
}

void checkAllZeroes() {
  if (data()->checkAllZeroes) {
    EM_ASM_(
      {
        window._c8.isAllZeroes = $0;
        window._c8.wasEverNonZero = !$0 || window._c8.wasEverNonZero;
      },
      isAllZeroes(data()->gl->pyramid().level(3), 0));
  }
}

// NOTE(akul): This function does not mutate state. This is added to prevent an ipad runtime crash
// issue seen on an iPad 2017 5th gen running ios 15.0.2. It appears that this function must be
// called on every frame to prevent a crash. It is unknown why exactly this is needed, one
// hypothesis may be that this issue is memory related. It may be possible that on a future date
// this logic could be removed.
void doIosWorkaround() {
  const auto deviceInfoReader = data()->deviceInfo.reader();
  auto manufacturer = deviceInfoReader.getManufacturer();
  if (manufacturer != "FAKE_MANUFACTURER") {
    // Always return out of this function. It seems that the expression in the if statement needs to
    // be known at runtime not compile time for this workaround to work.
    return;
  }
  auto m = DeviceInfos::getDeviceModel(deviceInfoReader);
  auto K = Intrinsics::getCameraIntrinsics(m);
  TrackingSensorFrame frame;
  prepareTrackingSensorFrame(m, deviceInfoReader.getManufacturer(), K, {}, &frame);
}

// Runs the entire pipeline on each frame but lags 2 frames behind.  This allows us to have deferred
// reads of the camera frame from the gpu to the cpu.
bool processPipeline() {
  if (data()->pipeline.size() < 2) {
    return false;
  }
  checkAllZeroes();
  doIosWorkaround();
  processPipelineData(data()->pipeline[0]);
  return true;
}

void resetOriginToLastPose() {
  // Then to make sure the handoff is smooth, start the new engine's origin to the current camera
  // position.
  auto lastCamResponse = data()->active.realityResponse.reader().getXRResponse().getCamera();
  if (!lastCamResponse.hasExtrinsic()) {
    return;
  }
  MutableRootMessage<XRConfiguration> originConfig;
  auto coords = originConfig.builder().getCoordinateConfiguration();
  coords.setAxes(data()->configuredCoordinateSystem.reader().getAxes());
  coords.getOrigin().setRotation(lastCamResponse.getExtrinsic().getRotation());
  coords.getOrigin().setPosition(lastCamResponse.getExtrinsic().getPosition());
  // If the height of the camera is greater than zero, we don't need to reset the scale; but if it
  // is less than zero, we will need to reset the scale.
  if (lastCamResponse.getExtrinsic().getPosition().getY() > 1e-3) {
    coords.setScale(lastCamResponse.getExtrinsic().getPosition().getY());
  } else {
    coords.setScale(data()->configuredCoordinateSystem.reader().getScale());
    coords.getOrigin().getPosition().setY(
      data()->configuredCoordinateSystem.reader().getOrigin().getPosition().getY());
  }

  data()->xr->configure(originConfig.reader());
}

DetectionImageLoader newImageLoader(
  ImageTargetMetadata::Reader imageTargetMetadataReader, DeviceInfos::DeviceModel m) {
  DetectionImageLoader im;
  if (data()->glShader == nullptr) {
    data()->glShader.reset(new Gr8FeatureShader());
    data()->glShader->initialize();
  }
  im.initialize(
    data()->glShader.get(), imageTargetMetadataReader, Intrinsics::getCameraIntrinsics(m));
  return im;
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_addNewDetectionImageLoader(uint8_t *bytes, int numBytes) {
  ConstRootMessage<ImageTargetMetadata> imageTargetMetadata(bytes, numBytes);
  data()->imLoaders_.push_back(newImageLoader(
    imageTargetMetadata.reader(), DeviceInfos::getDeviceModel(data()->deviceInfo.reader())));
  EM_ASM_(
    {
      window.omni8 = {};
      window.omni8.imageTargetTexture = GL.textures[$0];
    },
    data()->imLoaders_.back().imTexture().id());
}

C8_PUBLIC
void c8EmAsm_cancelProcessNewDetectionImageTexture() {
  ScopeTimer rt("c8EmAsm_cancelProcessNewDetectionImageTexture");
  if (!data()->imLoaders_.empty()) {
    data()->imLoaders_.pop_front();
  }
}

C8_PUBLIC
void c8EmAsm_processNewDetectionImageTexture() {
  ScopeTimer rt("c8EmAsm_processNewDetectionImageTexture");
  auto &im = data()->imLoaders_.front();
  String name = im.name();
  // TODO(nb): spread processing of queue entries across multiple animation frames.
  im.processGpu();
  im.readDataToCpu();
  data()->detectionImages_.insert(std::make_pair(name, im.extractFeatures()));
  data()->imLoaders_.pop_front();
  // Set curvy geometry buffer for use in event callbacks
  if (data()->detectionImages_.at(name).getType() == DetectionImageType::CURVY) {
    MutableRootMessage<CurvyGeometry> curvyGeometryMsg;
    auto curvyGeometry = curvyGeometryMsg.builder();
    data()->detectionImages_.at(name).toCurvyGeometry(curvyGeometry);
    data()->lastProcessedCurvyGeometry_ = ConstRootMessage<CurvyGeometry>(curvyGeometry);
    EM_ASM_(
      {
        window._c8.lastProcessedCurvyGeometry = $0;
        window._c8.lastProcessedCurvyGeometrySize = $1;
      },
      data()->lastProcessedCurvyGeometry_.bytes().begin(),
      data()->lastProcessedCurvyGeometry_.bytes().size());
  }
}

C8_PUBLIC
void c8EmAsm_unloadDetectionImages(uint8_t *bytes, int numBytes) {
  ScopeTimer rt("c8EmAsm_unloadDetectionImages");

  ConstRootMessage<ImageTargetNames> imageTargetNames(bytes, numBytes);
  auto reader = imageTargetNames.reader();

  for (const auto &name : reader.getNames()) {
    data()->detectionImages_.erase(name);
  }
}

C8_PUBLIC
void c8EmAsm_configureXr(uint8_t *config, int configSize) {
  ConstRootMessage<XRConfiguration> cm(config, configSize);
  auto c = cm.reader();

  if (c.hasMask()) {  // Typically this is sent with the graphics config as well.
    data()->xrConfig = ConstRootMessage<XRConfiguration>(c);
    data()->disableVio = c.getMask().getDisableVio();
  }
  if (c.hasCoordinateConfiguration()) {  // Typically this is sent separately from mask/graphics.
    MutableRootMessage<CoordinateSystemConfiguration> coords;
    if (!data()->enableDataRecorder) {
      // Save the user-specified coordinate configuration for future calls to recenter.
      coords = c.getCoordinateConfiguration();
    }
    data()->configuredCoordinateSystem = ConstRootMessage<CoordinateSystemConfiguration>(coords);
  }

  if (c.hasMapSrcUrl()) {
    data()->mapSrcUrl = c.getMapSrcUrl();
  }

  // This check is a protective measure in case the user calls configureXr before onAttach.
  // onAttach() calls rebuild() which initializes data()->xr
  if (data()->xr != nullptr) {
    data()->xr->configure(c);
  }
}

C8_PUBLIC
void c8EmAsm_clearDetectionImages() {
  ScopeTimer rt("c8EmAsm_clearDetectionImages");
  clearImageTargetState();
}

C8_PUBLIC
void c8EmAsm_engineCleanup() { data().reset(nullptr); }

C8_PUBLIC
void c8EmAsm_setDisablePixelBuffer(bool disablePixelBuffer) {
  data()->disablePixelBuffer = disablePixelBuffer;
}

C8_PUBLIC
void c8EmAsm_initDeviceInfo(uint8_t *deviceInfo, int deviceInfoSize) {
  data()->deviceInfo = ConstRootMessage<DeviceInfo>(deviceInfo, deviceInfoSize);
}

C8_PUBLIC
void c8EmAsm_engineInit(int captureWidth, int captureHeight, int rotation) {
  data()->displayWidth = EM_ASM_INT({ return window._c8.displayWidth; });
  data()->displayHeight = EM_ASM_INT({ return window._c8.displayHeight; });

  if (data()->glShader == nullptr) {
    data()->glShader.reset(new Gr8FeatureShader());
    data()->glShader->initialize();
    data()->gl.reset(new GlRealityFrame());
  }
  data()->gl->initialize(data()->glShader.get(), captureWidth, captureHeight, rotation);

  int useWebGl2 = EM_ASM_INT({ return window._c8.useWebGl2; });
  if (useWebGl2 && !data()->disablePixelBuffer) {
    data()->gl->enableWebGl2PixelBuffer();
  }

  // Set a default origin / facing until one is configured.
  if (!data()->configuredCoordinateSystem.reader().hasOrigin() && !data()->enableDataRecorder) {
    MutableRootMessage<CoordinateSystemConfiguration> defaultCoords;
    auto coords = defaultCoords.builder();
    coords.setAxes(CoordinateSystemConfiguration::CoordinateAxes::X_LEFT_Y_UP_Z_FORWARD);
    setQuaternion32f(1.0f, 0.0f, 0.0f, 0.0f, coords.getOrigin().getRotation());
    setPosition32f(0, 2, 0, coords.getOrigin().getPosition());
    data()->configuredCoordinateSystem =
      ConstRootMessage<CoordinateSystemConfiguration>(defaultCoords);
  }

  if (data()->sessionId.empty()) {
    RandomNumbers random = {std::random_device()};
    data()->sessionId =
      format("%016llx%016llx", random.nextUnsignedInt64(), random.nextUnsignedInt64());
  }

  rebuildEngine();
}

C8_PUBLIC
void c8EmAsm_setCheckAllZeroes(int checkAllZeroes) { data()->checkAllZeroes = checkAllZeroes; }

C8_PUBLIC
void c8EmAsm_query(uint8_t *ptr, int size) {
  auto requestMsg = ConstRootMessage<XrQueryRequest>(ptr, size);

  MutableRootMessage<XrQueryResponse> responseMsg;
  auto response = responseMsg.builder();
  {
    ScopeTimer t("xr-engine-query");
    data()->xr->query(requestMsg.reader(), &response);
  }
  data()->queryResponse = ConstRootMessage<XrQueryResponse>(responseMsg);

  EM_ASM_(
    {
      window._c8.queryResponse = $0;
      window._c8.queryResponseSize = $1;
    },
    data()->queryResponse.bytes().begin(),
    data()->queryResponse.bytes().size());
}

C8_PUBLIC
void c8EmAsm_recenter() {
  C8Log("[engine] %s", "recenter");
  rebuildEngine();
}

// Called by onProcessGpu. Saves IMU data to be read by c8EmAsm_stageFrame().
// TODO(paris): We could actually pass this data in c8EmAsm_stageFrame and remove data()->eventQueue
C8_PUBLIC
void c8EmAsm_setEventQueue(uint8_t *ptr, int size) {
  data()->eventQueue = ConstRootMessage<RequestPose>(ptr, size);
}

// Called by onProcessGpu. It records the current camera feed data as a PipelineData. It draws the
// camera feed as an image pyramid after reading a previous image pyramid from the gpu onto the cpu.
C8_PUBLIC
void c8EmAsm_stageFrame(
  GLuint cameraTexture,
  int captureWidth,
  int captureHeight,
  int rotation,
  float alpha,
  float beta,
  float gamma,
  float qw,
  float qx,
  float qy,
  float qz,
  double videoTimeSeconds,
  double frameTimeSeconds,
  double timeNanos,
  double latitude,
  double longitude,
  double accuracy) {
  ScopeTimer t("c8EmAsm_stageFrame");
  data()->frameTick++;
  data()->pipeline.emplace_back();
  auto &p = data()->pipeline.back();
  p.realityTextureId = cameraTexture;
  p.devicePose = Quaternion(qw, qx, qy, qz);
  p.deviceWebOrientation = {alpha, beta, gamma};
  p.videoTimeNanos = static_cast<int64_t>(videoTimeSeconds * 1e9);
  p.frameTimeNanos = static_cast<int64_t>(frameTimeSeconds * 1e9);
  p.timeNanos = static_cast<int64_t>(timeNanos);
  p.captureWidth = captureWidth;
  p.captureHeight = captureHeight;
  p.captureRotation = rotation;
  p.eventQueue = ConstRootMessage<RequestPose>(data()->eventQueue.reader());
  p.latitude = latitude;
  p.longitude = longitude;
  p.accuracy = accuracy;

  // Draw the camera frame as a pyramid on the gpu.
  data()->gl->draw(p.realityTextureId, GlRealityFrame::Options::DEFER_READ);
}  // end stageFrame()

C8_PUBLIC
void c8EmAsm_engineOrientationChange(int captureWidth, int captureHeight, int rotation) {
  // To make sure everything resets cleanly, just rebuild the pyramid and the engine.
  // TODO(nb/alvin): come up with a less heavy-handed solution for this.
  c8EmAsm_engineInit(captureWidth, captureHeight, rotation);
  resetOriginToLastPose();
}

C8_PUBLIC
void c8EmAsm_engineVideoSizeChange(int captureWidth, int captureHeight, int rotation) {
  // To make sure everything resets cleanly, just rebuild the pyramid and the engine.
  // TODO(nb/alvin): come up with a less heavy-handed solution for this.
  c8EmAsm_engineInit(captureWidth, captureHeight, rotation);
  resetOriginToLastPose();
}

C8_PUBLIC
void c8EmAsm_setNewMeshesFromJs(
  const char *nodeId,
  float *points,
  int pointsLength,
  float *colors,
  int colorsLength,
  uint32_t *triangles,
  int trianglesLength) {}

// Called by onProcessCpu.  Returns the estimated camera extrinsic for the active frame.
C8_PUBLIC
void c8EmAsm_processStagedFrame() {
  ScopeTimer("c8EmAsm_processStagedFrame");

  // If we don't process the pipeline, then we will simply return data()->active. Otherwise, we
  // will get the earliest frame in the pipeline and set it to active.
  if (processPipeline()) {
    data()->active = std::move(data()->pipeline[0]);
    data()->pipeline.pop_front();

    if (data()->framesUntilRecenter > 0) {
      if (data()->framesUntilRecenter == 1) {
        data()->readyForFetch = true;
        c8EmAsm_recenter();
      }
      --data()->framesUntilRecenter;
      MutableRootMessage<RealityResponse> realityAtOrigin(data()->active.realityResponse.reader());
      auto camAtOrigin =
        realityAtOrigin.builder().getXRResponse().getCamera().getExtrinsic().getPosition();

      auto origin = data()->configuredCoordinateSystem.reader().getOrigin().getPosition();
      setPosition32f(origin.getX(), origin.getY(), origin.getZ(), camAtOrigin);

      data()->active.realityResponse = ConstRootMessage<RealityResponse>(realityAtOrigin);
    }
  }

  EM_ASM_(
    {
      window._c8.reality = $0;
      window._c8.realitySize = $1;
      window._c8.realityTexture = GL.textures[$2];
    },
    data()->active.realityResponse.bytes().begin(),
    data()->active.realityResponse.bytes().size(),
    data()->active.realityTextureId);
}  // end processStagedFrame()

C8_PUBLIC
void c8EmAsm_setXrSessionToken(char *xrSessionToken) { data()->xrSessionToken = xrSessionToken; }

// You should call this method before calling c8EmAsm_processStagedFrame so processPipeline()
// can use the latest available YUV data
C8_PUBLIC
void c8EmAsm_prepareYuvBuffer(int rows, int cols, int bytesPerRow, uint8_t *pixels) {
  ScopeTimer t("prepare-recorder-yuv-buffer");
  auto yuvPixels = ConstYUVA8888PlanePixels(rows, cols, bytesPerRow, pixels);

  if (data()->yBuffer.pixels().rows() != rows || data()->yBuffer.pixels().cols() != cols) {
    data()->yBuffer = YPlanePixelBuffer(rows, cols);
    data()->uvBuffer = UVPlanePixelBuffer(rows / 2, cols / 2);
  }
  auto yPixels = data()->yBuffer.pixels();
  auto uvPixels = data()->uvBuffer.pixels();
  yuvToPlanarYuv(yuvPixels, &yPixels, &uvPixels);
}

// Set format on the wire. Default to 0. See recorder.ts setDataCompression method.
// @param encoding See definition in recorder.ts.
C8_PUBLIC
void c8EmAsm_setCompressedFormat(int encoding) {
  // Use int instead of CompressedImageData::Encoding to avoid coupling + not sure if WASM can
  // correctly pass an int from JS into capnp enum (uint16_t)
  if (encoding == static_cast<int>(CompressedImageData::Encoding::JPG_RGBA)) {
    data()->encodingMode = CompressedImageData::Encoding::JPG_RGBA;
  } else {
    data()->encodingMode = CompressedImageData::Encoding::UNSPECIFIED;
  }
}

C8_PUBLIC
void c8EmAsm_setIsDataRecording(bool isDataRecording) { data()->isDataRecording = isDataRecording; }

C8_PUBLIC
void c8EmAsm_setEnableDataRecorder(bool enableDataRecorder) {
  data()->enableDataRecorder = enableDataRecorder;
}

}  // EXTERN "C"
#else
#warning "xr-js-cc requires --cpu=js"
#endif
