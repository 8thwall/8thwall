@0xaf22a08806cacbaa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.nianticlabs.c8.xrapi");
$Java.outerClassname("XrapiProto");  # Must match this file's name!

struct XrFrameView {
  transform @0: List(Float32);
  projection @1: List(Float32);
}

struct XrFrameData {
  views @0: List(XrFrameView);
  time @1: Float32;
  controllers @2: List(XrController);
  connectEvents @3: List(XrConnectEvent);
}

enum XrConnectionState {
  nochange @0;
  connected @1;
  disconnected @2;
}

struct XrConnectEvent {
  connectionState @0: XrConnectionState = nochange;
  handedness @1: Text = "";
  interactionProfile @2: Text = "";
  isHand @3: Bool = false;
}

struct XrSwapchainSetup {
  width @0: Int32;
  height @1: Int32;
  motionVectorWidth @2: Int32;
  motionVectorHeight @3: Int32;
  depthStencilWidth @4: Int32;
  depthStencilHeight @5: Int32;
}

struct XrPose {
  position @0: List(Float32);
  orientation @1: List(Float32);
}

struct XrJointPose {
  pose @0: XrPose;
  radius @1: Float32;
}

struct XrButtonState {
  value @0: Float32;
  touched @1: Bool;
}

struct XrThumbstickState {
  x @0: Float32;
  y @1: Float32;
  click @2: XrButtonState;
}

struct XrController {
  pose @0: XrPose;
  selectState @1: Float32;
  squeezeState @2: Float32;
  buttonStates @3: List(XrButtonState);
  thumbstickState @4: XrThumbstickState;
  aimPose @5: XrPose;
  isHand @6: Bool;
  jointPoses @7: List(XrJointPose);
}
