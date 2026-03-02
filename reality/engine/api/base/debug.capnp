@0xd4d5c8154904dc03;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("Debug");  # Must match this file's name!

struct DebugData {
  tag @0: Text;
  data @1: Data;
}
