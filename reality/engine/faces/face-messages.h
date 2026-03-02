// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

/**
 * Face-messages is used to convert the Structs in face-types.h into their capnp form defined
 * in face.capnp
 */

#include "c8/geometry/face-types.h"
#include "c8/vector.h"
#include "reality/engine/api/face.capnp.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {

void fillRenderedImage(const FaceRenderResult &r, DebugResponse::Builder b);

void fillDetection(const DetectedPoints &p, DetectedPointsMsg::Builder d);

void fillDetections(
  const Vector<DetectedPoints> &faces,
  const Vector<DetectedPoints> &meshes,
  DebugDetections::Builder b);

void fillEarRenderedImage(const EarRenderResult &r, DebugResponse::Builder b);
void fillEarDetections(
  const Vector<DetectedPoints> &ears,
  const Vector<HPoint3> &earInFaceRoi,
  const Vector<HPoint3> &earInCameraFeed,
  const DebugDetections::Builder b);

void fillFace(FaceMsg::Builder b, const Face3d &faceData);
void fillFaces(FaceResponse::Builder b, const Vector<Face3d> &faceData);

void fillEar(FaceMsg::Builder b, const Ear3d &earsData);
void fillEars(FaceResponse::Builder b, const Vector<Ear3d> &earsData);

void fillIndices(FaceOptions::Reader opts, ModelGeometryMsg::Builder b);

}  // namespace c8
