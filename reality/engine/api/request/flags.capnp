@0xd8f39f312adcf079;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.request");
$Java.outerClassname("Flags");  # Must match this file's name!

struct RequestFlags {
  experimental @0 :Bool;
  logMask @1 :LogMask;
}

struct LogMask {
  # Log the raw request to the server.
  request @0: Bool;

  # Log an exapnded request (including raw image data!) to the server. This functionality should be
  # used very carefully and only for internal prototyping and QA.
  requestExpanded @1: Bool;

  # Log the response to the server.
  response @2: Bool;
}
