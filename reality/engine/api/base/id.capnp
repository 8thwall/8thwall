@0xbaca8a43f5d8840b;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.response");
$Java.outerClassname("Id");  # Must match this file's name!

struct EventId {
  eventTimeMicros @0: Int64;
}
