@0x9fb1cef4fbeee48d;

using import "../base/camera-intrinsics.capnp".GraphicsPinholeCameraModel;
using import "../base/camera-intrinsics.capnp".PixelPinholeCameraModel;
using import "../base/geo-types.capnp".Quaternion32f;
using import "../base/geo-types.capnp".Position32f;
using import "../base/geo-types.capnp".SurfaceFace;
using import "../base/geo-types.capnp".SurfaceVertex;
using import "../base/geo-types.capnp".SurfaceTextureCoord;
using import "../base/image-types.capnp".ImageUnion;
using import "../base/image-types.capnp".GrayImagePointer;
using import "../base/image-types.capnp".GrayImageData;
using import "../base/image-types.capnp".GrayImageData16;
using import "../base/image-types.capnp".FloatImageData;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("Sensor");  # Must match this file's name!

struct RequestSensor {
  camera @0 :RequestCamera;
  pose @1 :RequestPose;
  aRKit @2 :RequestARKit;
  tango @3 :RequestTango;
  aRCore @4 :RequestARCore;
  gps @5 :RequestGPS;
}

struct RequestCamera {
  # The frame captured by the camera.
  currentFrame @0 :CameraFrame;

  # Camera intrinsics and display context, if known.
  pixelIntrinsics @1: PixelPinholeCameraModel;

  # The clockwise angle through which the output image needs to be rotated to be upright.
  sensorOrientation @2: Int32;
}

struct RequestGPS {
  latitude @0: Float64;
  longitude @1: Float64;

  # The accuracy, with a 95% confidence level, of the latitude and longitude properties expressed
  # in meters: https://developer.mozilla.org/en-US/docs/Web/API/GeolocationCoordinates/accuracy
  horizontalAccuracy @2: Float64;

  elevationMeters @3: Float64;
  elevationAccuracy @4: Float64;
  headingDegrees @5: Float64;
  headingAccuracy @6: Float64;
}

struct CameraFrame {
  # Y-plane from yuv camera data.
  image @0 :ImageUnion;

  # Color data; this is a striped UV plane. It has the same width and half the height of the source
  # image; every other pixel is u and v, so the effective width is half.
  uvImage @1 :ImageUnion;

  # Color data; this is represented by a U and a V plane. These planes may or may not be perfectly
  # interleaved. They will have the same pixel stride.
  twoPlaneUvImage @2 :TwoPlaneUvImage;

  # Timestamp of when the image was staged in nanos.
  timestampNanos @3: Int64;

  # Image data; this is represented by a single buffer containing RBGA bytes.
  rGBAImage @4: ImageUnion;

  # Image pyramid for GR8 feature detection
  pyramid @5: Gr8PyramidData;

  # The video playback time in nanos.
  videoTimestampNanos @6: Int64;

  # Timestamp of when the image was read in nanos.
  frameTimestampNanos @7: Int64;
}

struct Gr8PyramidData {
  image @0: GrayImagePointer;
  levels @1: List(Gr8LevelLayout);
  rois @2: List(Gr8Roi);
}

struct Gr8PyramidDataBuffer {
  image @0: GrayImageData;
  levels @1: List(Gr8LevelLayout);
  rois @2: List(Gr8Roi);
}

struct Gr8LevelLayout {
  c @0: Int32;
  r @1: Int32;
  w @2: Int32;
  h @3: Int32;
  rotated @4: Bool;
}

struct Gr8ActivationRegion {
  left @0: Float32;
  right @1: Float32;
  top @2: Float32;
  bottom @3: Float32;
}

struct Gr8CurvyImageGeometry {
  radius @0: Float32;
  height @1: Float32;
  srcRows @2: Int32;
  srcCols @3: Int32;
  activationRegion @4: Gr8ActivationRegion;
  radiusBottom @5: Float32;
}

struct Gr8ImageRoi {
  enum Source {
    unspecified @0;
    gravity @1;
    imageTarget @2;
    hiresScan @3;
    curvyImageTarget @4;
  }

  source @0: Source;
  id @1: Int32;
  # Used for all sources except curvyImageTarget
  warp @2: List(Float32);
  inverseWarp @3: List(Float32);

  # Used when source is curvyImageTarget
  geom @4: Gr8CurvyImageGeometry;
  cameraIntrinsics @5: List(Float32);
  inverseCameraIntrinsics @6: List(Float32);
  # Location of the camera with respect to the image target 3d model
  globalPose @7: List(Float32);
  inverseGlobalPose @8: List(Float32);
  name @9: Text;
}

struct Gr8Roi {
  layout @0: Gr8LevelLayout;
  roi @1: Gr8ImageRoi;
}

struct TwoPlaneUvImage {
  uImage @0 :ImageUnion;
  vImage @1 :ImageUnion;

  # Pixel strides for both the U image plane and V image plane.
  pixelStride @2: Int32;
}

# https://developer.mozilla.org/en-US/docs/Web/API/Window/deviceorientation_event
struct WebOrientation {
  alpha @0: Float32;
  beta @1: Float32;
  gamma @2: Float32;
}

struct RequestPose {
  devicePose @0: Quaternion32f;
  deviceAcceleration @1: Position32f;
  eventQueue @2: List(RawPositionalSensorValue);

  # The amount that the sensors need to be rotated to bring them to a portrait reference frame.
  sensorRotationToPortrait @3: Quaternion32f;
  deviceWebOrientation @4: WebOrientation;
}

struct RawPositionalSensorValue {
  enum PositionalSensorKind {
    unspecified @0;
    accelerometer @1;
    gyroscope @2;
    magnetometer @3;
    linearAcceleration @4;
  }

  kind @0: PositionalSensorKind;
  timestampNanos @1: Int64;
  value @2: Position32f;
  # The event timestamp. On web: https://developer.mozilla.org/en-US/docs/Web/API/Event/timeStamp
  eventTimestampNanos @3: Int64;
  # The interval at which data is obtained from the underlying hardware. On web it is this or the
  # equivalent property per sensor kind:
  # https://developer.mozilla.org/en-US/docs/Web/API/DeviceMotionEvent/interval
  intervalNanos @4: Int64;
}

struct ARKitLightingEstimate {
  ambientIntensity @0: Float32;
  ambientColorTemperature @1: Float32;
}

struct ARKitPlaneGeometry {
  vertices @0: List(SurfaceVertex);
  textureCoordinates @1: List(SurfaceTextureCoord);
  triangleIndices @2: List(SurfaceFace);
  boundaryVertices @3: List(SurfaceVertex);
}

struct ARKitImageAnchor {
  enum TrackingStatus {
    unspecified @0;
    fullTracking @1;
    lastKnownPose @2;
  }

  uuidHash @0: Int64;
  anchorPose @1: CameraPose;
  name @2: Text;
  physicalSizeWidth @3: Float32;
  physicalSizeHeight @4: Float32;
  trackingStatus @5: TrackingStatus;
}

struct ARKitPlaneAnchor {
  enum ARKitPlaneAnchorAlignment {
    unspecified @0;
    horizontal @1;
    vertical @2;
  }

  uuidHash @0: Int64;
  anchorPose @1: CameraPose;
  center @2: Position32f;
  extent @3: Position32f;
  alignment @4: ARKitPlaneAnchorAlignment;
  geometry @5: ARKitPlaneGeometry;
}

struct ARKitTrackingState {
  enum ARKitTrackingStatus {
    unspecified @0;
    notAvailable @1;
    normal @2;
    limited @3;
  }
  enum ARKitTrackingStatusReason {
    unspecified @0;
    initializing @1;
    relocalizing @2;
    excessiveMotion @3;
    insufficientFeatures @4;
  }
  status @0: ARKitTrackingStatus;
  reason @1: ARKitTrackingStatusReason;
}

struct ARKitPoint {
  position @0: Position32f;
  id @1: UInt64;
}

struct RequestARKit {
  pose @0: CameraPose;
  lightEstimate @1: ARKitLightingEstimate;
  anchors @2: List(ARKitPlaneAnchor);
  trackingState @3: ARKitTrackingState;
  pointCloud @4: List(ARKitPoint);
  imageAnchors @5: List(ARKitImageAnchor);
  depthMap @6 : FloatImageData;
}

struct CameraPose {
  rotation @0: Quaternion32f;
  translation @1: Position32f;
}

struct RequestTango {
  pose @0: CameraPose;
}

struct ARCoreLightingEstimate() {
  pixelIntensity @0 :Float32;
}

struct ARCorePlane {
  enum ARCorePlaneAlignment {
    unspecified @0;
    horizontal @1;
    vertical @2;
  }
  hash @0: Int32;
  centerPose @1: CameraPose;
  extent @2: Position32f;
  polygonVertices @3: List(Position32f);
  alignment @4: ARCorePlaneAlignment;
}

struct ARCorePoint {
  position @0: Position32f;
  confidence @1: Float32;
  id @2: Int32;
}

struct ARCoreDetectedImage {
  enum TrackingStatus {
    unspecified @0;
    fullTracking @1;
    lastKnownPose @2;
    notTracking @3;
  }

  hash @0: Int32;
  centerPose @1: CameraPose;
  name @2: Text;

  # Extent is given along the X and Z axis, measured in meters.
  extent @3: Position32f;
  trackingStatus @4: TrackingStatus;
}

struct RequestARCore {
  enum ARCoreTrackingState {
    unspecified @0;
    tracking @1;
    paused @2;
    stopped @3;
  }
  
  enum ARCoreTrackingFailureReason {
    unspecified @0;
    badState @1;
    cameraUnavailable @2;
    excessiveMotion @3;
    insufficientFeatures @4;
    insufficientLight @5;
    none @6;
  }

  # This is the pose of the virtual camera in ARCore's right-handed world coordinate space. Note
  # that it is affected by the display orientation.
  pose @0: CameraPose;
  lightEstimate @1: ARCoreLightingEstimate;
  planes @2: List(ARCorePlane);
  projectionMatrix @3: List(Float32);
  poseTrackingState @4: ARCoreTrackingState;
  pointCloud @5: List(ARCorePoint);
  detectedImages @6: List(ARCoreDetectedImage);

  # The pose of the Android Sensor Coordinate System in ARCore's right-handed world coordinate
  # space, which is y+ up, x+ right, and z- forward.  If you want imu in relation to the camera,
  # do imuInCam = pose.inverse() * imuInWorld.
  imuInWorld @7: CameraPose;

  depthMap @8 : GrayImageData16;
  
  poseTrackingFailureReason @9: ARCoreTrackingFailureReason;
}
