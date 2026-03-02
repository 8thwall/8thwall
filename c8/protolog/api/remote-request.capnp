@0xa210a1e72403ba24;

using LogRequest = import "log-request.capnp";
using import "../../../reality/engine/api/reality.capnp".RealityResponse;
using import "../../../reality/engine/api/reality.capnp".XRConfiguration;
using import "../../../reality/engine/api/base/image-types.capnp".CompressedImageData;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.protolog.api");
$Java.outerClassname("RemoteRequest");  # Must match this file's name!

using Cs = import "/capnp/cs.capnp";
$Cs.namespace("C8");

struct RemoteServiceRequest {
  # latest image and all previous sensor data
  records @0 :List(XrRemoteRequest);
}

struct RemoteServiceResponse {
  record @0 :XrRemoteResponse;
}

struct XrRemoteRequest {
  header @0 :LogRequest.LogRecordHeader;
  realityEngine @1 :LogRequest.RealityEngineLogRecord;
  xrRemote @2 :XrRemoteApp;
}

struct XrRemoteResponse {
  header @0 :XrRemoteResponseHeader;
  realityResponse @1 :RealityResponse;
  engineStats @2 :LogRequest.RealityEngineStats;
  screenPreview @3 :CompressedImageData;
  newXrConfig @4 :XRConfiguration;
}

struct XrRemoteResponseHeader {
  timestampNanos @0 :Int64;
  app @1 :LogRequest.AppLogRecordHeader;
}

struct XrRemoteConnection {
  availableServers @0 :List(XrServer);
  connectedServer @1 :XrConnectedServer;
}

struct XrServer {
  enum InterfaceKind {
    unspecified @0;
    wifi @1;
    usb @2;
  }
  enum ResolveStrategy {
    unspecified @0;
    ipv4 @1;
    mdns @2;
  }
  id @0: Int64;
  displayName @1: Text;

  interfaceKind @2: InterfaceKind;
  resolveStrategy @3: ResolveStrategy;

  ipV4Address @4: Text;
  port @5:Int16;
}

struct XrConnectedServer {
  enum XrConnectionStatus {
    unspecified @0;
    connecting @1;
    connected @2;
    closed @3;
    connectingFailed @4;
  }
  server @0:XrServer;
  status @1:XrConnectionStatus;
}

struct XrServerList {
  servers @0 :List(Text);
}

struct XrRemoteApp {
  device @0 :XrAppDeviceInfo;
  touches @1 :List(XrTouch);
}

struct XrEditorAppInfo {
  screenOrientation @0 :XrAppDeviceInfo.XrScreenOrientation;
  screenPreview @1 :CompressedImageData;
}

struct XrAppDeviceInfo {
  enum XrDeviceOrientation {
    unspecified @0;
    portrait @1;
    portraitUpsideDown @2;
    landscapeLeft @3;
    landscapeRight @4;
    faceUp @5;
    faceDown @6;
  }
  enum XrScreenOrientation {
    unspecified @0;
    portrait @1;
    portraitUpsideDown @2;
    landscapeLeft @3;
    landscapeRight @4;
  }

  screenWidth @0 :Int32;
  screenHeight @1 :Int32;
  orientation @2 :XrDeviceOrientation;
  screenOrientation @3 :XrScreenOrientation;
}

struct XrTouch {
  enum XrTouchPhase {
    unspecified @0;
    began @1;
    moved @2;
    stationary @3;
    ended @4;
    cancelled @5;
  }

  positionX @0 :Float32;  # Screen position in pixels
  positionY @1 :Float32;  # Screen position in pixels
  timestamp @2 :Int64;    # Monotonically increases with touch time
  phase @3 :XrTouchPhase;
  tapCount @4 :Int32;
  fingerId @5 :Int32;
}
