@0xf15c6d4ae6aadb15;

using import "status.capnp".ResponseStatus;
using import "../base/geo-types.capnp".Quaternion32f;
using import "../base/geo-types.capnp".Transform32f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.response");
$Java.outerClassname("Pose");  # Must match this file's name!

struct XrTrackingState {
  enum XrTrackingStatus {
    unspecified @0;
    notAvailable @1;
    limited @2;
    normal @3;
  }
  enum XrTrackingStatusReason {
    unspecified @0;
    initializing @1;
    relocalizing @2;
    tooMuchMotion @3;
    notEnoughTexture @4;
  }
  status @0: XrTrackingStatus;
  reason @1: XrTrackingStatusReason;
  scale @2: Float32;
}

struct ResponsePose {
   experimental @0 :ExperimentalResponsePose;
   status @1 :ResponseStatus;
   transform @2 :Transform32f;
   transformDelta @3 :Transform32f;
   # Positional and angular velocity.
   velocity @4 :Transform32f;
   # Initial orientation of the camera with respect to the x-z axis.
   initialTransform @6 :Transform32f;
   trackingState @7 :XrTrackingState;

   # Deprecated tag numbers
   tombstone0 @5 :Quaternion32f;  # Deprecated!
}

struct ExperimentalResponsePose {
  keyframe @0: ExperimentalPoseKeyframesMay2017;

  # Matches tamExtrinsic_ in tracker. Not impacted by yaw offset and recenter pose.
  vioTrackerTransform @1 :Transform32f;
}

struct ExperimentalPoseKeyframesMay2017 {
  keyframeTransform @0 :Transform32f;
  keyframeTransformDelta @1 :Transform32f;
}

