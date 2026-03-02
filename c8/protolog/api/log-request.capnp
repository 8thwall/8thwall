@0xffb2cb1aca0c0e57;

using import "../../../c8/stats/api/detail.capnp".LoggingDetail;
using import "../../../c8/stats/api/summary.capnp".LoggingSummary;
using import "../../../reality/engine/api/reality.capnp".RealityRequest;
using import "../../../reality/engine/api/reality.capnp".RealityResponse;
using import "../../../reality/engine/api/device/info.capnp".DeviceInfo;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.protolog.api");
$Java.outerClassname("LogRequest");  # Must match this file's name!

struct LogServiceRequest {
  records @0 :List(LogRecord);
}

struct LogServiceResponse {
  response @0: Text;
}

struct LogRecord {
  header @0 :LogRecordHeader;
  realityEngine @1 :RealityEngineLogRecord;
  app @2 :DeprecatedAppLogRecord; # [Deprecated] use RemoteRequest.AppLogRecord
  dwellTime @3 :DwellTime;
  eventCounts @4 :EventCounts;
}

struct LogRecordHeader {
  app @0 :AppLogRecordHeader;
  device @1 :DeviceLogRecordHeader;
  reality @2: RealityEngineLogRecordHeader;
}

struct AppLogRecordHeader {
  enum MobileAppKeyStatus {
    unknown @0;       # Server has not validated the key yet.
    serverInvalid @1; # Server has responded that the key is invalid.
    serverValid @2;   # Server has responded that the key is valid.
    missing @3;       # Developer did not provide a mobile app key.
  }

  enum Hosting {
    unset @0;
    selfHosted @1;
    cloudEditorHosted @2;
    unknown @3;
  }

  enum LoggingReason {
    unspecified @0;
    pageLoad @1;
    pageUnload @2;
    pause @3;
    resume @4;
    start @5;
    stop @6;
    timer @7;
  }

  enum AccessMethod {
    unspecified @0;
    devToken @1;
    public @2;
  }

  # A unique identifier for the app.
  appId @0: Text;

  # A unique app key generated from the developer's 8th Wall account.
  mobileAppKey @1: Text;

  # Status code of the app key in use.
  mobileAppKeyStatus @2: MobileAppKeyStatus;

  # A session identifier to correlate log events across request.
  sessionId @3: Text;

  # Identifier of the link that was used to visit
  referredById @4: Text;

  # Current identifier of the visit that will be used as a referral identifier
  referralId @5: Text;

  # A loggingRatio of 1 means every session was logged, 5 means 1/5 were logged, 100 means 1/100.
  loggingRatio @6: Float32;

  # List of first-party pipeline modules used by the app
  modules @7: List(Text);

  # Whether SIMD is supported
  simdSupported @8: Bool;

  # Whether SIMD is enabled
  simdEnabled @9: Bool;

  # 8th Wall-hosted or Self-hosted
  hosting @10: Hosting;

  # The triggering event for sending this log record.
  loggingReason @11: LoggingReason;

  # Which type of access allowed the session to start
  accessMethod @12: AccessMethod;
}

struct DeviceLogRecordHeader {
  # Unique identifier for the device, if available.
  deviceId @0: Text;

  # Information regarding the type of device in use.
  deviceInfo @1: DeviceInfo;

  # Unique identifier for the device for a particular vendor.
  # For iOS, this is the UIDevice#identifierForVendor field.
  # On Android, it is calculated by hashing all components except for the last with the
  # unique deviceId.
  # e.g. Package name = "com.the8thwall.HelloXR"
  #      deviceId = "1s4f3-3gx43-h7j22"
  #      idForVendor = hash("com.the8thwall" + "1s4f3-3gx43-h7j22")
  idForVendor @2: Text;

  # Locale the device formatted as "{language code}_{country code}".
  locale @3: Text;

  # Unique identifier for the device for a particular application.
  # On Android devices below Oreo, this is calculated by hasing the unique device ID with the
  # application package name.
  # On Android devices on Oreo and above, this is provided by the application-unique device ID
  # given by the Android API.
  idForApp @4: Text;
}

struct RealityEngineLogRecordHeader {
  enum EngineType {
    unknown @0;
    c8 @1;
    arkit @2;
    arcore @3;
    tango @4;
    remoteOnly @5;
    web @6;
  }

  # The type of engine being run on the device.
  engineId @0 :EngineType;

  # The current frame the reality engine is operating on.
  frameId @1 :FrameId;

  # The version of XR built into this app.
  engineVersion @2 :Text;
}

struct FrameId {
  # A unique identifier for this session (one per engine create).
  sessionId @0: Int64;

  # Within a session, a set of tracks are expected to be a set of contiguous frames.
  trackId @1: Int64;

  # Within a track, frames have incrementing frame ids.
  frameId @2: Int64;
}

struct RealityEngineLogRecord {
  request @0 :RealityRequest;
  response @1 :RealityResponse;
  stats @2 :RealityEngineStats;
  environment @3 :RealityEngineEnvironment;
}

struct RealityEngineStats {
  summary @0 :LoggingSummary;
  details @1 :List(LoggingDetail);
}

struct RealityEngineEnvironment {
  realityImageWidth @0 : Int32;
  realityImageHeight @1 : Int32;
  capabilityPositionTracking @2 : Int32;
  capabilitySurfaceEstimation @3 : Int32;
}

# [Deprecated] use RemoteRequest.AppLogRecord
struct DeprecatedAppLogRecord @0xd4201303ceef0b19 {
  xrRemote @0 :DeprecatedXrRemoteApp;
}

# [Deprecated] use RemoteRequest.XrRemoteApp
struct DeprecatedXrRemoteApp @0xdd97eb0693cdbe0e {
  device @0 :DeprecatedXrAppDeviceInfo;
  touches @1 :List(DeprecatedXrTouch);
}

# [Deprecated] use RemoteRequest.XrAppDeviceInfo
struct DeprecatedXrAppDeviceInfo @0xd23668d5584fcdab {
  enum XrDeviceOrientation {
    unspecified @0;
    portrait @1;
    portraitUpsideDown @2;
    landscapeLeft @3;
    landscapeRight @4;
    faceUp @5;
    faceDown @6;
  }

  screenWidth @0 :Int32;
  screenHeight @1 :Int32;
  orientation @2 :XrDeviceOrientation;
}

# [Deprecated] use RemoteRequest.XrTouch
struct DeprecatedXrTouch @0x93ee0eb6831d6391 {
  enum XrTouchPhase {
    unspecified @0;
    began @1;
    moved @2;
    stationary @3;
    ended @4;
    cancelled @5;
  }

  positionX @0 :Float32;  # Screen position in pixels
  positionY @1 :Float32;  # Screen position in pixels
  timestamp @2 :Int64;    # Monotonically increases with touch time
  phase @3 :XrTouchPhase;
  tapCount @4 :Int32;
  fingerId @5 :Int32;
}

struct DwellTime {
  pageMillis @0 :Int64; # Duration on page in milliseconds
  arMillis @1 :Int64; # Duration in AR in milliseconds
  vrMillis @2 :Int64; # Duration in VR in milliseconds
  desktop3DMillis @3 :Int64; # Duration of desktop 3D in milliseconds
  faceEffectTrackingTime @4 :TrackingTime;
  worldEffectMillis @5 :Int64; # Duration of world effects in milliseconds
  imageTargetTrackingTime @6 :TrackingTime;
  curvyImageTargetTrackingTime @7 :TrackingTime;
  cameraMillis @8 :Int64; # Duration of camera usage

  # These roll up into worldEffectMillis
  worldEffectResponsiveMillis @9 :Int64; # Duration of world effects in responsive mode in milliseconds
  worldEffectAbsoluteMillis @10 :Int64; # Duration of world effects in absolute mode in milliseconds

  vpsTrackingTime @12 :VpsTrackingTime;

  # Deprecated tag numbers
  tombstone0 @11 :Int64;
}

struct VpsTrackingTime {
  totalMillis @0 :Int64; # Total duration in milliseconds

  # Time until the first Node is localized: onStart -> first Node localized.
  timeToNodeMillis @1 :Int64;
  # Time spent while we have a Node which is localized. Start timer when we have localized a Node.
  # Stop timer when VPS is reset, or in onDetach.
  nodeMillis @2 :Int64;

  # Time until the first Wayspot Anchor is localized: onStart -> first Wayspot Anchor localized.
  timeToWayspotAnchorMillis @3 :Int64;
  # Time spent while we have a Wayspot Anchor which is localized. Start timer when we have found
  # a Wayspot Anchor. Stop timer when VPS is reset, or in onDetach.
  wayspotAnchorMillis @4 :Int64;

  # Time until VPS mode falls back from device mode to server mode: onStart -> fallback occurred.
  # Set to zero if no fallback occurred during the session.
  timeToFallbackMillis @5 :Int64;
}

struct TrackingTime {
  totalMillis @0 :Int64; # Total duration in milliseconds
  trackingMillis @1 :Int64; # Tracking duration in milliseconds
}

struct EventCounts {
  mediaRecorder @0 :MediaRecorderStats;
  imageTargetsFound @1 :ObjectsFoundStats;
  facesFound @2 :ObjectsFoundStats;
  nodesFound @3 :VpsFoundStats;
  wayspotAnchorsFound @4 :VpsFoundStats;
}

struct MediaRecorderStats {
  sessions @0 :Int64; # Media recorder sessions
  photos @1 :Int64; # Media recorder photos taken
  shares @2 :Int64; # Media recorder shares
}

struct ObjectsFoundStats {
  total @0 :Int64; # Total instances when objects were found
  unique @1 :Int64; # Unique objects found
}

struct VpsFoundStats {
  total @0 :Int64; # Total instances when objects were found
  unique @1 :Int64; # Unique objects found
  updated @2 :Int64; # Total instances when objects were updated
}
