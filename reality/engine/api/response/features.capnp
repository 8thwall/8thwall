@0x99837f9fad2def3e;

using import "../base/geo-types.capnp".Position32f;

using import "status.capnp".ResponseStatus;
using import "../base/feature-types.capnp".FeatureDescriptors;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.response");
$Java.outerClassname("Features");  # Must match this file's name!

struct ResponseFeatureSet {
  status @0 :ResponseStatus;
  points @1 :List(FeaturePoint);
}

struct FeaturePoint {
  id @0: UInt64;
  position @1: Position32f;
  confidence @2: Float32;
}

struct DeprecatedResponseFeatures @0xe091cf981bfb3db9 {
  experimental @0 :ExperimentalResponseFeatures;
  status @1 :ResponseStatus;
  features @2 :FeatureDescriptors;
  previousFeatures @3 :FeatureDescriptors;
  matches @4 :List(FeatureMatch);
}

struct FeatureMatch {
  featuresIndex @0: Int32;
  previousFeaturesIndex @1: Int32;
}

struct ExperimentalResponseFeatures {
  keyframe @0: ExperimentalKeyframesApril2017;
}

struct ExperimentalKeyframesApril2017 {
  keyframeFeatures @0 :FeatureDescriptors;
  keyframeMatches @1 :List(FeatureMatch);
  keyframeLowMatchFrames @2 :Int32;
  framesSinceKeyframe @3 :Int32;
}
