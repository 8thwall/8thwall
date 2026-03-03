@0x95231bf9593576de;

using import "../../api/reality.capnp".RealityRequest;
using import "../../api/reality.capnp".RealityResponse;
using import "../../api/reality.capnp".CoordinateSystemConfiguration;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.executor.proto");
$Java.outerClassname("State");  # Must match this file's name!

struct InternalRealityRequest {
  request @0 :RealityRequest;
  state @1 :InternalRealityState;
}

struct InternalRealityExecutionContext {
  frameNum @0 :Int64;
}

struct InternalRealityState {
  lastCall @0 :InternalRealityPreviousCall;
  executionContext @1 :InternalRealityExecutionContext;
  coordinateConfiguration @2: CoordinateSystemConfiguration;
}

struct InternalRealityPreviousCall {
  request @0 :RealityRequest;
  response @1 :RealityResponse;
}
