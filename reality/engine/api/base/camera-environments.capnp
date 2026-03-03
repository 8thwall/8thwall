@0xa45111b381c78237;

using import "../device/info.capnp".DeviceInfo;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("CameraEnvironments");  # Must match this file's name!


# Info about the processing environment, including device model, orientation, and video dimension.
struct CameraEnvironment {
  # Dimensions of the camera feed.
  cameraWidth @0 :Int32;
  cameraHeight @1 :Int32;
  # Dimensions of the drawing surface.
  displayWidth @2 :Int32;
  displayHeight @3 :Int32;
  # Rotation of the phone from portrait, in -90, 0, 90, or 180.
  orientation @4 :Int32;
  # Device Info (manufacturer / model / os / osVersion)
  deviceEstimate @5 :DeviceInfo;
}
