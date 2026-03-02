@0xf914d313190d84f8;

using import "base/camera-intrinsics.capnp".CameraCoordinates;
using import "base/camera-intrinsics.capnp".GraphicsCalibration;
using import "base/camera-intrinsics.capnp".GraphicsCamera;
using import "base/geo-types.capnp".Box32f;
using import "base/geo-types.capnp".ImageBoxRoi;
using import "base/geo-types.capnp".Position32f;
using import "base/geo-types.capnp".Quaternion32f;
using import "base/image-types.capnp".RGBAImageData;
using import "mesh.capnp".DebugOptions;
using import "mesh.capnp".UVMsg;
using import "mesh.capnp".MeshFaceIdxs;
using import "mesh.capnp".DetectedPointsMsg;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api");
$Java.outerClassname("Face");  # Must match this file's name!

struct FaceOptions {
  debug @0 :DebugOptions;
  nearClip @1 :Float32;
  farClip @2 :Float32;
  coordinates @3 :CameraCoordinates;

  enum MeshGeometryOptions {
    unspecified @0;
    face @1;
    eyes @2;
    mouth @3;
    iris @4;
  }
  meshGeometry @4 :List(MeshGeometryOptions);

  # whether to return the standard or flat uvs.
  useStandardUvs @5 :Bool;

  maxDetections @6 :Int32;

  # whether to also output ears
  enableEars @7 :Bool;
}

struct FaceAnchorTransformMsg {
  position @0 :Position32f;
  rotation @1 :Quaternion32f;
  scale @2 :Float32;
  scaledWidth @3 :Float32;
  scaledHeight @4 :Float32;
  scaledDepth @5 :Float32;
}

# Information from the face framework to the UI about the capabilities of the framework and values
# that will be fixed across detections.
struct ModelGeometryMsg {
  # The maximum number of objects that can be detected at once.
  maxDetections @0 :Int32;
  # The number of vertices in each detected object.
  pointsPerDetection @1 :Int32;
  # Indices of the vertices that form the triangles of the mesh.
  indices @2 :List(MeshFaceIdxs);
  # Texture uvs for each mesh vertex.
  uvs @3 :List(UVMsg);
}

struct DebugDetections {
  faces @0 :List(DetectedPointsMsg);
  meshes @1 :List(DetectedPointsMsg);
  ears @2 :List(DetectedPointsMsg);
  earsInFaceRoi @3 :List(Position32f);
  earsInCameraFeed @4 :List(Position32f);
}

struct DebugResponse {
  # face
  renderedImg @0 :RGBAImageData;
  detections @1 :DebugDetections;
  # ears
  earRenderedImg @2 :RGBAImageData;
}

struct AttachmentPointMsg {
  enum AttachmentName {
    unspecified @0;
    forehead @1;
    rightEyebrowInner @2;
    rightEyebrowMiddle @3;
    rightEyebrowOuter @4;
    leftEyebrowInner @5;
    leftEyebrowMiddle @6;
    leftEyebrowOuter @7;
    leftEar @8;
    rightEar @9;
    leftCheek @10;
    rightCheek @11;
    noseBridge @12;
    noseTip @13;
    leftEye @14;
    rightEye @15;
    leftEyeOuterCorner @16;
    rightEyeOuterCorner @17;
    upperLip @18;
    lowerLip @19;
    mouth @20;
    mouthRightCorner @21;
    mouthLeftCorner @22;
    chin @23;
    leftIris @24;
    rightIris @25;
    leftUpperEyelid @26;
    rightUpperEyelid @27;
    leftLowerEyelid @28;
    rightLowerEyelid @29;
    earLeftHelix @30;
    earLeftCanal @31;
    earLeftLobe @32;
    earRightHelix @33;
    earRightCanal @34;
    earRightLobe @35;
  }
  name @0 :AttachmentName;
  position @1 :Position32f;
  rotation @2 :Quaternion32f;
}

struct FaceMsg {
  enum TrackingStatus {
    unspecified @0;
    found @1;
    updated @2;
    lost @3;
  }
  id @0 :Int32;
  status @1 :TrackingStatus;  # FOUND, UPDATED, LOST
  transform @2 :FaceAnchorTransformMsg;
  vertices @3 :List(Position32f);
  normals @4 :List(Position32f);
  attachmentPoints @5 :List(AttachmentPointMsg);
  uvsInCameraFrame @6 :List(UVMsg);

  mouthOpen @7 :Bool;
  leftEyeOpen @8 :Bool;
  rightEyeOpen @9 :Bool;
  leftEyebrowRaised @10 :Bool;
  rightEyebrowRaised @11 :Bool;

  interpupillaryDistanceInMM @12 :Float32;

  earVertices @13 :List(Position32f);
  earAttachmentPoints @14 :List(AttachmentPointMsg);

  leftEarFound @15 :Bool;
  rightEarFound @16 :Bool;
  leftLobeFound @17 :Bool;
  leftCanalFound @18 :Bool;
  leftHelixFound @19 :Bool;
  rightLobeFound @20 :Bool;
  rightCanalFound @21 :Bool;
  rightHelixFound @22 :Bool;
}

struct FaceResponse {
  debug @0 :DebugResponse;
  camera @1 :GraphicsCamera;
  faces @2 :List(FaceMsg);
}

# Data about the framework that is known at initialization time.
struct FrameworkResponse {
  modelGeometry @0 :ModelGeometryMsg;
}
