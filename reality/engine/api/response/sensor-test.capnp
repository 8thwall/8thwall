@0xbfcfd7bfb3c5fb2a;

using import "status.capnp".ResponseStatus;
using import "../base/geo-types.capnp".Position32f;
using import "../base/geo-types.capnp".Quaternion32f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.response");
$Java.outerClassname("SensorTest");  # Must match this file's name!

struct ResponseSensorTest {
  status @0 :ResponseStatus;
  camera @1 :CameraTest;
  pose @2 :PoseTest;
}

struct CameraTest {
  meanPixelValue @0 :Float64;
}

struct PoseTest {
  devicePose @0: Quaternion32f;
  deviceAcceleration @1: Position32f;
}
