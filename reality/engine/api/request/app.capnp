@0xa4caf69427e14845;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("App");  # Must match this file's name!

struct AppContext {
  enum DeviceOrientation {
    unspecified @0;
    portrait @1;
    landscapeLeft @2;
    portraitUpsideDown @3;
    landscapeRight @4;
  }
  enum RealityTextureRotation {
    unspecified @0;
    r0 @1;
    r90 @2;
    r180 @3;
    r270 @4;
  }
  enum DeviceNaturalOrientation {
    unspecified @0;
    portraitUp @1;
    landscapeUp @2;
  }
  deviceOrientation @0 :DeviceOrientation;
  realityTextureRotation @1 :RealityTextureRotation;
  naturalOrientation @2 :DeviceNaturalOrientation;
}

