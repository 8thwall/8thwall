@0xb0ecf7ee84852c8f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.device");
$Java.outerClassname("Info");  # Must match this file's name!

struct DeviceInfo {
  manufacturer @0: Text;
  model @1 :Text;
  os @2 :Text;
  osVersion @3 :Text;
}
