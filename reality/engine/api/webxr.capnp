@0xdd186315564fb3cd;

using import "base/camera-intrinsics.capnp".PixelPinholeCameraModel;
using import "base/geo-types.capnp".Transform32f;

# Information received from webxr and the data for the overlay canvas
struct WebXRController {
  canvasData @0: Data;
  textureHeight @1: Int32;
  textureWidth @2: Int32;
  intrinsic @3: PixelPinholeCameraModel;
  extrinsic @4: List(Float32);
  place @5: Transform32f;
  widthInMeters @6: Float32;
  heightInMeters @7: Float32;
}
