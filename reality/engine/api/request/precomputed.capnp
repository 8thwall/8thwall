@0xfefff98625abb0d2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("Precomputed");  # Must match this file's name!

struct PrecomputedEngineData {
  trackerInputPointer @0: Int64;
  frameTrackerOutputPointer @1: Int64;
}
