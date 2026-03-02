@0xa52655fd50699cde;

using import "./geo-types.capnp".Transform32f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("CameraIntrinsics");  # Must match this file's name!

struct PixelCalibration {
  # The pinhole camera model, in pixel coordinates. The matrix is stored as 16 floats stored in
  # column-major order. This is derivable from a PixelPinholeCameraModel.
  matrix44f @0: List(Float32);
}

struct PixelPinholeCameraModel {
  # Intrinsic parameters used to construct the pinhole camera matrix.
  pixelsWidth @0: Int32;
  pixelsHeight @1: Int32;
  centerPointX @2: Float32;
  centerPointY @3: Float32;
  focalLengthHorizontal @4: Float32;
  focalLengthVertical @5: Float32;
}

struct GraphicsPinholeCameraModel {
  # Intrinsic parameters used to construct the graphics camera matrix from the pixel camera matrix.
  textureWidth @0: Int32;
  textureHeight @1: Int32;
  nearClip @2: Float32;
  farClip @3: Float32;
  digitalZoomHorizontal @4: Float32;
  digitalZoomVertical @5: Float32;
}

struct GraphicsCalibration {
  # The pinhole camera model, in coordinates used in graphics systems like OpenGL, Metal and Unity.
  # The matrix is stored as 16 floats stored in column-major order. This is derivable from a
  # GraphicsPinholeCameraModel and PixelPinholeCameraModel combined.
  matrix44f @0: List(Float32);
}

struct CameraCoordinates {
  enum Axes {
    unspecified @0;
    leftHanded @1;  # x-right, y-up, z-forward
    rightHanded @2;  # x-right, y-up, z-backward
  }
  origin @0: Transform32f;
  scale @1: Float32;
  axes @2 : Axes;
  mirroredDisplay @3: Bool;
}

struct GraphicsCamera {
  extrinsic @0: Transform32f;
  intrinsic @1: GraphicsCalibration;
}
