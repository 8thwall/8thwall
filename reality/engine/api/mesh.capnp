@0x8c188b53f548c92d;

using import "base/geo-types.capnp".Box32f;
using import "base/geo-types.capnp".ImageBoxRoi;
using import "base/geo-types.capnp".Position32f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api");
$Java.outerClassname("Mesh");  # Must match this file's name!

struct ExtraOutputMask {
  renderedImg @0 :Bool;
  detections @1 :Bool;
  earRenderedImg @2 :Bool;
  earDetections @3 :Bool;
}

struct DebugOptions {
  # Provide extra info to the UI for debugging purposes, at in
  extraOutput @0 :ExtraOutputMask;
  onlyDetectFaces @1 :Bool;
}

# Data about one detected object and its associated points.
struct DetectedPointsMsg {
  # How confident are we in this detected object (0-1).
  confidence @0 :Float32;
  # Region of the rendered image that was processed. Points are relative to this viewport.
  viewport @1 :Box32f;
  # Where in the source image did the data in the viewport come from? Bounding box of detection
  # result, relative to the rendered viewport, in 0-1, with info about how to transform back to
  # image space.
  roi @2 :ImageBoxRoi;
  # Location of detected points, relative to the viewport, in 0-1.
  points @3 :List(Position32f);
  # Detected class of the object. In Hand detection, this value reflects left or right hand.
  detectedClass @4 :Int32;
}

# Index into a vertex array of the three vertices of a triangle.
struct MeshFaceIdxs {
  a @0 :Int32;
  b @1 :Int32;
  c @2 :Int32;
}

struct UVMsg {
  u @0 :Float32;
  v @1 :Float32;
}
