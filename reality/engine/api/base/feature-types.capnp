@0xd7b8033525000839;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("FeatureTypes");  # Must match this file's name!

struct FeatureDescriptors {
  features @0 :List(FeatureKeyPoint);
  descriptors @1 :FeatureDescriptorData;
}

struct FeatureKeyPoint {
  x @0 :Float32;
  y @1 :Float32;
}

struct FeatureDescriptorData {
  rows @0 :Int32;
  cols @1 :Int32;
  bytesPerRow @2 :Int32;
  uInt8FeatureData @3 :Data;
}

