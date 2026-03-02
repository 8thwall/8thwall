@0xe474474b8a80961b;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("Mask");  # Must match this file's name!

struct RequestMask {
  sensorTest @0 :Bool;
  pose @1 :Bool;
  features @2 :Bool;
}

