@0xfd2cc1bd119b9158;

using import "../base/geo-types.capnp".Quaternion32f;
using import "../base/geo-types.capnp".Transform32f;
using import "sensor.capnp".ARCorePlane;
using import "sensor.capnp".ARKitPlaneAnchor;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("HitTest");  # Must match this file's name!

struct XRHitTestResult {
  results @0 :List(XRHitResult);
}

struct XRHitResult {
  enum Type {
    unspecified @0;
    featurePoint @1;
    estimatedHorizontalPlane @2;
    estimatedVerticalPlane @3;
    existingPlane @4;
  }
  type @0 :Type;
  cameraDistance @1 :Float32;
  hitTransform @2 :Transform32f;
  aRKitAnchor @3: ARKitPlaneAnchor;
  aRCoreAnchor @4: ARCorePlane;
}
