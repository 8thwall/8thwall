// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "face-messages.h",
  };
  deps = {
    ":face-geometry",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:mesh-types",
    "//c8/protolog:xr-requests",
    "//reality/engine/ears:ear-types",
    "//reality/engine/api:face.capnp-cc",
    "//reality/engine/api/base:camera-intrinsics.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}

cc_end(0x94c879bf);

#include <unordered_map>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh-types.h"
#include "c8/protolog/xr-requests.h"
#include "c8/vector.h"
#include "reality/engine/api/face.capnp.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/faces/face-messages.h"

namespace c8 {

namespace {
void setAttachmentPoint(
  const AttachmentPoint &anchor, AttachmentPointMsg::Builder attachmentPoint) {
  setPosition32f(
    anchor.position.x(), anchor.position.y(), anchor.position.z(), attachmentPoint.getPosition());
  setQuaternion32f(
    anchor.rotation.w(),
    anchor.rotation.x(),
    anchor.rotation.y(),
    anchor.rotation.z(),
    attachmentPoint.getRotation());

  attachmentPoint.setName(anchor.name);
}
}  // namespace

void fillRenderedImage(const FaceRenderResult &r, DebugResponse::Builder b) {
  auto im = r.rawPixels;
  b.getRenderedImg().setRows(im.rows());
  b.getRenderedImg().setCols(im.cols());
  b.getRenderedImg().setBytesPerRow(im.rowBytes());
  b.getRenderedImg().initUInt8PixelData(im.rows() * im.rowBytes());
  std::memcpy(
    b.getRenderedImg().getUInt8PixelData().begin(), im.pixels(), im.rows() * im.rowBytes());
}

void setImagePoint(HPoint2 pt, ImagePoint32f::Builder p) {
  p.setX(pt.x());
  p.setY(pt.y());
}

void fillDetection(const DetectedPoints &p, DetectedPointsMsg::Builder d) {
  d.setConfidence(p.confidence);
  d.getViewport().setX(p.viewport.x);
  d.getViewport().setY(p.viewport.y);
  d.getViewport().setW(p.viewport.w);
  d.getViewport().setH(p.viewport.h);

  setImagePoint(p.boundingBox.upperLeft, d.getRoi().getCorners().getUpperLeft());
  setImagePoint(p.boundingBox.upperRight, d.getRoi().getCorners().getUpperRight());
  setImagePoint(p.boundingBox.lowerLeft, d.getRoi().getCorners().getLowerLeft());
  setImagePoint(p.boundingBox.lowerRight, d.getRoi().getCorners().getLowerRight());

  auto transform = renderTexToImageTex(p.roi);
  d.getRoi().initRenderTexToImageTexMatrix44f(16);
  for (int i = 0; i < 16; ++i) {
    d.getRoi().getRenderTexToImageTexMatrix44f().set(i, transform.data()[i]);
  }

  auto dps = d.initPoints(p.points.size());
  for (int j = 0; j < p.points.size(); ++j) {
    auto dp = dps[j];
    auto pp = p.points[j];
    dp.setX(pp.x());
    dp.setY(pp.y());
    dp.setZ(pp.z());
  }

  d.setDetectedClass(p.detectedClass);
}

void fillDetections(
  const Vector<DetectedPoints> &faces,
  const Vector<DetectedPoints> &meshes,
  DebugDetections::Builder b) {
  auto fs = b.initFaces(faces.size());
  for (int i = 0; i < faces.size(); ++i) {
    fillDetection(faces[i], fs[i]);
  }
  auto ms = b.initMeshes(meshes.size());
  for (int i = 0; i < meshes.size(); ++i) {
    fillDetection(meshes[i], ms[i]);
  }
}

void fillEarDetections(
  const Vector<DetectedPoints> &ears,
  const Vector<HPoint3> &earInFaceRoi,
  const Vector<HPoint3> &earInCameraFeed,
  DebugDetections::Builder b) {
  auto es = b.initEars(ears.size());
  for (int i = 0; i < ears.size(); ++i) {
    fillDetection(ears[i], es[i]);
  }

  auto ef = b.initEarsInFaceRoi(earInFaceRoi.size());
  for (int i = 0; i < earInFaceRoi.size(); ++i) {
    auto dp = ef[i];
    auto pp = earInFaceRoi[i];
    dp.setX(pp.x());
    dp.setY(pp.y());
    dp.setZ(pp.z());
  }

  auto ec = b.initEarsInCameraFeed(earInCameraFeed.size());
  for (int i = 0; i < earInFaceRoi.size(); ++i) {
    auto dp = ec[i];
    auto pp = earInCameraFeed[i];
    dp.setX(pp.x());
    dp.setY(pp.y());
    dp.setZ(pp.z());
  }
}

void fillFace(FaceMsg::Builder b, const Face3d &faceData) {
  b.setId(faceData.id);

  switch (faceData.status) {
    case Face3d::TrackingStatus::FOUND: {
      b.setStatus(FaceMsg::TrackingStatus::FOUND);
      break;
    }
    case Face3d::TrackingStatus::UPDATED: {
      b.setStatus(FaceMsg::TrackingStatus::UPDATED);
      break;
    }
    case Face3d::TrackingStatus::LOST: {
      b.setStatus(FaceMsg::TrackingStatus::LOST);
      break;
    }
    default:
      // Don't specify status.
      break;
  }

  if (!(faceData.status == Face3d::TrackingStatus::FOUND
        || faceData.status == Face3d::TrackingStatus::UPDATED)) {
    return;
  }

  b.getTransform().getPosition().setX(faceData.transform.position.x());
  b.getTransform().getPosition().setY(faceData.transform.position.y());
  b.getTransform().getPosition().setZ(faceData.transform.position.z());

  b.getTransform().getRotation().setW(faceData.transform.rotation.w());
  b.getTransform().getRotation().setX(faceData.transform.rotation.x());
  b.getTransform().getRotation().setY(faceData.transform.rotation.y());
  b.getTransform().getRotation().setZ(faceData.transform.rotation.z());

  b.getTransform().setScale(faceData.transform.scale);
  b.getTransform().setScaledWidth(faceData.transform.scaledWidth);
  b.getTransform().setScaledHeight(faceData.transform.scaledHeight);
  b.getTransform().setScaledDepth(faceData.transform.scaledDepth);

  auto vertices = b.initVertices(faceData.vertices.size());
  for (int i = 0; i < faceData.vertices.size(); ++i) {
    vertices[i].setX(faceData.vertices[i].x());
    vertices[i].setY(faceData.vertices[i].y());
    vertices[i].setZ(faceData.vertices[i].z());
  }

  auto normals = b.initNormals(faceData.normals.size());
  for (int i = 0; i < faceData.vertices.size(); ++i) {
    normals[i].setX(faceData.normals[i].x());
    normals[i].setY(faceData.normals[i].y());
    normals[i].setZ(faceData.normals[i].z());
  }

  // Set uvs in camera frame.
  auto uvsInCameraSpace = b.initUvsInCameraFrame(faceData.uvsInCameraSpace.size());
  for (int i = 0; i < faceData.uvsInCameraSpace.size(); ++i) {
    uvsInCameraSpace[i].setU(faceData.uvsInCameraSpace[i].x());
    uvsInCameraSpace[i].setV(faceData.uvsInCameraSpace[i].y());
  }

  // Set anchors
  auto attachmentPoints = b.initAttachmentPoints(faceData.attachmentPoints.size());
  for (int i = 0; i < faceData.attachmentPoints.size(); ++i) {
    const auto &anchor = faceData.attachmentPoints[i];
    setPosition32f(
      anchor.position.x(),
      anchor.position.y(),
      anchor.position.z(),
      attachmentPoints[i].getPosition());
    setQuaternion32f(
      anchor.rotation.w(),
      anchor.rotation.x(),
      anchor.rotation.y(),
      anchor.rotation.z(),
      attachmentPoints[i].getRotation());

    attachmentPoints[i].setName(anchor.name);
  }

  b.setMouthOpen(faceData.expressionOutput.mouthOpen);
  b.setLeftEyeOpen(faceData.expressionOutput.leftEyeOpen);
  b.setRightEyeOpen(faceData.expressionOutput.rightEyeOpen);
  b.setLeftEyebrowRaised(faceData.expressionOutput.leftEyebrowRaised);
  b.setRightEyebrowRaised(faceData.expressionOutput.rightEyebrowRaised);

  b.setInterpupillaryDistanceInMM(faceData.interpupillaryDistanceInMM);
}

void fillFaces(FaceResponse::Builder b, const Vector<Face3d> &faceData) {
  auto faces = b.initFaces(faceData.size());
  for (int i = 0; i < faceData.size(); ++i) {
    fillFace(faces[i], faceData[i]);
  }
}

void fillEar(FaceMsg::Builder b, const Ear3d &earsData) {
  auto earVertices =
    b.initEarVertices(earsData.leftVertices.size() + earsData.rightVertices.size());
  {
    int j = 0;
    for (const auto &v : earsData.leftVertices) {
      earVertices[j].setX(v.x());
      earVertices[j].setY(v.y());
      earVertices[j].setZ(v.z());
      ++j;
    }
    for (const auto &v : earsData.rightVertices) {
      earVertices[j].setX(v.x());
      earVertices[j].setY(v.y());
      earVertices[j].setZ(v.z());
      ++j;
    }
  }

  // Set anchors
  auto attachmentPoints = b.initEarAttachmentPoints(
    earsData.leftAttachmentPoints.size() + earsData.rightAttachmentPoints.size());

  {
    int j = 0;
    for (const auto &anchor : earsData.leftAttachmentPoints) {
      setAttachmentPoint(anchor, attachmentPoints[j]);
      ++j;
    }

    for (const auto &anchor : earsData.rightAttachmentPoints) {
      setAttachmentPoint(anchor, attachmentPoints[j]);
      ++j;
    }
  }

  // Set ear states
  b.setLeftEarFound(earsData.earStatesOutput.leftEarFound);
  b.setRightEarFound(earsData.earStatesOutput.rightEarFound);
  b.setLeftLobeFound(earsData.earStatesOutput.leftLobeFound);
  b.setLeftCanalFound(earsData.earStatesOutput.leftCanalFound);
  b.setLeftHelixFound(earsData.earStatesOutput.leftHelixFound);
  b.setRightLobeFound(earsData.earStatesOutput.rightLobeFound);
  b.setRightCanalFound(earsData.earStatesOutput.rightCanalFound);
  b.setRightHelixFound(earsData.earStatesOutput.rightHelixFound);
}

void fillEars(FaceResponse::Builder b, const Vector<Ear3d> &earsData) {
  // We assume that faces have been filled
  auto faces = b.getFaces();
  if (faces.size() < earsData.size()) {
    C8_THROW("[face-messages] FaceResponse has fewer faces than required.");
  }
  for (int i = 0; i < earsData.size(); ++i) {
    fillEar(faces[i], earsData[i]);
  }
}

// The user passes in their specifications of what parts of the facemesh they want via the
// configuration option meshGeometry, which is by default ['face'].  This function will add the
// indices based on their configurations.
void fillIndices(FaceOptions::Reader opts, ModelGeometryMsg::Builder b) {
  FaceMeshGeometryConfig config;

  for (const auto geometryOption : opts.getMeshGeometry()) {
    if (geometryOption == FaceOptions::MeshGeometryOptions::FACE) {
      config.showFace = true;
    }
    if (geometryOption == FaceOptions::MeshGeometryOptions::EYES) {
      config.showEyes = true;
    }
    if (geometryOption == FaceOptions::MeshGeometryOptions::MOUTH) {
      config.showMouth = true;
    }
    if (geometryOption == FaceOptions::MeshGeometryOptions::IRIS) {
      config.showIris = true;
    }
  }

  const auto userIndices = meshIndicesFromMeshGeometry(config);
  auto fs = b.initIndices(userIndices.size());

  for (int i = 0; i < userIndices.size(); ++i) {
    fs[i].setA(userIndices[i].a);
    fs[i].setB(userIndices[i].b);
    fs[i].setC(userIndices[i].c);
  }
}

void fillEarRenderedImage(const EarRenderResult &r, DebugResponse::Builder b) {
  auto im = r.rawPixels;
  b.getEarRenderedImg().setRows(im.rows());
  b.getEarRenderedImg().setCols(im.cols());
  b.getEarRenderedImg().setBytesPerRow(im.rowBytes());
  b.getEarRenderedImg().initUInt8PixelData(im.rows() * im.rowBytes());
  std::memcpy(
    b.getEarRenderedImg().getUInt8PixelData().begin(), im.pixels(), im.rows() * im.rowBytes());
}

}  // namespace c8
