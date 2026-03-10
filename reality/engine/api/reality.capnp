@0xedb7c95bd8ba2fff;

using import "base/camera-intrinsics.capnp".GraphicsCalibration;
using import "base/camera-intrinsics.capnp".GraphicsPinholeCameraModel;
using import "base/debug.capnp".DebugData;
using import "base/geo-types.capnp".ActiveSurface;
using import "base/geo-types.capnp".SurfaceSet;
using import "base/geo-types.capnp".Transform32f;
using import "base/id.capnp".EventId;
using import "base/image-types.capnp".CompressedImageData;
using import "base/lighting.capnp".GlobalIllumination;
using import "device/info.capnp".DeviceInfo;
using import "request/app.capnp".AppContext;
using import "request/flags.capnp".RequestFlags;
using import "request/mask.capnp".RequestMask;
using import "request/sensor.capnp".Gr8ImageRoi;
using import "request/sensor.capnp".RequestSensor;
using import "response/features.capnp".DeprecatedResponseFeatures;
using import "response/features.capnp".ResponseFeatureSet;
using import "response/pose.capnp".ResponsePose;
using import "response/pose.capnp".XrTrackingState;
using import "response/sensor-test.capnp".ResponseSensorTest;
using import "response/status.capnp".ResponseStatus;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api");
$Java.outerClassname("Reality");  # Must match this file's name!

struct RealityRequest {
  mask @0 :RequestMask;
  sensors @1 :RequestSensor;
  flags @2 :RequestFlags;

  # XR api
  xRConfiguration @3: XRConfiguration;

  deviceInfo @4: DeviceInfo;
  appContext @5: AppContext;

  # Serialized debug data
  debugData @6: List(DebugData);
}

struct XRAppEnvironment {
  enum RenderingSystemKind {
    unspecified @0;
    opengl @1;
    metal @2;
    direct3d11 @3;
  }
  managedCameraTextures @0: CameraTextures;
  renderingSystem @1: RenderingSystemKind;
}

struct CameraTextures {
  rgbaTexture @0 :CapnpTexture;
  yTexture @1: CapnpTexture;
  uvTexture @2: CapnpTexture;
}

struct CapnpTexture @0x937e66cf4dcd1c25 {
  ptr @0 :Int64;
  width @1: Int32;
  height @2: Int32;
}

struct XRCapabilities {
  enum PositionalTrackingKind {
    unspecified @0;
    rotationOnly @1;
    rotationAndPosition @2;
    rotationAndPositionNoScale @3;
  }

  enum SurfaceEstimationKind {
    unspecified @0;
    fixedSurfaces @1;
    horizontalOnly @2;
    verticalOnly @3;
    horizontalAndVertical @4;
  }

  enum TargetImageDetectionKind {
    unspecified @0;
    unsupported @1;
    fixedSizeImageTarget @2;
  }

  positionTracking @0: PositionalTrackingKind;
  surfaceEstimation @1: SurfaceEstimationKind;
  targetImageDetection @2: TargetImageDetectionKind;
}

struct XREnvironment {
  enum ImageShaderKind {
    unspecified @0;
    arcore @1;
    standardRgba @2;
  }
  enum ARCoreAvailability {
    unspecified @0;
    supportedApkTooOld @1;
    supportedInstalled @2;
    supportedNotInstalled @3;
    unsupportedDeviceNotCapable @4;
    unknown @5;
  }
  realityImageWidth @0: Int32;
  realityImageHeight @1: Int32;
  realityImage @2: CameraTextures;
  realityImageShader @3: ImageShaderKind;
  capabilities @4: XRCapabilities;
  engineInfo @5: EngineInfo;
  aRCoreAvailability @6: ARCoreAvailability;
}

struct EngineInfo {
  xrVersion @0: Text;
}

struct XRDetectionImage {
  name @0: Text;
  image @1: CompressedImageData;
  realWidthInMeter @2: Float32;
}

struct ImageDetectionConfiguration {
  imageDetectionSet @0: List(XRDetectionImage);
}

struct XRWayspotAnchor {
  name @0: Text;
  blob @1: Text;
  nodeId @2: Text;
  spaceId @3: Text;
  latitude @4: Float64;
  longitude @5: Float64;
}

struct VpsConfiguration {
  wayspotAnchors @0: List(XRWayspotAnchor);
  environment @1: Text; # Deprecated
}

struct XRConfiguration {
  mask @0: XRRequestMask; # flags which control certain features
  graphicsIntrinsics @1: GraphicsPinholeCameraModel;
  mobileAppKey @2: Text;
  cameraConfiguration @3: XRCameraConfiguration;

  # Only change if an image detection set is present in the capnp message
  # i.e. to clear the set, provide an empty set
  #      to change other configuration settings, simply provide no image detection set.
  #
  # ImageDetection configuration changes should be sent in isolation. If multiple other changes are
  # batched with image detection configuration changes, the behavior is unspecified and part of
  # the update might be ignored.
  imageDetection @4: ImageDetectionConfiguration;

  # Adjust the coordinate system so that the specified origin location in the current coordinate
  # system becomes the origin in the new coordinate system. Note that consecutive calls to configure
  # the coordinate system are additive.
  #
  # CoordinateSystemConfiguration changes should be sent in isolation. If multiple other changes are
  # batched with coordinate system configuration changes, the behavior is unspecified and part of
  # the update might be ignored.
  coordinateConfiguration @5: CoordinateSystemConfiguration;

  # Adjust properties of the engine being used. If the engine is currently running, this may entail
  # pausing and tearing down the engine and recreating it. Typically these changes should be made
  # before the engine resumes execution.
  #
  # XREngineConfiguration changes should be sent in isolation. If multiple other changes are batched
  # with engine configuration changes, the behavior is unspecified and part of the update might be
  # ignored.
  engineConfiguration @6: XREngineConfiguration;

  # Only change if a vps set is present in the capnp message
  # i.e. to clear the set, provide an empty set
  #      to change other configuration settings, simply provide no vps set.
  #
  # ImageDetection configuration changes should be sent in isolation. If multiple other changes are
  # batched with vps configuration changes, the behavior is unspecified and part of
  # the update might be ignored.
  vpsConfiguration @7: VpsConfiguration;

  # To hold an optionally passed CloudMap URL which can be downloaded and tracked against
  mapSrcUrl @8: Text;
}

struct XRRequestMask {
  # VPS mode for localization.
  enum VpsMode {
    # Run localization on device using a local map.
    off @0;
    # Run localization on VPS server to localize to nearby/selected anchors and fetch localization
    # result from server.
    server @1;
    # Download map8 maps of nearby/selected anchors from VPS server and run localization on device
    # using the downloaded maps.
    device @2;
    # Try to localize in device mode first with a fallback to server mode for the case where not
    # every nearby/selected anchor has map8 data.
    deviceWithServerFallback @3;
  }
  lighting @0: Bool;
  camera @1: Bool;
  surfaces @2: Bool; # i.e. enable both vertical and horizontal surfaces
  verticalSurfaces @3: Bool;
  featureSet @4: Bool;
  estimateScale @5: Bool;
  disableVio @6: Bool;
  disableImageTargets @7: Bool;
  vps @8: Bool; # [Deprecated][Engine v28] use vpsMode instead
  depthInit @9: Bool; # [Deprecated][Engine v28] coupled with server VPS mode
  vpsMode @10: VpsMode;
}

struct XRCameraConfiguration {
  autofocus @0: Bool;
  drawTimestampOnFrame @1: Bool;
  captureGeometry @2: CameraCaptureGeometry;
  depthMapping @3: Bool;
  gps @4: Bool;
}

struct CameraCaptureGeometry {
  width @0: Int32;
  height @1: Int32;
}

# NOTE(nb): Ideally this would be shared between reality/face, but moving it from this file is an
# unsafe schema change.
struct CoordinateSystemConfiguration {
  enum CoordinateAxes {
    unspecified @0;
    xRightYUpZForward @1;
    xLeftYUpZForward @2;
  }
  origin @0: Transform32f;
  scale @1: Float32;
  axes @2: CoordinateAxes;
  mirroredDisplay @3: Bool;
}

struct XREngineConfiguration {
  enum SpecialExecutionMode {
    unspecified @0;
    normal @1;
    remoteOnly @2;
    disableNativeArEngine @3;
  }
  mode @0: SpecialExecutionMode;
}

struct RealityResponse {
  eventId @0: EventId;
  status @1: ResponseStatus;
  sensorTest @2: ResponseSensorTest;
  pose @3: ResponsePose;
  features @4: DeprecatedResponseFeatures;

  # XR api
  xRResponse @5: XRResponse;
  appContext @6: AppContext;

  featureSet @7 :ResponseFeatureSet;

  # Display texture.
  rgbaTexture @8 :CapnpTexture;

  engineExport @9: EngineExport;
}

struct EngineExport {
  rois @0: List(Gr8ImageRoi);
}

struct XRResponse {
  lighting @0: ResponseLighting;
  camera @1: ResponseCamera;
  surfaces @2: ResponseSurfaces;
  detection @3: ResponseDetection;
}

struct XrQueryRequest {
  hitTest @0: XrHitTestRequest;
}

struct XrQueryResponse {
  hitTest @0: XrHitTestResponse;
}

struct XrHitTestRequest {
  x @0: Float32;
  y @1: Float32;
  includedTypes @2: List(XrHitTestResult.ResultType);
}

struct XrHitTestResponse {
  hits @0: List(XrHitTestResult);
}

struct XrHitTestResult {
  enum ResultType {
    unspecified @0;
    featurePoint @1;  # Covers FeaturePoint
    estimatedSurface @2;  # Covers EstimatedHorizontalPlane, EstimatedVerticalPlane, ExistingPlane
    detectedSurface @3;  # Covers ExistingPlaneUsingExtent, ExistingPlaneUsingGeometry
  }
  type @0: ResultType;
  place @1: Transform32f;
  distance @2: Float32;
  # TODO(nb): ARKit gives anchors UUIDs, do we need those here? It might be hard to keep them
  # stable, so let's delay adding them until a demonstrated need.
}

struct ResponseLighting {
  global @0: GlobalIllumination;
}

struct ResponseCamera {
  extrinsic @0: Transform32f;
  intrinsic @1: GraphicsCalibration;
  trackingState @2: XrTrackingState;
}

struct ResponseSurfaces {
  set @0: SurfaceSet;
  activeSurface @1: ActiveSurface;
}

struct ResponseDetection {
  # The Image Targets we found this frame.
  images @0: List(DetectedImage);
  # The Wayspot Anchors we found this frame.
  wayspotAnchors @1: List(DetectedWayspotAnchors);
  # The mesh we found this frame.
  mesh @2: DetectedMesh;
  # If we localized to a Node this frame, contains its ID.
  trackedNodeId @3: Text;
  # True if VPS was reset and we should clear our Nodes and Wayspot Anchors.
  vpsWasReset @4: Bool;
}

enum ImageTargetTypeMsg {
  unspecified @0;
  planar @1;
  curvy @2;
}

# How we describe the planar or curvy image when sending data from cpp to js
struct DetectedImage {
  enum TrackingStatus {
    unspecified @0;
    fullTracking @1;
    lastKnownPose @2;
    notTracking @3;
  }

  place @0: Transform32f;
  # XRDetectionImage.name corresponding to this anchor
  name @1: Text;
  id @2: Int64;
  widthInMeters @3: Float32;
  heightInMeters @4: Float32;
  trackingStatus @5: TrackingStatus;
  type @6: ImageTargetTypeMsg;

  # this is only populated if type == curvy
  curvyGeometry @7: CurvyGeometry;
}

struct DetectedMesh {
  hasUpdate @0: Bool = false;
  # Right-handed with +x: right, +y: up, +z: towards.
  place @1: Transform32f;
  nodeId @2: Text;
}

struct DetectedWayspotAnchors {
  # Right-handed with +x: right, +y: up, +z: towards.
  place @0: Transform32f;
  # Name of the Wayspot Anchor
  name @1: Text;
  # Unused
  tombstone0 @2: Float32;
}

struct CurvyGeometry {
  # this data is provided by the client
  curvyCircumferenceTop @0: Float32;
  curvyCircumferenceBottom @1: Float32;
  curvySideLength @2: Float32;
  targetCircumferenceTop @3: Float32;

  # this data is computed
  height @4: Float32;
  topRadius @5: Float32;
  bottomRadius @6: Float32;
  arcLengthRadians @7: Float32;
  arcStartRadians @8: Float32;
}

struct ImageTargetMetadata {
  name @0: Text;
  type @1: ImageTargetTypeMsg;
  imageWidth @2: Int32;
  imageHeight @3: Int32;
  originalImageWidth @4: Int32;
  originalImageHeight @5: Int32;
  cropOriginalImageX @6: Int32;
  cropOriginalImageY @7: Int32;
  cropOriginalImageWidth @8: Int32;
  cropOriginalImageHeight @9: Int32;
  curvyGeometry @10: CurvyGeometry;
  isRotated @11: Bool;
  # True if this is a static target. If true, we consider the orientation to determine the feature
  # type.
  isStaticTarget @12: Bool;
  # Roll angle of the image target in the scene.
  rollAngle @13: Float32;
  # Pitch angle of the image target in the scene.
  pitchAngle @14: Float32;
}

struct ImageTargetNames {
  names @0: List(Text);
}
