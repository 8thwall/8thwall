@0xbde4e9de7e9320c2;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.response");
$Java.outerClassname("Status");  # Must match this file's name!

struct ResponseStatus {
  error @0 :ResponseError;
}

struct ResponseError {
  enum ErrorCode {
    unspecified @0;
    invalidRequest @1;
  }
  failed @0 :Bool;
  code @1 :ErrorCode;
  message @2 :Text;
}

